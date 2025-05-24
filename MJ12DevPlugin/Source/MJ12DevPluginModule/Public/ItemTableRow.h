// ItemTableRow.h
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h" // Required for GameplayTags
#include "ItemTableRow.generated.h"

USTRUCT(BlueprintType)
struct FItemTableRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item Properties")
    FName ItemID; // Should be unique across all items

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item Properties")
    FText DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item Properties")
    TSoftObjectPtr<UStaticMesh> StaticMesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item Properties")
    TSoftObjectPtr<USkeletalMesh> SkeletalMesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item Properties")
    TSoftObjectPtr<UTexture2D> Icon;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item Properties", meta=(ClampMin="1"))
    int32 MaxStackSize = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item Properties")
    float Weight = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item Properties")
    FGameplayTagContainer ItemTags; // Tags for various item properties (e.g., "Item.Consumable", "Item.Weapon", "Item.Equippable")

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item Properties", meta=(EditCondition="ItemTags.HasTag(FGameplayTag::RequestGameplayTag(FName(\"Item.Equippable\")))", EditConditionHides))
    FGameplayTag EquipTargetSlotTag; // Defines the equipment slot this item fits into (e.g., "Equipment.Slot.Weapon", "Equipment.Slot.Head")
};
