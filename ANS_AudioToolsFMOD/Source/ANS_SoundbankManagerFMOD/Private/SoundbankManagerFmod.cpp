#include "SoundbankManagerFmod.h"

#include "Data.h"
#include "FMODBank.h"
#include "FMODStudioModule.h"
#include "FMODUtils.h"
#include "fmod_studio.hpp"

DEFINE_LOG_CATEGORY(LogSoundbankManagerFmod);

USoundbankManagerFmod* USoundbankManagerFmod::Get(const UObject* WorldContextObject)
{
	const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull);

	if (!IsValid(World))
	{
		return nullptr;
	}

	return World->GetGameInstance()->GetSubsystem<USoundbankManagerFmod>();
}

void USoundbankManagerFmod::RegisterAndLoadAlwaysLoadedSoundbankData(const FFmodAlwaysLoadedSoundbankData& SoundbankData,
	const FOnSoundbankManagerOperationReady& OnSoundbankReadyCallback)
{
	RegisterAndLoadSoundBankData(SoundbankData, OnSoundbankReadyCallback);
}

void USoundbankManagerFmod::K2_RegisterAndLoadAlwaysLoadedSoundbankData(
	const FFmodAlwaysLoadedSoundbankData& SoundbankData, const FOnSoundbankManagerOperationReady& OnSoundbankReadyCallback)
{
	RegisterAndLoadSoundBankData(SoundbankData, OnSoundbankReadyCallback);
}

void USoundbankManagerFmod::RegisterAndLoadDynamicallyLoadedSoundbankData(const FFmodDynamicallyLoadedSoundbankData& SoundbankData,
                                                                          const FOnSoundbankManagerOperationReady& OnSoundbankReadyCallback)
{
	RegisterAndLoadSoundBankData(SoundbankData, OnSoundbankReadyCallback);
}

void USoundbankManagerFmod::K2_RegisterAndLoadDynamicallyLoadedSoundbankData(
	const FFmodDynamicallyLoadedSoundbankData& SoundbankData, const FOnSoundbankManagerOperationReady& OnSoundbankReadyCallback)
{
	RegisterAndLoadSoundBankData(SoundbankData, OnSoundbankReadyCallback);
}

void USoundbankManagerFmod::RegisterAndLoadSoundBankData(const FFmodSoundbankData& SoundbankData, const FOnSoundbankManagerOperationReady& OnSoundbankReadyCallback)
{
	if (SoundbankData.Bank.IsNull()) { return; }
	
	bool bAlwaysLoaded = false;
	bool bLoadBlocking = true;
	bool bLoadSampleData = true;
	
	if (SoundbankData.GetLoadType() == DynamicallyLoaded)
	{
		const auto Data = static_cast<const FFmodDynamicallyLoadedSoundbankData*>(&SoundbankData);
		bLoadBlocking = Data->bLoadBlocking;
		bLoadSampleData = Data->bLoadSampleData;
	}
	else if (SoundbankData.GetLoadType() == AlwaysLoaded)
	{
		const auto Data = static_cast<const FFmodAlwaysLoadedSoundbankData*>(&SoundbankData);
		bLoadBlocking = Data->bLoadBlocking;
		bLoadSampleData = Data->bLoadSampleData;
		bAlwaysLoaded = true;
	}
	
	RegisterAndLoadSoundBank(SoundbankData.Bank.LoadSynchronous(), bAlwaysLoaded,
		bLoadBlocking, bLoadSampleData, OnSoundbankReadyCallback);
}

