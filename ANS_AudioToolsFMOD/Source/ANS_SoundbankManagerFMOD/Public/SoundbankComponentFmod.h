#pragma once

#include "CoreMinimal.h"
#include "Data.h"
#include "Components/ActorComponent.h"

#include "SoundbankComponentFmod.generated.h"

class UFMODBank;

/**
 * Abstract component class that manages FMOD soundbank loading and unloading.
 * Handles automatic registration/unregistration of soundbanks during the actor's lifecycle.
 * Provides callbacks for when soundbanks are ready to use.
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class ANS_SOUNDBANKMANAGERFMOD_API USoundbankComponentFmod : public UActorComponent
{
	GENERATED_BODY()

public:

	USoundbankComponentFmod();
	
	/**
	 * Multicast delegate that broadcasts when the soundbank has been successfully registered and loaded.
	 * Use this to trigger logic that depends on the soundbank being available.
	 */
	UPROPERTY(BlueprintAssignable)
	FOnSoundbankRegisteredEvent OnSoundbankRegisteredMulticast;
				
	/**
	 * Checks whether the soundbank is currently registered and loaded.
	 * @return True if the soundbank is registered, false otherwise.
	 */
	UFUNCTION(BlueprintPure)
	bool IsSoundbankRegistered() const { return bIsRegistered; }

	/**
	 * Manually registers and loads the soundbank specified in BankData.
	 * Does nothing if already registered or if BankData.Bank is null.
	 * When loading completes, OnSoundbankRegisteredMulticast will be broadcast.
	 */
	UFUNCTION(BlueprintCallable)
	void RegisterSoundbankUsage();

	/**
	 * Manually unregisters and unloads the soundbank.
	 * Does nothing if not currently registered or if BankData.Bank is null.
	 * Respects the UnloadDelayInSeconds setting from BankData.
	 */
	UFUNCTION(BlueprintCallable)
	void UnregisterSoundbankUsage();

protected:
	
	/** Delegate bound to the soundbank manager that gets called when the soundbank loading operation completes. */
	FOnSoundbankManagerOperationReady OnSoundbankReady;

	/** Tracks whether the soundbank usage is currently registered with the soundbank manager. */
	bool bIsRegistered = false;

	/**
	 * If true, automatically registers the soundbank when BeginPlay is called.
	 * Set to false and use RegisterSoundbankUsage if you want to manually control when the soundbank is loaded.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bRegisterBankOnBeginPlay = true;

	/**
	 * Configuration data for the soundbank, including the bank reference and unload delay settings.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FFmodDynamicallyLoadedSoundbankData BankData;

	/**
	 * Internal callback handler that is invoked when the soundbank is ready.
	 * Sets bIsRegistered to true and broadcasts OnSoundbankRegisteredMulticast.
	 */
	UFUNCTION()
	void HandleOnSoundbankReady();
	
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};
