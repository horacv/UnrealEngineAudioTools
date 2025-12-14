#pragma once

#include "CoreMinimal.h"

#include "EditorAudioToolsData.generated.h"

UENUM(BlueprintType)
enum EEditorAudioToolsMetadataQuery : uint8
{
	WithTagExcludingEmptyValues UMETA(ToolTip = "Has the tag and a value assigned to that tag"),
	WithTagIncludingEmptyValues UMETA(ToolTip = "Has the tag but the value assigned to that tag can be empty"),
	WithoutTag UMETA(ToolTip = "Does not have the tag"),
	WithoutTagOrWithEmptyValues UMETA(ToolTip = "Does not have the tag or has a different tag with an empty value"),
};

USTRUCT(BlueprintType)
struct FEditorAudioToolsAssetMetadata 
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FName Tag = FName();
	UPROPERTY(BlueprintReadWrite)
	FString Value = FString();
};