void USoundbankManagerFmod::RegisterAndLoadSoundBank(const UFMODBank* Soundbank, const bool bAlwaysLoaded, const bool bLoadBlocking,
	const bool bLoadSampleData, const FOnSoundbankManagerOperationReady& OnSoundbankReadyCallback)
{
	if (!IsValid(Soundbank)) { return; }

	const FGuid BankGUID = Soundbank->AssetGuid;
	const FString BankDisplayName = Soundbank->GetName();
	
	StopAndInvalidateUnloadingTimer(BankGUID, BankDisplayName);
	
	if (auto* Counter = LoadedSoundbanksCounters.Find(BankGUID);
		Counter != nullptr && Counter->Count > 0)
	{
		if (bAlwaysLoaded && Counter->Count > 1)
		{
			Counter->Count = 1;
			UE_LOG(LogSoundbankManagerFmod, Log, TEXT("Soundbank \"%s\" set as Always Loaded, Counter: %d"), *BankDisplayName, Counter->Count);
		}
		else if (!Counter->bAlwaysLoaded)
		{
			Counter->Count++;
			UE_LOG(LogSoundbankManagerFmod, Log, TEXT("Soundbank \"%s\" usage registered, Counter: %d"), *BankDisplayName, Counter->Count);
		}
		OnSoundbankReadyCallback.ExecuteIfBound();
		return;
	}
	
	if (LoadSoundbank(Soundbank, OnSoundbankReadyCallback, bLoadBlocking, bLoadSampleData))
	{
		LoadedSoundbanksCounters.Emplace(BankGUID, FFmodSoundbankNameAndCount(BankDisplayName, 1, bAlwaysLoaded));
		UE_LOG(LogSoundbankManagerFmod, Log, TEXT("Soundbank \"%s\" usage registered, Always Loaded: %d, Counter: %d"), *BankDisplayName, bAlwaysLoaded, 1);

	}
}

void USoundbankManagerFmod::K2_RegisterAndLoadSoundBank(UFMODBank* Soundbank, const bool bAlwaysLoaded, const bool bLoadBlocking,
	const bool bLoadSampleData, const FOnSoundbankManagerOperationReady& OnSoundbankReadyCallback)
{
	RegisterAndLoadSoundBank(Soundbank, bAlwaysLoaded, bLoadBlocking, bLoadSampleData, OnSoundbankReadyCallback);
}

bool USoundbankManagerFmod::UnregisterAndUnloadSoundbank(const UFMODBank* Soundbank, const float DelayInSeconds)
{
	if (!IsValid(Soundbank)) { return false; }

	const FString BankDisplayName = Soundbank->GetName();
	const FGuid BankGUID = Soundbank->AssetGuid;
	const auto Counter = LoadedSoundbanksCounters.FindRef(BankGUID);
	unsigned int Count = Counter.Count;
	if (Count == 0)
	{
		UE_LOG(LogSoundbankManagerFmod, Log, TEXT("Soundbank \"%s\" not unregistered or unloaded. Reason: Counter is 0, Counter: %d"), *BankDisplayName, Count);
		return false;
	}
	
	if (Counter.bAlwaysLoaded) // Always loaded Soundbanks should not be unloaded
	{
		UE_LOG(LogSoundbankManagerFmod, Log, TEXT("Soundbank \"%s\" not unregistered or unloaded. Reason: Always loaded, Counter: %d"), *BankDisplayName, Count);
		return false;
	}
	
	LoadedSoundbanksCounters.FindOrAdd(BankGUID).Count = --Count;
	
	UE_LOG(LogSoundbankManagerFmod, Log, TEXT("Soundbank \"%s\" usage unregistered, Counter: %d"), *BankDisplayName, Count);
	
	if (Count == 0)
	{
		StopAndInvalidateUnloadingTimer(BankGUID, BankDisplayName);

		if (DelayInSeconds <= 0)
		{
			UnloadSoundbankImmediate(Soundbank);
		}
		else
		{
			UnloadSoundbankDelayed(Soundbank, DelayInSeconds);
		}
	}
	
	return true;
}

bool USoundbankManagerFmod::IsUnloadingSoundBank(const UFMODBank* Soundbank) const
{
	if (IsValid(Soundbank)) { return false; }
	
	const FGuid BankGUID = Soundbank->AssetGuid;
	return UnloadingSoundbankTimers.Contains(BankGUID);
}

