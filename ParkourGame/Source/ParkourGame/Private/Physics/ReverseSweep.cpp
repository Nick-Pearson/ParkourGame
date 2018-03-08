#include "ReverseSweep.h"

#include "Engine/World.h"
#include "Runtime/Engine/Private/Collision/PhysXCollision.h"
#include "Runtime/Engine/Private/PhysicsEngine/PhysXSupport.h"

#pragma region STD PHYSX CODE
#if WITH_EDITOR

namespace ECollisionQuery
{
	enum Type
	{
		ObjectQuery = 0,
		TraceQuery = 1
	};
}

PxQueryHitType::Enum FPxQueryFilterCallback::CalcQueryHitType(const PxFilterData &PQueryFilter, const PxFilterData &PShapeFilter, bool bPreFilter)
{
	ECollisionQuery::Type QueryType = (ECollisionQuery::Type)PQueryFilter.word0;
	FMaskFilter QuerierMaskFilter;
	const ECollisionChannel QuerierChannel = GetCollisionChannelAndExtraFilter(PQueryFilter.word3, QuerierMaskFilter);

	FMaskFilter ShapeMaskFilter;
	const ECollisionChannel ShapeChannel = GetCollisionChannelAndExtraFilter(PShapeFilter.word3, ShapeMaskFilter);

	if ((QuerierMaskFilter & ShapeMaskFilter) != 0)	//If ignore mask hit something, ignore it
	{
		return PxQueryHitType::eNONE;
	}

	const PxU32 ShapeBit = ECC_TO_BITFIELD(ShapeChannel);
	if (QueryType == ECollisionQuery::ObjectQuery)
	{
		const int32 MultiTrace = (int32)QuerierChannel;
		// do I belong to one of objects of interest?
		if (ShapeBit & PQueryFilter.word1)
		{
			if (bPreFilter)	//In the case of an object query we actually want to return all object types (or first in single case). So in PreFilter we have to trick physx by not blocking in the multi case, and blocking in the single case.
			{

				return MultiTrace ? PxQueryHitType::eTOUCH : PxQueryHitType::eBLOCK;
			}
			else
			{
				return PxQueryHitType::eBLOCK;	//In the case where an object query is being resolved for the user we just return a block because object query doesn't have the concept of overlap at all and block seems more natural
			}
		}
}
	else
	{
		// Then see if the channel wants to be blocked
		// @todo delete once we fix up object/trace APIs to work separate
		PxU32 ShapeFlags = PShapeFilter.word3 & 0xFFFFFF;
		bool bStaticShape = ((ShapeFlags & EPDF_StaticShape) != 0);

		// if query channel is Touch All, then just return touch
		if (QuerierChannel == ECC_OverlapAll_Deprecated)
		{
			return PxQueryHitType::eTOUCH;
		}
		// @todo delete once we fix up object/trace APIs to work separate

		PxU32 const QuerierBit = ECC_TO_BITFIELD(QuerierChannel);
		PxQueryHitType::Enum QuerierHitType = PxQueryHitType::eNONE;
		PxQueryHitType::Enum ShapeHitType = PxQueryHitType::eNONE;

		// check if Querier wants a hit
		if ((QuerierBit & PShapeFilter.word1) != 0)
		{
			QuerierHitType = PxQueryHitType::eBLOCK;
		}
		else if ((QuerierBit & PShapeFilter.word2) != 0)
		{
			QuerierHitType = PxQueryHitType::eTOUCH;
		}

		if ((ShapeBit & PQueryFilter.word1) != 0)
		{
			ShapeHitType = PxQueryHitType::eBLOCK;
		}
		else if ((ShapeBit & PQueryFilter.word2) != 0)
		{
			ShapeHitType = PxQueryHitType::eTOUCH;
		}

		// return minimum agreed-upon interaction
		return FMath::Min(QuerierHitType, ShapeHitType);
	}

	return PxQueryHitType::eNONE;
}

