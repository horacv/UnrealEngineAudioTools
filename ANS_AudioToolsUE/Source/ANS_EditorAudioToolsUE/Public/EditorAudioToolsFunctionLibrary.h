#pragma once

#include "AssetRegistry/IAssetRegistry.h"
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "EditorAudioToolsFunctionLibrary.generated.h"

enum EEditorAudioToolsMetadataQuery : uint8;
struct FEditorAudioToolsAssetMetadata;

UCLASS()
class ANS_EDITORAUDIOTOOLSUE_API UEditorAudioToolsFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	/**
	 * Opens the Asset Audit UI for the specified assets in the Unreal Editor.
	 * This function provides access to Unreal's built-in asset auditing tools.
	 * @param Assets Array of asset data objects to audit. Each FAssetData represents an asset in the project.
	 * @note This function is editor-only and will only work in the Unreal Editor environment.
	 */
	UFUNCTION(BlueprintCallable, Category = "Editor Audio Tools UE", meta = (DisplayName = "Audit Assets"))
	static void AuditAssets(TArray<FAssetData> Assets);

	/**
	 * Retrieves all unique classes from a collection of asset objects.
	 * Iterates through the provided assets and collects their class types into a set to ensure uniqueness,
	 * then outputs the results as an array.
	 * @param Assets Array of asset objects to analyze for their class types.
	 * @param OutClasses [out] Array that will be populated with the unique classes found in the asset collection.
	 * @return True if any classes were found, false if the input array was empty or no classes were collected.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Editor Audio Tools UE", meta = (DisplayName = "Get Classes In Asset Collection"))
	static bool GetClassesInAssetCollection(const TArray<UObject*> Assets, TArray<UClass*>& OutClasses);

	/**
	 * Retrieves all assets in the project that match the specified class types.
	 * Queries the Asset Registry to find assets of the given classes, with optional support for derived classes.
	 * @param Classes Array of class types to search for in the Asset Registry.
	 * @param bSearchSubClasses If true, includes assets of classes derived from the specified classes. If false, only exact class matches are returned.
	 * @param OutAssets [out] Array that will be populated with the asset data for all matching assets found.
	 * @return True if any assets were found matching the specified classes, false if no assets were found or if the Classes array was empty.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Editor Audio Tools UE", meta = (DisplayName = "Get All Assets Of Classes"))
	static bool GetAllAssetsOfClasses(const TArray<UClass*> Classes, const bool bSearchSubClasses, TArray<FAssetData>& OutAssets);

	/**
	 * Filters a collection of assets to find only those that are not referenced by any other assets.
	 * Checks each asset in the collection using the Asset Registry to determine if it has any referencers
	 * based on the specified dependency options. Assets with no referencers are considered unreferenced.
	 * @param AssetCollection Array of asset data objects to check for references. Each FAssetData represents an asset to evaluate.
	 * @param ReferenceOptions Options that define how to query asset dependencies (e.g., hard references, soft references, searchable names).
	 * @return Array of FAssetData containing only the assets from the input collection that have no referencers.
	 * @note This function is useful for identifying unused assets that may be safe to delete or archive.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Editor Audio Tools UE", meta = (DisplayName = "Get All Unreferenced Assets In Collection"))
	static UPARAM(DisplayName="FilteredAssets") TArray<FAssetData> GetAllUnreferencedAssetsInCollection(TArray<FAssetData> AssetCollection, const FAssetRegistryDependencyOptions& ReferenceOptions);

	/**
	 * Filters assets based on metadata tag presence and value state.
	 * Examines each asset's metadata to determine if it matches the specified tag query criteria.
	 * The query can check for tag existence, tag absence, or value emptiness based on the QueryType parameter.
	 * @param AssetsToFilter Array of asset data objects to filter based on metadata tag criteria.
	 * @param Tag The metadata tag name to search for in each asset's metadata.
	 * @param QueryType Defines the filtering criteria - whether to include assets with the tag (with or without values), 
	 *				  without the tag, or without the tag/with empty values. See EEditorAudioToolsMetadataQuery for options.
	 * @return Array of FAssetData containing only the assets that match the specified metadata query criteria.
	 * @see FilterAssetsByMetadataTagAndValue for filtering by both tag and specific value
	 */
	UFUNCTION(BlueprintCallable, Category = "Editor Audio Tools UE", meta = (DisplayName = "Filter Assets By Metadata Tag"))
	static UPARAM(DisplayName="FilteredAssets") TArray<FAssetData> FilterAssetsByMetadataTag(TArray<FAssetData> AssetsToFilter, FName Tag, EEditorAudioToolsMetadataQuery QueryType);

	/**
	 * Filters assets based on both metadata tag and specific value matching.
	 * Examines each asset's metadata to find assets where the specified tag exists with an exact value match.
	 * This function performs a more specific filter than FilterAssetsByMetadataTag by requiring both tag existence and value equality.
	 * @param AssetsToFilter Array of asset data objects to filter based on metadata tag and value criteria.
	 * @param Metadata Structure containing the Tag (FName) and Value (FString) to match against asset metadata.
	 * @param bIgnoreCaseInValue If true, performs case-insensitive string comparison when matching values. If false, requires an exact case match.
	 * @return Array of FAssetData containing only the assets where the metadata tag exists and its value matches the specified value.
	 * @see FilterAssetsByMetadataTag for filtering by tag presence only without value matching
	 */
	UFUNCTION(BlueprintCallable, Category = "Editor Audio Tools UE", meta = (DisplayName = "Filter Assets By Metadata Tag And Value"))
	static UPARAM(DisplayName="FilteredAssets") TArray<FAssetData> FilterAssetsByMetadataTagAndValue(TArray<FAssetData> AssetsToFilter, FEditorAudioToolsAssetMetadata Metadata, const bool bIgnoreCaseInValue = true);

	/**
	 * Adds metadata tag-value pairs to multiple asset objects.
	 * Iterates through the provided assets and applies each metadata entry (tag and value) to them.
	 * The assets are automatically saved and checked out after modification (only if changes were made).
	 * @param Assets Array of asset objects to which metadata entries will be added. Each asset must be a valid UObject.
	 * @param MetadataEntries Array of metadata entries to add. Each entry contains a Tag (FName) and Value (FString) pair.
	 * @return True if the assets were successfully saved after adding metadata, false otherwise.
	 * @note This function modifies and saves assets. Can't be reverted with undo. Use with caution as it will mark assets as dirty and trigger source control operations.
	 * @see RemoveMetadataEntriesInAssetObjects for removing specific metadata entries
	 * @see RemoveAllMetadataEntriesInAssetObjects for removing all metadata from assets
	 * @see RemoveMetadataTagsInAssetObjects for removing metadata by tag only (regardless of value)
	 */
	UFUNCTION(BlueprintCallable, Category = "Editor Audio Tools UE", meta = (DisplayName = "Add Metadata Entries To Asset Objects"))
	static bool AddMetadataEntriesToAssetObjects(TArray<UObject*> Assets, TArray<FEditorAudioToolsAssetMetadata> MetadataEntries);

	/**
	 * Removes all metadata entries from multiple asset objects.
	 * Iterates through each asset and removes all of its metadata tag-value pairs.
	 * The assets are automatically saved and checked out after modification (only if changes were made).
	 * @param Assets Array of asset objects from which all metadata entries will be removed. Each asset must be a valid UObject.
	 * @return True, if the assets were successfully saved after removing all metadata, false otherwise.
	 * @note This function modifies and saves assets. Can't be reverted with undo. Use with caution as it will mark assets as dirty and trigger source control operations.
	 * @see RemoveMetadataEntriesInAssetObjects for removing specific metadata entries by tag and value
	 * @see RemoveMetadataTagsInAssetObjects for removing specific metadata tags
	 * @see AddMetadataEntriesToAssetObjects for adding metadata by tag and value
	 */
	UFUNCTION(BlueprintCallable, Category = "Editor Audio Tools UE", meta = (DisplayName = "Remove All Metadata Entries In Asset Objects"))
	static bool RemoveAllMetadataEntriesInAssetObjects(TArray<UObject*> Assets);

	/**
	 * Removes specific metadata tag-value pairs from multiple asset objects.
	 * Iterates through each asset and removes metadata entries only when both the tag name and value match the specified criteria.
	 * Only metadata entries with matching tag names AND values (according to case sensitivity setting) are removed.
	 * The assets are automatically saved and checked out after modification (only if changes were made).
	 * @param Assets Array of asset objects from which matching metadata entries will be removed. Each asset must be a valid UObject.
	 * @param MetadataEntries Array of metadata entries to remove. Each entry contains a Tag (FName) and Value (FString) pair that must match exactly.
	 * @param bIgnoreCaseInValue If true, performs case-insensitive string comparison when matching values. If false, requires an exact case match.
	 * @return True, if the assets were successfully saved after removing metadata entries, false otherwise.
	 * @note This function modifies and saves assets. Can't be reverted with undo. Use with caution as it will mark assets as dirty and trigger source control operations.
	 * @see RemoveAllMetadataEntriesInAssetObjects for removing all metadata from assets
	 * @see RemoveMetadataTagsInAssetObjects for removing metadata by tag only (regardless of value)
	 * @see AddMetadataEntriesToAssetObjects for adding metadata by tag and value
	 */
	UFUNCTION(BlueprintCallable, Category = "Editor Audio Tools UE", meta = (DisplayName = "Remove Metadata Entries In Asset Objects"))
	static bool RemoveMetadataEntriesInAssetObjects(TArray<UObject*> Assets, TArray<FEditorAudioToolsAssetMetadata> MetadataEntries, const bool bIgnoreCaseInValue);

	/**
	 * Removes specific metadata tags from multiple asset objects regardless of their values.
	 * Iterates through each asset and removes all metadata entries that match any of the specified tag names.
	 * Unlike RemoveMetadataEntriesInAssetObjects, this function removes tags without checking their values.
	 * The assets are automatically saved and checked out after modification (only if changes were made).
	 * @param Assets Array of asset objects from which the specified metadata tags will be removed. Each asset must be a valid UObject.
	 * @param Tags Array of metadata tag names (FName) to remove from the assets. Any metadata entry with a matching tag name will be removed.
	 * @return True if the assets were successfully saved after removing metadata tags, false otherwise.
	 * @note This function modifies and saves assets. Can't be reverted with undo. Use with caution as it will mark assets as dirty and trigger source control operations.
	 * @see RemoveMetadataEntriesInAssetObjects for removing metadata by both tag and value
	 * @see RemoveAllMetadataEntriesInAssetObjects for removing all metadata from assets
	 * @see AddMetadataEntriesToAssetObjects for adding metadata by tag and value
	 */
	UFUNCTION(BlueprintCallable, Category = "Editor Audio Tools UE", meta = (DisplayName = "Remove Metadata Tags In Asset Objects"))
	static bool RemoveMetadataTagsInAssetObjects(TArray<UObject*> Assets, TArray<FName> Tags);
};