void USoundbankManagerFmod::RegisterAndLoadSoundbankTable(const UDataTable* BankTable, const FOnSoundbankManagerOperationReady& OnSoundbanksReadyCallback)
{
	if (!IsValid(BankTable)) { return; }
	
	FTablePendingSoundbanks PendingSoundbanks;
	PendingSoundbanks.TableDisplayName = BankTable->GetName();
	PendingSoundbanks.Callback = OnSoundbanksReadyCallback;

	TArray<FFmodSoundbankData*> SoundBanksData;
	BankTable->GetAllRows<FFmodSoundbankData>(FString(), SoundBanksData);

	for (auto*& SoundBankData : SoundBanksData)
	{
		if (SoundBankData->Bank.IsNull()) { continue; }
		
		if (!SoundBankData->bLoadBlocking)
		{
			PendingSoundbanks.PendingBanks.Add(SoundBankData->Bank.LoadSynchronous()->AssetGuid);
		}

		RegisterAndLoadSoundBankData(*SoundBankData, {});
	}
	
	if (PendingSoundbanks.PendingBanks.IsEmpty())
	{
		OnSoundbanksReadyCallback.ExecuteIfBound();
		UE_LOG(LogSoundbankManagerFmod, Log, TEXT("Soundbank Table \"%s\" Loading Complete"), *PendingSoundbanks.TableDisplayName);
	}
	else
	{
		PendingTableSoundbanks.Add(PendingSoundbanks);
	}
}

void USoundbankManagerFmod::K2_RegisterAndLoadSoundbankTable(const UDataTable* BankTable,
	const FOnSoundbankManagerOperationReady& OnSoundbanksReadyCallback)
{
	RegisterAndLoadSoundbankTable(BankTable, OnSoundbanksReadyCallback);
}

void USoundbankManagerFmod::HandleOnAlwaysLoadedSoundbankTableLoaded()
{
	OnAlwaysLoadedSoundbankTableLoadedMulticast.Broadcast();
	OnAlwaysLoadedSoundbankTableLoaded.Unbind();
}

bool USoundbankManagerFmod::LoadSoundbank(const UFMODBank* Soundbank, const FOnSoundbankManagerOperationReady& OnSoundbankReady, const bool bBlocking, const bool bLoadSampleData)
{
	FMOD::Studio::System* AudioEngine = IFMODStudioModule::Get().GetStudioSystem(EFMODSystemContext::Runtime);
	if (!(AudioEngine && IsValid(Soundbank))) { return false; }
	
	FMOD::Studio::Bank* StudioBank;
	const FString BankPath = IFMODStudioModule::Get().GetBankPath(*Soundbank);
	const FMOD_STUDIO_LOAD_BANK_FLAGS Load_Bank_Flags = bBlocking || bLoadSampleData ? FMOD_STUDIO_LOAD_BANK_NORMAL : FMOD_STUDIO_LOAD_BANK_NONBLOCKING;
	const FMOD_RESULT Result = AudioEngine->loadBankFile(TCHAR_TO_UTF8(*BankPath), Load_Bank_Flags, &StudioBank);

	if (Result == FMOD_OK && bLoadSampleData)
	{
		StudioBank->loadSampleData();
	}
	
	if (bBlocking) // If blocking, the requested Soundbank must be ready at this point
	{
		OnSoundbankLoadedMulticast.Broadcast(Soundbank);
		OnSoundbankReady.ExecuteIfBound();
		UE_LOG(LogSoundbankManagerFmod, Log, TEXT("Soundbank \"%s\" Loaded Synchronously"), *Soundbank->GetName());	
	}
	else // If loading Async, add it to the PendingOnSoundbankReadyCallbacks to be called when ready
	{
		PendingOnSoundbankReadyCallbacks.FindOrAdd(Soundbank->AssetGuid).Callbacks.Add(OnSoundbankReady);
	}

	return Result == FMOD_OK;
}