PxQueryHitType::Enum FPxQueryFilterCallback::preFilter(const PxFilterData& filterData, const PxShape* shape, const PxRigidActor* actor, PxHitFlags& queryFlags)
{

	ensureMsgf(shape, TEXT("Invalid shape encountered in FPxQueryFilterCallback::preFilter, actor: %p, filterData: %x %x %x %x"), actor, filterData.word0, filterData.word1, filterData.word2, filterData.word3);

	if (!shape)
	{
		// Early out to avoid crashing.
		return (PrefilterReturnValue = PxQueryHitType::eNONE);
	}

	// Check if the shape is the right complexity for the trace 
	const PxFilterData ShapeFilter = shape->getQueryFilterData();

#define ENABLE_PREFILTER_LOGGING 0
#if ENABLE_PREFILTER_LOGGING
	static bool bLoggingEnabled = false;
	if (bLoggingEnabled)
	{
		FBodyInstance* BodyInst = FPhysxUserData::Get<FBodyInstance>(actor->userData);
		if (BodyInst && BodyInst->OwnerComponent.IsValid())
		{
			UE_LOG(LogCollision, Warning, TEXT("[PREFILTER] against %s[%s] : About to check "),
				(BodyInst->OwnerComponent.Get()->GetOwner()) ? *BodyInst->OwnerComponent.Get()->GetOwner()->GetName() : TEXT("NO OWNER"),
				*BodyInst->OwnerComponent.Get()->GetName());
		}

		UE_LOG(LogCollision, Warning, TEXT("ShapeFilter : %x %x %x %x"), ShapeFilter.word0, ShapeFilter.word1, ShapeFilter.word2, ShapeFilter.word3);
		UE_LOG(LogCollision, Warning, TEXT("FilterData : %x %x %x %x"), filterData.word0, filterData.word1, filterData.word2, filterData.word3);
	}
#endif // ENABLE_PREFILTER_LOGGING

	// Shape : shape's Filter Data
	// Querier : filterData that owns the trace
	PxU32 ShapeFlags = ShapeFilter.word3 & 0xFFFFFF;
	PxU32 QuerierFlags = filterData.word3 & 0xFFFFFF;
	PxU32 CommonFlags = ShapeFlags & QuerierFlags;

	// First check complexity, none of them matches
	if (!(CommonFlags & EPDF_SimpleCollision) && !(CommonFlags & EPDF_ComplexCollision))
	{
		return (PrefilterReturnValue = PxQueryHitType::eNONE);
	}

	PxQueryHitType::Enum Result = FPxQueryFilterCallback::CalcQueryHitType(filterData, ShapeFilter, true);

	if (Result == PxQueryHitType::eTOUCH && bIgnoreTouches)
	{
		Result = PxQueryHitType::eNONE;
	}

	if (Result == PxQueryHitType::eBLOCK && bIgnoreBlocks)
	{
		Result = PxQueryHitType::eNONE;
	}

	// If not already rejected, check ignore actor and component list.
	if (Result != PxQueryHitType::eNONE)
	{
		// See if we are ignoring the actor this shape belongs to (word0 of shape filterdata is actorID)
		if (IgnoreActors.Contains(ShapeFilter.word0))
		{
			//UE_LOG(LogTemp, Log, TEXT("Ignoring Actor: %d"), ShapeFilter.word0);
			Result = PxQueryHitType::eNONE;
		}

		// We usually don't have ignore components so we try to avoid the virtual getSimulationFilterData() call below. 'word2' of shape sim filter data is componentID.
		if (IgnoreComponents.Num() > 0 && IgnoreComponents.Contains(shape->getSimulationFilterData().word2))
		{
			//UE_LOG(LogTemp, Log, TEXT("Ignoring Component: %d"), shape->getSimulationFilterData().word2);
			Result = PxQueryHitType::eNONE;
		}
	}

#if ENABLE_PREFILTER_LOGGING
	if (bLoggingEnabled)
	{
		FBodyInstance* BodyInst = FPhysxUserData::Get<FBodyInstance>(actor->userData);
		if (BodyInst && BodyInst->OwnerComponent.IsValid())
		{
			ECollisionChannel QuerierChannel = GetCollisionChannel(filterData.word3);
			UE_LOG(LogCollision, Log, TEXT("[PREFILTER] Result for Querier [CHANNEL: %d, FLAG: %x] %s[%s] [%d]"),
				(int32)QuerierChannel, QuerierFlags,
				(BodyInst->OwnerComponent.Get()->GetOwner()) ? *BodyInst->OwnerComponent.Get()->GetOwner()->GetName() : TEXT("NO OWNER"),
				*BodyInst->OwnerComponent.Get()->GetName(), (int32)Result);
		}
	}
#endif // ENABLE_PREFILTER_LOGGING

	if (bIsOverlapQuery && Result == PxQueryHitType::eBLOCK)
	{
		Result = PxQueryHitType::eTOUCH;	//In the case of overlaps, physx only understands touches. We do this at the end to ensure all filtering logic based on block vs overlap is correct
	}

	return  (PrefilterReturnValue = Result);
}

