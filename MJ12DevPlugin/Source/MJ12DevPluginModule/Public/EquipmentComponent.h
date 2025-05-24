// EquipmentComponent.h
#pragma once

#include "CoreMinimal.h"
#include "InventoryComponent.h" // Inherits from UInventoryComponent
#include "GameplayTagContainer.h"  // For FGameplayTag
#include "EquipmentComponent.generated.h"

// Delegate broadcast when an equipment slot changes (item equipped/unequipped and data loaded).
// Provides the slot tag and the FInventorySlot (or nullptr if unequipped).
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEquipmentSlotChanged, FGameplayTag, SlotTag, const FInventorySlot*, SlotData);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MJ12DEVPLUGINMODULE_API UEquipmentComponent : public UInventoryComponent // Changed API macro
{
    GENERATED_BODY()

public:
    UEquipmentComponent();

    UPROPERTY(BlueprintAssignable, Category="Equipment")
    FOnEquipmentSlotChanged OnEquipmentSlotChanged;

    // Using FGameplayTag for EquipSlot key for flexibility and data-driven slot definitions
    UPROPERTY(ReplicatedUsing=OnRep_EquippedItems, BlueprintReadOnly, Category="Equipment")
    TMap<FGameplayTag, FInventorySlot> EquippedItems;

    UFUNCTION(BlueprintCallable, Category="Equipment")
    bool EquipItemFromInventory(FGameplayTag TargetEquipSlotTag, FName ItemIDFromInventory);

    UFUNCTION(BlueprintCallable, Category="Equipment")
    bool EquipItemByCreatingInstance(FGameplayTag TargetEquipSlotTag, FName ItemIDToCreateAndEquip);

    UFUNCTION(BlueprintCallable, Category="Equipment")
    bool UnequipItem(FGameplayTag EquipSlotTag);

    UFUNCTION(BlueprintPure, Category="Equipment")
    bool GetEquippedItem(FGameplayTag EquipSlotTag, FInventorySlot& OutEquippedItem) const;

protected:
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UFUNCTION()
    void OnRep_EquippedItems();

private:
    bool CanEquipItem(const UItemData* ItemDataToEquip, FGameplayTag TargetEquipSlotTag) const;
    void HandleEquippedItemDataLoaded(FGameplayTag SlotTag, UItemData* LoadedData);
};