bool USoundbankManagerFmod::UnloadSoundbankImmediate(const UFMODBank* Soundbank)
{
	const FMOD::Studio::System* StudioSystem = IFMODStudioModule::Get().GetStudioSystem(EFMODSystemContext::Runtime);
	if (!(StudioSystem && IsValid(Soundbank)))
	{
		return false;
	}

	const FMOD_GUID BankGUID = FMODUtils::ConvertGuid(Soundbank->AssetGuid);

	FMOD::Studio::Bank* Bank;

	if (const FMOD_RESULT Result = StudioSystem->getBankByID(&BankGUID, &Bank);
		Result == FMOD_OK && Bank)
	{
		const bool bUnloaded = Bank->unload() == FMOD_OK;
		if (bUnloaded)
		{
			OnSoundbankUnloadedMulticast.Broadcast(Soundbank);
			UE_LOG(LogSoundbankManagerFmod, Log, TEXT("Soundbank \"%s\" Unloaded"), *Soundbank->GetName());
		}
		return bUnloaded;
	}
	return false;
}

void USoundbankManagerFmod::UnloadSoundbankDelayed(const UFMODBank* Soundbank, const float DelayInSeconds)
{
	if (const UGameInstance* GameInstance = GetGameInstance();
		IsValid(GameInstance) && IsValid(Soundbank))
	{
		FTimerHandle TimerHandle;
		FTimerDelegate Delegate;
		const FGuid BankGUID = Soundbank->AssetGuid;
		Delegate.BindUFunction(this, "OnUnloadingSoundBankTimerExpired", MakeSoftObjectPtr(Soundbank), BankGUID);
		GameInstance->GetTimerManager().SetTimer(TimerHandle, Delegate, DelayInSeconds, false);
		UnloadingSoundbankTimers.Emplace(BankGUID, TimerHandle);
		UE_LOG(LogSoundbankManagerFmod, Log, TEXT("Soundbank \"%s\" Unloading in %f seconds"), *Soundbank->GetName(), DelayInSeconds);
	}
}

void USoundbankManagerFmod::StopAndInvalidateUnloadingTimer(const FGuid& BankGUID, const FString& DebugName)
{
	auto TimerHandle = UnloadingSoundbankTimers.FindRef(BankGUID);
	const UGameInstance* GameInstance = GetGameInstance();
	
	if (TimerHandle.IsValid() && IsValid(GameInstance))
	{
		GameInstance->GetTimerManager().ClearTimer(TimerHandle);
		UnloadingSoundbankTimers.Remove(BankGUID);
		UE_LOG(LogSoundbankManagerFmod, Log, TEXT("Soundbank \"%s\" Unloading Timer Stopped"), *DebugName);
	}
}

void USoundbankManagerFmod::OnUnloadingSoundbankTimerExpired(const TSoftObjectPtr<UFMODBank>& SoundbankSoftPtr, const FGuid& BankGUID)
{
	if (const UFMODBank* SoundBankPtr = SoundbankSoftPtr.LoadSynchronous())
	{
		UnloadSoundbankImmediate(SoundBankPtr);
	}
	UnloadingSoundbankTimers.Remove(BankGUID);
}