PxQueryHitType::Enum FPxQueryFilterCallbackSweep::postFilter(const PxFilterData& filterData, const PxQueryHit& hit)
{

	PxSweepHit& SweepHit = (PxSweepHit&)hit;
	const bool bIsOverlap = SweepHit.hadInitialOverlap();

	if (bIsOverlap && DiscardInitialOverlaps)
	{
		return PxQueryHitType::eNONE;
	}
	else
	{
		if (bIsOverlap && PrefilterReturnValue == PxQueryHitType::eBLOCK)
		{
			// We want to keep initial blocking overlaps and continue the sweep until a non-overlapping blocking hit.
			// We will later report this hit as a blocking hit when we compute the hit type (using FPxQueryFilterCallback::CalcQueryHitType).
			return PxQueryHitType::eTOUCH;
		}

		return PrefilterReturnValue;
	}
}

#endif

struct FScopedMultiSceneReadLock
{
	FScopedMultiSceneReadLock()
	{
		for (int32 i = 0; i < ARRAY_COUNT(SceneLocks); i++)
		{
			SceneLocks[i] = nullptr;
		}
	}

	~FScopedMultiSceneReadLock()
	{
		UnlockAll();
	}

	inline void LockRead(const UWorld* World, PxScene* Scene, EPhysicsSceneType SceneType)
	{
		checkSlow(SceneLocks[SceneType] == nullptr); // no nested locks allowed.
		SCENE_LOCK_READ(Scene);
		SceneLocks[SceneType] = Scene;
	}

	inline void UnlockRead(PxScene* Scene, EPhysicsSceneType SceneType)
	{
		checkSlow(SceneLocks[SceneType] == Scene || SceneLocks[SceneType] == nullptr);
		SCENE_UNLOCK_READ(Scene);
		SceneLocks[SceneType] = nullptr;
	}

	inline void UnlockAll()
	{
		for (int32 i = 0; i < ARRAY_COUNT(SceneLocks); i++)
		{
			SCENE_UNLOCK_READ(SceneLocks[i]);
			SceneLocks[i] = nullptr;
		}
	}

	PxScene* SceneLocks[PST_MAX];
};

PxQueryFlags pk_StaticDynamicQueryFlags(const FCollisionQueryParams& Params)
{
	switch (Params.MobilityType)
	{
	case EQueryMobilityType::Any: return  PxQueryFlag::eSTATIC | PxQueryFlag::eDYNAMIC;
	case EQueryMobilityType::Static: return  PxQueryFlag::eSTATIC;
	case EQueryMobilityType::Dynamic: return  PxQueryFlag::eDYNAMIC;
	default: check(0);
	}

	check(0);
	return PxQueryFlag::eSTATIC | PxQueryFlag::eDYNAMIC;
}

template<typename HitType>
class FDynamicHitBuffer : public PxHitCallback<HitType>
{
private:
	/** Hit buffer used to provide hits via processTouches */
	TTypeCompatibleBytes<HitType> HitBuffer[HIT_BUFFER_SIZE];

	/** Hits encountered. Can be larger than HIT_BUFFER_SIZE */
	TArray<TTypeCompatibleBytes<HitType>, TInlineAllocator<HIT_BUFFER_SIZE>> Hits;

public:
	FDynamicHitBuffer()
		: PxHitCallback<HitType>((HitType*)HitBuffer, HIT_BUFFER_SIZE)
	{}

	virtual PxAgain processTouches(const HitType* buffer, PxU32 nbHits) override
	{
		Hits.Append((TTypeCompatibleBytes<HitType>*)buffer, nbHits);
		return true;
	}

	virtual void finalizeQuery() override
	{
		if (this->hasBlock)
		{
			// copy blocking hit to hits
			processTouches(&this->block, 1);
		}
	}

