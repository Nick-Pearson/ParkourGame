#pragma once

#include "Runtime/Engine/Private/PhysicsEngine/PhysXSupport.h"

FVector GeomReverseSweep(const UWorld* World, const struct FCollisionShape& CollisionShape, const FQuat& Rot, FVector Start, FVector End, ECollisionChannel TraceChannel, const struct FCollisionQueryParams& Params, const struct FCollisionResponseParams& ResponseParams, const struct FCollisionObjectQueryParams& ObjectParams = FCollisionObjectQueryParams::DefaultObjectQueryParam);

PxVec3 GeomReverseSweep_PhysX(const UWorld* World, const PxGeometry& PGeom, const PxQuat& PGeomRot, FVector Start, FVector End, ECollisionChannel TraceChannel, const struct FCollisionQueryParams& Params, const struct FCollisionResponseParams& ResponseParams, const struct FCollisionObjectQueryParams& ObjectParams);