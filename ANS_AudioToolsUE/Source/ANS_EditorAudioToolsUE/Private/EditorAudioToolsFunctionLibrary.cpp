#include "EditorAudioToolsFunctionLibrary.h"

#include "AssetManagerEditorModule.h"
#include "AssetRegistry/AssetRegistryHelpers.h"
#include "EditorAssetLibrary.h"
#include "EditorAudioToolsData.h"
#include "Engine/AssetManager.h"


void UEditorAudioToolsFunctionLibrary::AuditAssets(const TArray<FAssetData> Assets)
{
	IAssetManagerEditorModule::Get().OpenAssetAuditUI(Assets);
}

bool UEditorAudioToolsFunctionLibrary::GetClassesInAssetCollection(const TArray<UObject*> Assets, TArray<UClass*>& OutClasses)
{
	if (Assets.IsEmpty())
		return false;
	
	TSet<UClass*> FoundClasses;
	for (const UObject* Asset : Assets)
	{
		FoundClasses.Add(Asset->GetClass());
	}
	
	OutClasses = FoundClasses.Array();
	return !OutClasses.IsEmpty();
}

bool UEditorAudioToolsFunctionLibrary::GetAllAssetsOfClasses(const TArray<UClass*> Classes, const bool bSearchSubClasses, TArray<FAssetData>& OutAssets)
{
	if (Classes.IsEmpty())
		return false;
	
	for (const UClass* Class : Classes)
	{
		TArray<FAssetData> AssetsOfClass;
		UAssetManager::Get().GetAssetRegistry().GetAssetsByClass(FTopLevelAssetPath(Class), AssetsOfClass, bSearchSubClasses);
		OutAssets.Append(AssetsOfClass);
	}
	
	return !OutAssets.IsEmpty();
}

TArray<FAssetData> UEditorAudioToolsFunctionLibrary::GetAllUnreferencedAssetsInCollection(TArray<FAssetData> AssetCollection, const FAssetRegistryDependencyOptions& ReferenceOptions)
{
	TArray<FAssetData> UnreferencedAssets;
	
	for (const FAssetData& Asset : AssetCollection)
	{
		TArray<FName> Referencers;
		UAssetManager::Get().GetAssetRegistry().K2_GetReferencers(Asset.PackageName, ReferenceOptions, Referencers);
		
		if (Referencers.IsEmpty())
			UnreferencedAssets.Add(Asset);
	}
	
	return UnreferencedAssets;
}

TArray<FAssetData> UEditorAudioToolsFunctionLibrary::FilterAssetsByMetadataTag(TArray<FAssetData> AssetsToFilter, const FName Tag, const EEditorAudioToolsMetadataQuery QueryType)
{
	TArray<FAssetData> FilteredAssets;
	
	for (const FAssetData& AssetToFilter : AssetsToFilter)
	{
		TMap<FName, FString> CurrentMetadata = UEditorAssetLibrary::GetMetadataTagValues(AssetToFilter.GetAsset());
		
		const bool bTagFound = CurrentMetadata.Contains(Tag);
		bool bValueIsEmpty = true;
		
		if (bTagFound)
			bValueIsEmpty = CurrentMetadata.FindRef(Tag).IsEmpty();
		
		if (bTagFound && (!bValueIsEmpty && QueryType == WithTagExcludingEmptyValues || QueryType == WithTagIncludingEmptyValues))
		{
			FilteredAssets.Add(AssetToFilter);
			continue;
		}
		if ((!bTagFound && QueryType == WithoutTag) || ((!bTagFound || bValueIsEmpty) && QueryType == WithoutTagOrWithEmptyValues))
		{
			FilteredAssets.Add(AssetToFilter);
		}
	}
	
	return FilteredAssets;
}

TArray<FAssetData> UEditorAudioToolsFunctionLibrary::FilterAssetsByMetadataTagAndValue(TArray<FAssetData> AssetsToFilter, const FEditorAudioToolsAssetMetadata Metadata, const bool bIgnoreCaseInValue)
{
	TArray<FAssetData> FilteredAssets;
	const ESearchCase::Type SearchCase = bIgnoreCaseInValue ? ESearchCase::IgnoreCase : ESearchCase::CaseSensitive;
	
	for (const FAssetData& AssetToFilter : AssetsToFilter)
	{
		TMap<FName, FString> CurrentMetadata = UEditorAssetLibrary::GetMetadataTagValues(AssetToFilter.GetAsset());
		if (!CurrentMetadata.Contains(Metadata.Tag))
			continue;
		
		if (CurrentMetadata.FindRef(Metadata.Tag).Compare(Metadata.Value, SearchCase) == 0) // 0 == Is Equal
			FilteredAssets.Add(AssetToFilter);	
	}
	
	return FilteredAssets;
}

bool UEditorAudioToolsFunctionLibrary::AddMetadataEntriesToAssetObjects(TArray<UObject*> Assets, TArray<FEditorAudioToolsAssetMetadata> MetadataEntries)
{
	constexpr bool bSaveOnlyIfDirty = true;

	for (UObject* Object : Assets)
	{
		for (const auto& [Tag, Value] : MetadataEntries)
		{
			UEditorAssetLibrary::SetMetadataTag(Object, Tag, Value);
		}
	}
	
	return UEditorAssetLibrary::SaveLoadedAssets(Assets, bSaveOnlyIfDirty);
}

bool UEditorAudioToolsFunctionLibrary::RemoveAllMetadataEntriesInAssetObjects(TArray<UObject*> Assets)
{
	constexpr bool bSaveOnlyIfDirty = true;

	for (UObject* Object : Assets)
	{
		for (const auto& [Tag, Value] : UEditorAssetLibrary::GetMetadataTagValues(Object))
		{
			UEditorAssetLibrary::RemoveMetadataTag(Object, Tag);
		}
	}
	
	return UEditorAssetLibrary::SaveLoadedAssets(Assets, bSaveOnlyIfDirty);
}

bool UEditorAudioToolsFunctionLibrary::RemoveMetadataEntriesInAssetObjects(TArray<UObject*> Assets, TArray<FEditorAudioToolsAssetMetadata> MetadataEntries, const bool bIgnoreCaseInValue)
{
	constexpr bool bSaveOnlyIfDirty = true;
	const ESearchCase::Type SearchCase = bIgnoreCaseInValue ? ESearchCase::IgnoreCase : ESearchCase::CaseSensitive;
	
	for (UObject* Object : Assets)
	{
		TMap<FName, FString> CurrentMetadata = UEditorAssetLibrary::GetMetadataTagValues(Object);
		
		for (const auto& [TagToFind, ValueToFind] : MetadataEntries)
		{
			if (!CurrentMetadata.Contains(TagToFind))
				continue;
			
			if (CurrentMetadata.FindRef(TagToFind).Compare(ValueToFind, SearchCase) == 0) // 0 == Is Equal
				UEditorAssetLibrary::RemoveMetadataTag(Object, TagToFind);
		}
	}
	
	return UEditorAssetLibrary::SaveLoadedAssets(Assets, bSaveOnlyIfDirty);
}

bool UEditorAudioToolsFunctionLibrary::RemoveMetadataTagsInAssetObjects(TArray<UObject*> Assets, TArray<FName> Tags)
{
	constexpr bool bSaveOnlyIfDirty = true;
	
	for (UObject* Object : Assets)
	{
		for (const FName Tag : Tags)
		{
			UEditorAssetLibrary::RemoveMetadataTag(Object, Tag);
		}
	}
	
	return UEditorAssetLibrary::SaveLoadedAssets(Assets, bSaveOnlyIfDirty);
}