	FORCEINLINE int32 GetNumHits() const
	{
		return Hits.Num();
	}

	FORCEINLINE HitType* GetHits()
	{
		return (HitType*)Hits.GetData();
	}
};

#pragma endregion

#pragma region REVERSE SWEEP

PxVec3 GeomReverseSweep_PhysX(const UWorld* World, const PxGeometry& PGeom, const PxQuat& PGeomRot, FVector Start, FVector End, ECollisionChannel TraceChannel, const FCollisionQueryParams& Params, const struct FCollisionResponseParams& ResponseParams, const struct FCollisionObjectQueryParams& ObjectParams);

FVector GeomReverseSweep(const UWorld* World, const struct FCollisionShape& CollisionShape, const FQuat& Rot, FVector Start, FVector End, ECollisionChannel TraceChannel, const struct FCollisionQueryParams& Params, const struct FCollisionResponseParams& ResponseParams, const struct FCollisionObjectQueryParams& ObjectParams)
{
	if ((World == NULL) || (World->GetPhysicsScene() == NULL))
	{
		return FVector::ZeroVector;
	}

	FVector POut = FVector::ZeroVector;

	FPhysXShapeAdaptor ShapeAdaptor(Rot, CollisionShape);
	const PxGeometry& PGeom = ShapeAdaptor.GetGeometry();
	const PxQuat& PGeomRot = ShapeAdaptor.GetGeomOrientation();

	POut = P2UVector(GeomReverseSweep_PhysX(World, PGeom, PGeomRot, Start, End, TraceChannel, Params, ResponseParams, ObjectParams));

	return POut;
}

PxVec3 GeomReverseSweep_PhysX(const UWorld* World, const PxGeometry& PGeom, const PxQuat& PGeomRot, FVector Start, FVector End, ECollisionChannel TraceChannel, const struct FCollisionQueryParams& Params, const struct FCollisionResponseParams& ResponseParams, const struct FCollisionObjectQueryParams& ObjectParams)
{
	// Create filter data used to filter collisions
	PxFilterData PFilter = CreateQueryFilterData(TraceChannel, Params.bTraceComplex, ResponseParams.CollisionResponse, Params, ObjectParams, true);
	PxQueryFilterData PQueryFilterData(PFilter, pk_StaticDynamicQueryFlags(Params) | PxQueryFlag::ePREFILTER | PxQueryFlag::ePOSTFILTER | PxQueryFlag::eNO_BLOCK);
	PxHitFlags POutputFlags = PxHitFlag::ePOSITION | PxHitFlag::eNORMAL | PxHitFlag::eDISTANCE | PxHitFlag::eFACE_INDEX | PxHitFlag::eMESH_MULTIPLE | PxHitFlag::eMESH_BOTH_SIDES;
	FPxQueryFilterCallbackSweep PQueryCallbackSweep(Params);

	const FVector Delta = End - Start;
	const float DeltaMagSize = Delta.Size();
	const float DeltaMag = FMath::IsNearlyZero(DeltaMagSize) ? 0.f : DeltaMagSize;

	FPhysScene* PhysScene = World->GetPhysicsScene();
	PxScene* Scene = PhysScene->GetPhysXScene(PST_Sync);

	// Lock scene
	FScopedMultiSceneReadLock SceneLocks;
	SceneLocks.LockRead(World, Scene, PST_Sync);

	const PxTransform PStartTM(U2PVector(Start), PGeomRot);
	PxVec3 PDir = DeltaMag == 0.f ? PxVec3(1.f, 0.f, 0.f) : U2PVector(Delta / DeltaMag);	//If DeltaMag is 0 (equality of float is fine because we sanitized to 0) then just use any normalized direction

	FDynamicHitBuffer<PxRaycastHit> PRayBuffer;

	Scene->raycast(U2PVector(Start), PDir, DeltaMag, PRayBuffer, POutputFlags, PQueryFilterData);
	PxI32 NumHits = PRayBuffer.GetNumHits();

	PxRigidActor *PActor = NULL;
	float CurDist = DeltaMag;

	for (int32 i = 0; i < NumHits; i++)
	{
		PxRaycastHit& PHit = PRayBuffer.GetHits()[i];
		PxTransform PTest(PHit.position, PGeomRot);
		PxOverlapBuffer overlap;
		Scene->overlap(PGeom, PTest, overlap);
		if (overlap.getNbAnyHits() > 1)
			continue;
		if (PHit.distance <= CurDist && PHit.distance > 0.f) {
			CurDist = PHit.distance;
		}
	}

	return PDir * CurDist;
}