void USoundbankManagerFmod::HandlePendingSoundbankReadyCallbacks()
{
	if (PendingOnSoundbankReadyCallbacks.IsEmpty()) { return; }

	const FMOD::Studio::System* StudioSystem = IFMODStudioModule::Get().GetStudioSystem(EFMODSystemContext::Runtime);
	if (!StudioSystem)
	{
		return;
	}
	
	for (auto It = PendingOnSoundbankReadyCallbacks.CreateIterator(); It; ++It)
	{
		const FMOD_GUID BankGUID = FMODUtils::ConvertGuid(It.Key());
		FMOD::Studio::Bank* Bank;

		if (const FMOD_RESULT Result = StudioSystem->getBankByID(&BankGUID, &Bank); Result == FMOD_OK && Bank)
		{
			FMOD_STUDIO_LOADING_STATE LoadingState;
			Bank->getLoadingState(&LoadingState);
			
			if (LoadingState == FMOD_STUDIO_LOADING_STATE_LOADED)
			{
				for (auto& Callback : It.Value().Callbacks)
				{
					char Path[256] = {};
					int Retrieved;
					Bank->getPath(Path, sizeof(Path), &Retrieved);
					if (const auto Soundbank = Cast<UFMODBank>(IFMODStudioModule::Get().FindAssetByName(Path));
						IsValid(Soundbank))
					{
						OnSoundbankLoadedMulticast.Broadcast(Soundbank);
						Callback.ExecuteIfBound();
						UE_LOG(LogSoundbankManagerFmod, Log, TEXT("Soundbank \"%s\" Loaded Asynchronously"), *IFMODStudioModule::Get().FindAssetByName(Path)->GetName());	
					}
				}
				It.RemoveCurrent();
			}
		}
		else
		{
			It.RemoveCurrent();
		}
	}
}

void USoundbankManagerFmod::HandlePendingTableSoundbankReadyCallbacks()
{
	if (PendingTableSoundbanks.IsEmpty()) { return; }
	
	const FMOD::Studio::System* StudioSystem = IFMODStudioModule::Get().GetStudioSystem(EFMODSystemContext::Runtime);
	if (!StudioSystem)
	{
		return;
	}
	
	for (auto ItEntry = PendingTableSoundbanks.CreateIterator(); ItEntry; ++ItEntry)
	{
		for (auto ItBank = ItEntry->PendingBanks.CreateIterator(); ItBank; ++ItBank)
		{
			const FMOD_GUID BankGUID = FMODUtils::ConvertGuid(*ItBank);
			FMOD::Studio::Bank* Bank;

			if (const FMOD_RESULT Result = StudioSystem->getBankByID(&BankGUID, &Bank); Result == FMOD_OK && Bank)
			{
				FMOD_STUDIO_LOADING_STATE LoadingState;
				Bank->getLoadingState(&LoadingState);
			
				if (LoadingState == FMOD_STUDIO_LOADING_STATE_LOADED)
				{
					ItBank.RemoveCurrent();
				}
			}
		}
		
		if (ItEntry->PendingBanks.IsEmpty())
		{
			ItEntry->Callback.ExecuteIfBound();
			ItEntry.RemoveCurrent();
			UE_LOG(LogSoundbankManagerFmod, Log, TEXT("Soundbank Table \"%s\" Loading Complete"), *ItEntry->TableDisplayName);	
		}
	}
}

bool USoundbankManagerFmod::TickGameThread([[maybe_unused]]float DeltaTime)
{
	HandlePendingSoundbankReadyCallbacks();
	HandlePendingTableSoundbankReadyCallbacks();
	return true;
}

void USoundbankManagerFmod::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	constexpr float FirstTickDelay = 0.1f;
	TickDelegateHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &USoundbankManagerFmod::TickGameThread), FirstTickDelay);

	if (const UDataTable* AlwaysLoadedSoundBanksPtr = AlwaysLoadedSoundbanks.Get(); 
		IsValid(AlwaysLoadedSoundBanksPtr))
	{
		OnAlwaysLoadedSoundbankTableLoaded.BindDynamic(this, &USoundbankManagerFmod::HandleOnAlwaysLoadedSoundbankTableLoaded);
		RegisterAndLoadSoundbankTable(AlwaysLoadedSoundBanksPtr, OnAlwaysLoadedSoundbankTableLoaded);
	}
}

void USoundbankManagerFmod::Deinitialize()
{
	FTSTicker::GetCoreTicker().RemoveTicker(TickDelegateHandle);
	Super::Deinitialize();
}