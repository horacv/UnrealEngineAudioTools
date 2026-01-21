#pragma once

#include "CoreMinimal.h"
#include "Data.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "SoundbankManagerFmod.generated.h"

class UFMODBank;

DECLARE_LOG_CATEGORY_EXTERN(LogSoundbankManagerFmod, Log, All);

UCLASS(Blueprintable, Abstract)
class ANS_SOUNDBANKMANAGERFMOD_API USoundbankManagerFmod final : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	
	/**
	 * Gets the SoundbankManagerFmod subsystem instance for the given world context.
	 * @param WorldContextObject Object that provides the world context
	 * @return Pointer to the USoundbankManagerFmod subsystem instance, or nullptr if not found
	 */
	static USoundbankManagerFmod* Get(const UObject* WorldContextObject);

	/**
	 * Registers a sound bank using always-loaded sound bank data and attempts to load it.
	 * @param SoundbankData Struct containing the reference to the sound bank along with its loading parameters
	 * @param OnSoundbankReadyCallback Delegate to call when the sound bank is successfully loaded and ready
	 */
	void RegisterAndLoadAlwaysLoadedSoundbankData(const FFmodAlwaysLoadedSoundbankData& SoundbankData,
		const FOnSoundbankManagerOperationReady& OnSoundbankReadyCallback = FOnSoundbankManagerOperationReady());

	/**
	 * Registers a sound bank using always-loaded sound bank data and attempts to load it.
	 * @param SoundbankData Struct containing the reference to the sound bank along with its loading parameters
	 * @param OnSoundbankReadyCallback Delegate to call when the sound bank is successfully loaded and ready
	 */
	UFUNCTION(BlueprintCallable, DisplayName="RegisterAndLoadAlwaysLoadedSoundbankData", meta = (AutoCreateRefTerm = "OnSoundbankReadyCallback"))
	void K2_RegisterAndLoadAlwaysLoadedSoundbankData(const FFmodAlwaysLoadedSoundbankData& SoundbankData,
		const FOnSoundbankManagerOperationReady& OnSoundbankReadyCallback);

	/**
	 * Registers and loads data for a dynamically loaded soundbank.
	 * @param SoundbankData Struct containing metadata and assets related to the soundbank.
	 * @param OnSoundbankReadyCallback Delegate invoked when the soundbank is registered and loaded successfully.
	 */
	void RegisterAndLoadDynamicallyLoadedSoundbankData(const FFmodDynamicallyLoadedSoundbankData& SoundbankData,
		const FOnSoundbankManagerOperationReady& OnSoundbankReadyCallback = FOnSoundbankManagerOperationReady());

	/**
	 * Registers and loads data for a dynamically loaded soundbank.
	 * @param SoundbankData Struct containing metadata and assets related to the soundbank.
	 * @param OnSoundbankReadyCallback Delegate invoked when the soundbank is registered and loaded successfully.
	 */
	UFUNCTION(BlueprintCallable, DisplayName="RegisterAndLoadDynamicallyLoadedSoundbankData", meta = (AutoCreateRefTerm = "OnSoundbankReadyCallback"))
	void K2_RegisterAndLoadDynamicallyLoadedSoundbankData(const FFmodDynamicallyLoadedSoundbankData& SoundbankData,
		const FOnSoundbankManagerOperationReady& OnSoundbankReadyCallback);

	/**
	 * Registers and loads a sound bank using the provided sound bank data and triggers the provided callback upon completion.
	 * @param SoundbankData Data defining the sound bank to be registered and loaded, including its load type and properties.
	 * @param OnSoundbankReadyCallback Delegate function to be executed when the sound bank operation is completed and ready.
	 */
	void RegisterAndLoadSoundBankData(const FFmodSoundbankData& SoundbankData,
		const FOnSoundbankManagerOperationReady& OnSoundbankReadyCallback = FOnSoundbankManagerOperationReady());

	/**
	 * Registers and loads an FMOD soundbank into memory, optionally configuring its loading behavior and usage tracking.
	 * @param Soundbank The FMOD soundbank to register and load. Must be a valid asset.
	 * @param bAlwaysLoaded If true, the soundbank will remain loaded indefinitely, regardless of use count.
	 * @param bLoadBlocking If true, the soundbank loading process will block execution until it completes.
	 * @param bLoadSampleData If true, relevant sample data for the soundbank will also be loaded into memory.
	 * @param OnSoundbankReadyCallback Delegate that will be called when the soundbank is ready for use, either immediately or after loading completes.
	 */
	void RegisterAndLoadSoundBank(const UFMODBank* Soundbank, bool bAlwaysLoaded = false,
		bool bLoadBlocking = true, bool bLoadSampleData = true,
		const FOnSoundbankManagerOperationReady& OnSoundbankReadyCallback = FOnSoundbankManagerOperationReady());

	/**
	 * Registers and loads an FMOD soundbank into memory, optionally configuring its loading behavior and usage tracking.
	 * @param Soundbank The FMOD soundbank to register and load. Must be a valid asset.
	 * @param bAlwaysLoaded If true, the soundbank will remain loaded indefinitely, regardless of use count.
	 * @param bLoadBlocking If true, the soundbank loading process will block execution until it completes.
	 * @param bLoadSampleData If true, relevant sample data for the soundbank will also be loaded into memory.
	 * @param OnSoundbankReadyCallback Delegate that will be called when the soundbank is ready for use, either immediately or after loading completes.
	 */
	UFUNCTION(BlueprintCallable, DisplayName="RegisterAndLoadSoundBank", meta = (AutoCreateRefTerm = "OnSoundbankReadyCallback"))
	void K2_RegisterAndLoadSoundBank(UFMODBank* Soundbank, bool bAlwaysLoaded,
		bool bLoadBlocking, bool bLoadSampleData, const FOnSoundbankManagerOperationReady& OnSoundbankReadyCallback);

	/**
	 * Unregisters and unloads the given FMOD soundbank after a specified delay.
	 * Decrements the usage counter for the soundbank, and if the counter reaches zero, schedules the soundbank for unloading.
	 * @param Soundbank Pointer to the FMOD soundbank asset to unregister and unload
	 * @param DelayInSeconds Time in seconds to delay the unloading of the soundbank; if less than or equal to 0, the soundbank will be unloaded immediately
	 * @return True if the operation was successfully initiated, false if the input soundbank is invalid or the usage count is already zero
	 */
	UFUNCTION(BlueprintCallable)
	UPARAM(DisplayName = "Success") bool UnregisterAndUnloadSoundbank(const UFMODBank* Soundbank, const float DelayInSeconds = 3.f);

	/**
	 * Checks if a specified FMOD sound bank is in the process of being unloaded.
	 * @param Soundbank Pointer to the FMOD bank to check
	 * @return True if the sound bank is unloading, false otherwise
	 */
	UFUNCTION(BlueprintPure)
	bool IsUnloadingSoundBank(const UFMODBank* Soundbank) const;

	/**
	 * Registers and loads all soundbanks listed in the given DataTable.
	 * @param BankTable The DataTable containing soundbank information. Each row should define the properties of a soundbank to load.
	 * @param OnSoundbanksReadyCallback A delegate that is triggered once all soundbanks specified in the table are successfully loaded or marked as pending.
	 */
	void RegisterAndLoadSoundbankTable(const UDataTable* BankTable,
		const FOnSoundbankManagerOperationReady& OnSoundbanksReadyCallback = FOnSoundbankManagerOperationReady());

	/**
	 * Registers and loads all soundbanks listed in the given DataTable.
	 * @param BankTable The DataTable containing soundbank information. Each row should define the properties of a soundbank to load.
	 * @param OnSoundbanksReadyCallback A delegate that is triggered once all soundbanks specified in the table are successfully loaded or marked as pending.
	 */
	UFUNCTION(BlueprintCallable, DisplayName="RegisterAndLoadSoundbankTable", meta = (AutoCreateRefTerm = "OnSoundbanksReadyCallback"))
	void K2_RegisterAndLoadSoundbankTable(const UDataTable* BankTable, const FOnSoundbankManagerOperationReady& OnSoundbanksReadyCallback);

	/**
	 * Delegate event triggered when the banks in the always-loaded soundbank table finish loading.
	 * This is a multicast delegate compatible with Blueprint scripting.
	 */
	UPROPERTY(BlueprintAssignable)
	FOnAlwaysLoadedSoundbankTableLoadedEvent OnAlwaysLoadedSoundbankTableLoadedMulticast;

	/**
	 * Delegate triggered when a soundbank load action occurs.
	 * Intended for handling actions or responses upon the completion of a soundbank loading process.
	 * This is a multicast delegate compatible with Blueprint scripting.
	 */
	UPROPERTY(BlueprintAssignable)
	FOnSoundbankLoadActionEvent OnSoundbankLoadedMulticast;

	/**
	 * Broadcasts an event when a soundbank has been unloaded.
	 * Intended for handling actions or responses upon the completion of a soundbank unloading process.
	 * This is a multicast delegate compatible with Blueprint scripting.
	 */
	UPROPERTY(BlueprintAssignable)
	FOnSoundbankLoadActionEvent OnSoundbankUnloadedMulticast;
	