#pragma endregion

#pragma region MORE STD PHYSX CODE
#if WITH_EDITOR

static const PxQuat CapsuleRotator(0.f, 0.707106781f, 0.f, 0.707106781f);

PxQuat ConvertToPhysXCapsuleRot(const FQuat& GeomRot)
{
	// Rotation required because PhysX capsule points down X, we want it down Z
	return U2PQuat(GeomRot) * CapsuleRotator;
}

PxFilterData CreateObjectQueryFilterData(const bool bTraceComplex, const int32 MultiTrace/*=1 if multi. 0 otherwise*/, const struct FCollisionObjectQueryParams & ObjectParam)
{
	/**
	* Format for QueryData :
	*		word0 (meta data - ECollisionQuery. Extendable)
	*
	*		For object queries
	*
	*		word1 (object type queries)
	*		word2 (unused)
	*		word3 (Multi (1) or single (0) (top 8) + Flags (lower 24))
	*/

	PxFilterData PNewData;

	PNewData.word0 = ECollisionQuery::ObjectQuery;

	if (bTraceComplex)
	{
		PNewData.word3 |= EPDF_ComplexCollision;
	}
	else
	{
		PNewData.word3 |= EPDF_SimpleCollision;
	}

	// get object param bits
	PNewData.word1 = ObjectParam.GetQueryBitfield();

	// if 'nothing', then set no bits
	PNewData.word3 |= CreateChannelAndFilter((ECollisionChannel)MultiTrace, ObjectParam.IgnoreMask);

	return PNewData;
}

PxFilterData CreateTraceQueryFilterData(const uint8 MyChannel, const bool bTraceComplex, const FCollisionResponseContainer& InCollisionResponseContainer, const FCollisionQueryParams& Params)
{
	/**
	* Format for QueryData :
	*		word0 (meta data - ECollisionQuery. Extendable)
	*
	*		For trace queries
	*
	*		word1 (blocking channels)
	*		word2 (touching channels)
	*		word3 (MyChannel (top 8) as ECollisionChannel + Flags (lower 24))
	*/

	PxFilterData PNewData;

	PNewData.word0 = ECollisionQuery::TraceQuery;

	if (bTraceComplex)
	{
		PNewData.word3 |= EPDF_ComplexCollision;
	}
	else
	{
		PNewData.word3 |= EPDF_SimpleCollision;
	}

	// word1 encodes 'what i block', word2 encodes 'what i touch'
	for (int32 i = 0; i<ARRAY_COUNT(InCollisionResponseContainer.EnumArray); i++)
	{
		if (InCollisionResponseContainer.EnumArray[i] == ECR_Block)
		{
			// if i block, set that in word1
			PNewData.word1 |= CRC_TO_BITFIELD(i);
		}
		else if (InCollisionResponseContainer.EnumArray[i] == ECR_Overlap)
		{
			// if i touch, set that in word2
			PNewData.word2 |= CRC_TO_BITFIELD(i);
		}
	}

	// if 'nothing', then set no bits
	PNewData.word3 |= CreateChannelAndFilter((ECollisionChannel)MyChannel, Params.IgnoreMask);

	return PNewData;
}

#define TRACE_MULTI		1
#define TRACE_SINGLE	0

PxFilterData CreateQueryFilterData(const uint8 MyChannel, const bool bTraceComplex, const FCollisionResponseContainer& InCollisionResponseContainer, const struct FCollisionQueryParams& QueryParam, const struct FCollisionObjectQueryParams & ObjectParam, const bool bMultitrace)
{
	if (ObjectParam.IsValid())
	{
		return CreateObjectQueryFilterData(bTraceComplex, (bMultitrace ? TRACE_MULTI : TRACE_SINGLE), ObjectParam);
	}
	else
	{
		return CreateTraceQueryFilterData(MyChannel, bTraceComplex, InCollisionResponseContainer, QueryParam);
	}
}

#endif
#pragma endregion