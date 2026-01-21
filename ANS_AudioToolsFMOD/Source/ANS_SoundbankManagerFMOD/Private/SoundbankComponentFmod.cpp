#include "SoundbankComponentFmod.h"

#include "FMODBank.h"
#include "SoundbankManagerFmod.h"

USoundbankComponentFmod::USoundbankComponentFmod()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USoundbankComponentFmod::RegisterSoundbankUsage()
{
	if (bIsRegistered || BankData.Bank.IsNull()) { return; }

	if (auto* SoundBankManager = USoundbankManagerFmod::Get(this))
	{
		OnSoundbankReady.BindDynamic(this, &USoundbankComponentFmod::HandleOnSoundbankReady);
		SoundBankManager->RegisterAndLoadSoundBankData(BankData, OnSoundbankReady);
	}
}

void USoundbankComponentFmod::UnregisterSoundbankUsage()
{
	if (!bIsRegistered || BankData.Bank.IsNull()) { return; }

	if (auto* SoundBankManager = USoundbankManagerFmod::Get(this))
	{
		if (SoundBankManager->UnregisterAndUnloadSoundbank(BankData.Bank.Get(), BankData.UnloadDelayInSeconds))
		{
			OnSoundbankReady.Unbind();
			bIsRegistered = false;
		}
	}
}

void USoundbankComponentFmod::HandleOnSoundbankReady()
{
	bIsRegistered = true;
	OnSoundbankRegisteredMulticast.Broadcast();
}

void USoundbankComponentFmod::BeginPlay()
{
	Super::BeginPlay();
	if (bRegisterBankOnBeginPlay) { RegisterSoundbankUsage(); }
}

void USoundbankComponentFmod::BeginDestroy()
{
	UnregisterSoundbankUsage();
	Super::BeginDestroy();
}

void USoundbankComponentFmod::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnregisterSoundbankUsage();
	Super::EndPlay(EndPlayReason);
}