protected:
	
	/**
	 * Handles the event when the always-loaded soundbank table has finished loading.
	 * This method is called internally when all soundbanks marked as always-loaded have been successfully loaded.
	 * It broadcasts the OnAlwaysLoadedSoundbankTableLoadedMulticast delegate and executes the OnAlwaysLoadedSoundbankTableLoaded callback.
	 */
	UFUNCTION()
	void HandleOnAlwaysLoadedSoundbankTableLoaded();
	
	// Data table containing all Soundbanks that need to be loaded throughout the game instance lifetime
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SoundBank Manager",
		meta=(RequiredAssetDataTags="RowStructure=/Script/ANS_SoundbankManagerFMOD.FmodAlwaysLoadedSoundbankData"))
	TObjectPtr<UDataTable> AlwaysLoadedSoundbanks;
	
	// Counts the number of entities using a specific Soundbank
	UPROPERTY(Transient)
	TMap<FGuid, FFmodSoundbankNameAndCount> LoadedSoundbanksCounters;

	// Keeps the timer handles of unloading Soundbanks
	UPROPERTY(Transient)
	TMap<FGuid, FTimerHandle> UnloadingSoundbankTimers;

	/**
	 * Holds a mapping between unique identifiers (FGuid) and their corresponding callbacks (FFmodSoundbankReadyCallbacks)
	 * that are pending execution when associated Soundbanks become ready.
	 * This map is used to manage and trigger callbacks once the asynchronous Soundbank loading process is completed.
	 */
	UPROPERTY(Transient)
	TMap<FGuid, FFmodSoundbankReadyCallbacks> PendingOnSoundbankReadyCallbacks;

	/**
	 * This array holds callback information for soundbanks that are pending loading and loaded from a data table.
	 */
	UPROPERTY(Transient)
	TArray<FTablePendingSoundbanks> PendingTableSoundbanks;

	/**
	 * Delegate triggered when the always-loaded soundbank table has been successfully loaded.
	 * This delegate can be used to notify systems dependent on the always-loaded soundbanks
	 * that the operation is complete and resources are ready for use.
	 */
	FOnSoundbankManagerOperationReady OnAlwaysLoadedSoundbankTableLoaded;

	/**
	 * Loads the specified FMOD soundbank and optionally its sample data.
	 * The operation can be performed synchronously or asynchronously based on the provided flags.
	 * @param Soundbank The FMOD soundbank to load
	 * @param OnSoundbankReady Delegate to execute when the soundbank is ready for use
	 * @param bBlocking If true, the loading operation is performed synchronously; otherwise, asynchronously
	 * @param bLoadSampleData If true, sample data for the soundbank will also be loaded
	 * @return True if the soundbank was successfully loaded, false otherwise
	 */
	bool LoadSoundbank(const UFMODBank* Soundbank, const FOnSoundbankManagerOperationReady& OnSoundbankReady, const bool bBlocking = true, const bool bLoadSampleData = true);

	/**
	 * Unloads the specified FMOD soundbank immediately, ensuring its resources are released.
	 * @param Soundbank Pointer to the UFMODBank object representing the soundbank to be unloaded
	 * @return True if the soundbank was successfully unloaded, false otherwise
	 */
	bool UnloadSoundbankImmediate(const UFMODBank* Soundbank);

	/**
	 * Unloads the specified FMOD soundbank after a specified delay, setting a timer to handle the operation asynchronously.
	 * @param Soundbank The FMOD bank to unload. Must be valid during the function call.
	 * @param DelayInSeconds The delay (in seconds) after which the soundbank will be unloaded.
	 */
	void UnloadSoundbankDelayed(const UFMODBank* Soundbank, const float DelayInSeconds = 3.f);

	/**
	 * Stops the unloading timer associated with the given soundbank and invalidates its entry.
	 * @param BankGUID The GUID of the soundbank whose unloading timer should be stopped.
	 * @param DebugName A string used for debugging purposes, typically the name of the soundbank.
	 */
	void StopAndInvalidateUnloadingTimer(const FGuid& BankGUID, const FString& DebugName = TEXT(""));

	/**
	 * Handles expiration of the timer for unloading a soundbank.
	 * @param SoundbankSoftPtr A soft pointer to the FMOD bank to be unloaded
	 * @param BankGUID The unique identifier of the soundbank associated with the timer
	 */
	UFUNCTION()
    void OnUnloadingSoundbankTimerExpired(const TSoftObjectPtr<UFMODBank>& SoundbankSoftPtr, const FGuid& BankGUID);

	/**
	 * Processes and handles pending callbacks for Soundbank readiness.
	 * This method iterates through the list of pending callbacks associated with Soundbanks
	 * and checks their loading state. For each Soundbank that is fully loaded, the associated
	 * callbacks are invoked. The entry is then removed from the pending callbacks list.
	 * If a Soundbank cannot be found or is in an error state, the entry is also removed.
	 */
	void HandlePendingSoundbankReadyCallbacks();

	/**
	 * Handles pending table soundbank ready callbacks by iterating through the list of pending soundbanks.
	 * Checks the loading state of each soundbank and executes the associated callback once all the soundbanks
	 * for a table are fully loaded.
	 *
	 * This function interacts with the FMOD Studio System to query the loading states of soundbanks.
	 * If a table's soundbanks are all loaded, the callback associated with the table is executed,
	 * and the entry is removed from the pending list.
	 */
	void HandlePendingTableSoundbankReadyCallbacks();
	
	/**
	 * Tick function called each frame to handle pending sound bank ready callbacks.
	 * @param DeltaTime Time elapsed since the last tick
	 * @return True to continue ticking, false to stop
	 */
	UFUNCTION()
	bool TickGameThread(float DeltaTime);
	
	/**
	 * Handle for the ticker delegate used to process sound bank callbacks on the game thread.
	 */
	FTSTicker::FDelegateHandle TickDelegateHandle;

	/**
	 * Initializes the subsystem. Sets up a ticker for callback processing and loads always-loaded sound banks if configured.
	 * @param Collection Collection of subsystems being initialized
	 */
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/**
	 * Cleans up the subsystem. Unregisters ticker delegate and performs cleanup of loaded sound banks.
	 */
	virtual void Deinitialize() override;
};