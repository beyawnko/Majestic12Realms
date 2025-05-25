// ItemData.h
#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTagContainer.h" // Required for GameplayTags
#include "ItemData.generated.h"

UCLASS(BlueprintType)
class MJ12DEVPLUGINMODULE_API UItemData : public UObject // Changed API macro
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadOnly, Category="Item Data")
    FName ItemID;

    UPROPERTY(BlueprintReadOnly, Category="Item Data")
    FText DisplayName;

    // Soft pointers allow these to be loaded on demand
    UPROPERTY(BlueprintReadOnly, Category="Item Data")
    TSoftObjectPtr<UStaticMesh> StaticMesh;

    UPROPERTY(BlueprintReadOnly, Category="Item Data")
    TSoftObjectPtr<USkeletalMesh> SkeletalMesh;

    UPROPERTY(BlueprintReadOnly, Category="Item Data")
    TSoftObjectPtr<UTexture2D> Icon;

    UPROPERTY(BlueprintReadOnly, Category="Item Data")
    int32 MaxStackSize;

    UPROPERTY(BlueprintReadOnly, Category="Item Data")
    float Weight;

    UPROPERTY(BlueprintReadOnly, Category="Item Data")
    FGameplayTagContainer ItemTags;

    UPROPERTY(BlueprintReadOnly, Category="Item Data")
    FGameplayTag EquipTargetSlotTag;

    // Helper function to check if assets are loaded
    UFUNCTION(BlueprintPure, Category="Item Data")
    bool AreAssetsLoaded() const;
};
