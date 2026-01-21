#pragma once

#include "CoreMinimal.h"
#include "FMODBank.h"
#include "FMODStudioModule.h"
#include "FMODUtils.h"

#include "Data.generated.h"

DECLARE_DYNAMIC_DELEGATE(FOnSoundbankManagerOperationReady);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAlwaysLoadedSoundbankTableLoadedEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSoundbankRegisteredEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSoundbankLoadActionEvent, const UFMODBank*, Bank);

UENUM(BlueprintType)
enum EFmodSoundbankLoadType
{
	Default,
	AlwaysLoaded,
	DynamicallyLoaded
};

USTRUCT(BlueprintType)
struct FFmodSoundbankData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UFMODBank> Bank = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bLoadBlocking = true;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bLoadSampleData = true;
	
	EFmodSoundbankLoadType GetLoadType() const { return LoadType; }
	
protected:
	EFmodSoundbankLoadType LoadType = Default;
};

USTRUCT(BlueprintType)
struct FFmodAlwaysLoadedSoundbankData : public FFmodSoundbankData
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString GamePath;
	
	FFmodAlwaysLoadedSoundbankData() { LoadType = AlwaysLoaded; }
	
private:
	virtual void OnDataTableChanged(const UDataTable* InDataTable, const FName InRowName) override
	{
		FTableRowBase::OnDataTableChanged(InDataTable, InRowName);
#if WITH_EDITOR
		if (!Bank.IsNull() && IFMODStudioModule::Get().IsAvailable())
		{
			if (const auto* BankPtr = Bank.LoadSynchronous())
			{
				GamePath = BankPtr->GetPathName();
			}
		}
#endif
	}
};

USTRUCT(BlueprintType)
struct FFmodDynamicallyLoadedSoundbankData : public FFmodSoundbankData
{
	GENERATED_USTRUCT_BODY()
	
	FFmodDynamicallyLoadedSoundbankData() { LoadType = DynamicallyLoaded; }
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float UnloadDelayInSeconds = 3.0f;
};

USTRUCT()
struct FFmodSoundbankNameAndCount
{
	GENERATED_USTRUCT_BODY()

	FString Name = FString("");
	unsigned int Count = 0;
	bool bAlwaysLoaded = false;
};

USTRUCT()
struct FFmodSoundbankReadyCallbacks
{
	GENERATED_USTRUCT_BODY()
	
	TArray<FOnSoundbankManagerOperationReady> Callbacks;
};

USTRUCT()
struct FTablePendingSoundbanks
{
	GENERATED_USTRUCT_BODY()
	
	FString TableDisplayName;
	FOnSoundbankManagerOperationReady Callback;
	TArray<FGuid> PendingBanks;
};