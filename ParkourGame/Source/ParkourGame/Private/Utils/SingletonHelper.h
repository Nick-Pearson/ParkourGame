#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "SingletonHelper.generated.h"

class AActor;

class FSingletonHelper
{
public:
	FSingletonHelper();
	~FSingletonHelper();

public:
	template<typename T>
	FORCEINLINE T* GetSingletonObject(UWorld* WorldPtr) { return Cast<T>(GetSingletonObjectByClass(WorldPtr, T::StaticClass())); }

	AActor* GetSingletonObjectByClass(UWorld* WorldPtr, UClass* Class);

private:
	TMap<UClass*, TWeakObjectPtr<AActor>> ObjectPtrs;
};


UCLASS()
class USingletonHelperLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintPure, Category = "SingeltonHelperLibrary", meta = (WorldContext = "WorldContextObject"))
	static class AMiniGameManager* GetMiniGameManager(UObject* WorldContextObject);

};
