// MJ12DevPluginComponents.cpp

#include "ItemData.h"
#include "InventoryComponent.h"
#include "EquipmentComponent.h"
#include "ItemTableRow.h"

#include "Engine/DataTable.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/Actor.h" 
#include "GameplayTagsManager.h"

// --- UItemData Implementation ---
bool UItemData::AreAssetsLoaded() const
{
    bool bAreLoaded = true;
    if (StaticMesh.ToSoftObjectPath().IsValid() && !StaticMesh.Get()) bAreLoaded = false;
    if (SkeletalMesh.ToSoftObjectPath().IsValid() && !SkeletalMesh.Get()) bAreLoaded = false;
    if (Icon.ToSoftObjectPath().IsValid() && !Icon.Get()) bAreLoaded = false;
    return bAreLoaded;
}


// --- UInventoryComponent Implementation ---

UInventoryComponent::UInventoryComponent()
{
    PrimaryComponentTick.bCanEverTick = false; 
    SetIsReplicatedByDefault(true);
}

void UInventoryComponent::BeginPlay()
{
    Super::BeginPlay();

    if (GetOwnerRole() == ENetRole::ROLE_Authority || GetNetMode() == NM_Standalone)
    {
        for (int32 i = 0; i < Inventory.Num(); ++i)
        {
            if (Inventory[i].ItemID != NAME_None && !Inventory[i].ItemDataInstance)
            {
                RequestLoadItemData(Inventory[i].ItemID, [this, i](UItemData* LoadedData) {
                    HandleItemDataLoaded(Inventory[i].ItemID, LoadedData, i);
                });
            }
        }
    }
}

void UInventoryComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
    for (auto const& [ItemID, Handle] : ActiveLoadHandles)
    {
        if (Handle.IsValid())
        {
            Handle->CancelHandle();
        }
    }
    ActiveLoadHandles.Empty();
    Super::EndPlay(EndPlayReason);
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(UInventoryComponent, Inventory);
}

void UInventoryComponent::OnRep_Inventory()
{
    PendingOnRepLoads = 0;
    bool bAnyLoadRequested = false;

    for (int32 i = 0; i < Inventory.Num(); ++i)
    {
        if (Inventory[i].ItemID != NAME_None && !Inventory[i].ItemDataInstance)
        {
            PendingOnRepLoads++;
            bAnyLoadRequested = true;
            RequestLoadItemData(Inventory[i].ItemID, [this, ItemID = Inventory[i].ItemID, i](UItemData* LoadedData) {
                HandleItemDataLoaded(ItemID, LoadedData, i, true /*bFromOnRep*/);
            });
        }
        else if(Inventory[i].ItemID != NAME_None && Inventory[i].ItemDataInstance)
        {
             OnInventorySlotUpdated.Broadcast(Inventory[i]);
        }
    }

    if (!bAnyLoadRequested) 
    {
        OnInventoryReloaded.Broadcast();
    }
}

void UInventoryComponent::HandleItemDataLoaded(FName ItemID, UItemData* LoadedData, int32 SlotIndex, bool bFromOnRep)
{
    if (!LoadedData)
    {
        UE_LOG(LogMJ12DevPlugin, Warning, TEXT("HandleItemDataLoaded: LoadedData is null for ItemID %s"), *ItemID.ToString());
        if (bFromOnRep)
        {
            PendingOnRepLoads--;
            if (PendingOnRepLoads <= 0)
            {
                OnInventoryReloaded.Broadcast();
            }
        }
        return;
    }

    if (SlotIndex != -1 && Inventory.IsValidIndex(SlotIndex) && Inventory[SlotIndex].ItemID == ItemID)
    {
        Inventory[SlotIndex].ItemDataInstance = LoadedData;
        OnInventorySlotUpdated.Broadcast(Inventory[SlotIndex]);
    }
    else 
    {
        for(FInventorySlot& Slot : Inventory)
        {
            if(Slot.ItemID == ItemID && !Slot.ItemDataInstance) 
            {
                Slot.ItemDataInstance = LoadedData;
                OnInventorySlotUpdated.Broadcast(Slot);
                break; 
            }
        }
    }

    if (bFromOnRep)
    {
        PendingOnRepLoads--;
        if (PendingOnRepLoads <= 0)
        {
            OnInventoryReloaded.Broadcast();
        }
    }
}


bool UInventoryComponent::AddItem(FName ItemID, int32 Quantity)
{
    if (ItemID == NAME_None || Quantity <= 0)
    {
        UE_LOG(LogMJ12DevPlugin, Warning, TEXT("AddItem: Invalid ItemID or Quantity. ItemID: %s, Quantity: %d"), *ItemID.ToString(), Quantity);
        return false;
    }

    if (!ItemDataTable)
    {
        UE_LOG(LogMJ12DevPlugin, Error, TEXT("AddItem: ItemDataTable is not set!"));
        return false;
    }

    FItemTableRow* Row = ItemDataTable->FindRow<FItemTableRow>(ItemID, TEXT("AddItem"));
    if (!Row)
    {
        UE_LOG(LogMJ12DevPlugin, Warning, TEXT("AddItem: ItemID %s not found in ItemDataTable."), *ItemID.ToString());
        return false;
    }

    if (GetOwnerRole() == ROLE_Authority || GetNetMode() == NM_Standalone)
    {
        RequestLoadItemData(ItemID, [this, ItemID, Quantity, RowData = *Row](UItemData* LoadedItemData) {
            if (!LoadedItemData) return; 

            int32 RemainingQuantity = Quantity;
            int32 SlotIndexToUpdate = -1;

            if (LoadedItemData->MaxStackSize > 1)
            {
                for (int32 i = 0; i < Inventory.Num(); ++i)
                {
                    if (Inventory[i].ItemID == ItemID && Inventory[i].Quantity < LoadedItemData->MaxStackSize)
                    {
                        int32 CanAdd = FMath::Min(RemainingQuantity, LoadedItemData->MaxStackSize - Inventory[i].Quantity);
                        Inventory[i].Quantity += CanAdd;
                        RemainingQuantity -= CanAdd;
                        Inventory[i].ItemDataInstance = LoadedItemData; 
                        SlotIndexToUpdate = i;
                        if (RemainingQuantity <= 0) break;
                    }
                }
            }

            while (RemainingQuantity > 0)
            {
                int32 AddAmount = FMath::Min(RemainingQuantity, LoadedItemData->MaxStackSize);
                FInventorySlot NewSlot(ItemID, AddAmount);
                NewSlot.ItemDataInstance = LoadedItemData;
                Inventory.Add(NewSlot); 
                SlotIndexToUpdate = Inventory.Num() - 1; 
                RemainingQuantity -= AddAmount;
            }

            if (GetNetMode() != NM_Client) 
            {
                if (GetNetMode() == NM_ListenServer || GetNetMode() == NM_DedicatedServer)
                {
                    OnRep_Inventory(); 
                }
            }

            if(Inventory.IsValidIndex(SlotIndexToUpdate))
            {
                 OnInventorySlotUpdated.Broadcast(Inventory[SlotIndexToUpdate]);
            }
            else if (Inventory.Num() > 0) 
            {
                OnInventorySlotUpdated.Broadcast(Inventory.Last());
            }
        });
    }
    else
    {
         UE_LOG(LogMJ12DevPlugin, Warning, TEXT("AddItem called on client for %s. This should typically be a server RPC."), *GetOwner()->GetName());
         return false; 
    }

    return true; 
}

bool UInventoryComponent::RemoveItem(FName ItemID, int32 Quantity)
{
    if (ItemID == NAME_None || Quantity <= 0) return false;
    if (GetOwnerRole() < ROLE_Authority) 
    {
        UE_LOG(LogMJ12DevPlugin, Warning, TEXT("RemoveItem called on client. Denied."));
        return false;
    }

    int32 QuantityToRemove = Quantity;
    bool bItemRemoved = false;

    for (int32 i = Inventory.Num() - 1; i >= 0; --i) 
    {
        if (Inventory[i].ItemID == ItemID)
        {
            int32 CanRemove = FMath::Min(QuantityToRemove, Inventory[i].Quantity);
            Inventory[i].Quantity -= CanRemove;
            QuantityToRemove -= CanRemove;

            FInventorySlot UpdatedSlot = Inventory[i]; 

            if (Inventory[i].Quantity <= 0)
            {
                Inventory.RemoveAt(i);
                UpdatedSlot.Quantity = 0; 
            }
            bItemRemoved = true;
            OnInventorySlotUpdated.Broadcast(UpdatedSlot); 

            if (QuantityToRemove <= 0) break;
        }
    }

    if (bItemRemoved && (GetNetMode() == NM_ListenServer || GetNetMode() == NM_DedicatedServer))
    {
        OnRep_Inventory(); 
    }

    return bItemRemoved && QuantityToRemove == 0; 
}

bool UInventoryComponent::RemoveItemAt(int32 SlotIndex, int32 Quantity)
{
    if (!Inventory.IsValidIndex(SlotIndex) || Quantity <= 0) return false;
    if (GetOwnerRole() < ROLE_Authority) 
    {
         UE_LOG(LogMJ12DevPlugin, Warning, TEXT("RemoveItemAt called on client. Denied."));
        return false;
    }

    FInventorySlot& Slot = Inventory[SlotIndex];
    int32 QuantityToRemove = FMath::Min(Quantity, Slot.Quantity);
    Slot.Quantity -= QuantityToRemove;

    FInventorySlot UpdatedSlot = Slot; 

    if (Slot.Quantity <= 0)
    {
        Inventory.RemoveAt(SlotIndex);
        UpdatedSlot.Quantity = 0; 
    }

    OnInventorySlotUpdated.Broadcast(UpdatedSlot);

    if (GetNetMode() == NM_ListenServer || GetNetMode() == NM_DedicatedServer)
    {
        OnRep_Inventory(); 
    }
    return true;
}


int32 UInventoryComponent::GetItemQuantity(FName ItemID) const
{
    int32 TotalQuantity = 0;
    for (const FInventorySlot& Slot : Inventory)
    {
        if (Slot.ItemID == ItemID)
        {
            TotalQuantity += Slot.Quantity;
        }
    }
    return TotalQuantity;
}

bool UInventoryComponent::FindItemSlot(FName ItemID, FInventorySlot& OutSlot, int32& OutSlotIndex) const
{
    for (int32 i = 0; i < Inventory.Num(); ++i)
    {
        if (Inventory[i].ItemID == ItemID)
        {
            OutSlot = Inventory[i];
            OutSlotIndex = i;
            return true;
        }
    }
    OutSlotIndex = -1;
    return false;
}

UItemData* UInventoryComponent::GetItemData(FName ItemID) const
{
    for (const FInventorySlot& Slot : Inventory)
    {
        if (Slot.ItemID == ItemID && Slot.ItemDataInstance)
        {
            return Slot.ItemDataInstance;
        }
    }
    return nullptr;
}


void UInventoryComponent::RequestLoadItemData(FName ItemID, TFunction<void(UItemData*)> Callback)
{
    if (ItemID == NAME_None)
    {
        Callback(nullptr);
        return;
    }

    for (const FInventorySlot& Slot : Inventory)
    {
        if (Slot.ItemID == ItemID && Slot.ItemDataInstance && Slot.ItemDataInstance->AreAssetsLoaded())
        {
            Callback(Slot.ItemDataInstance);
            return;
        }
    }

    if (ActiveLoadHandles.Contains(ItemID) && ActiveLoadHandles[ItemID].IsValid())
    {
        for (const FInventorySlot& Slot : Inventory) {
            if (Slot.ItemID == ItemID && Slot.ItemDataInstance) {
                Callback(Slot.ItemDataInstance); 
                return;
            }
        }
        Callback(nullptr); 
        return;
    }

    if (!ItemDataTable)
    {
        UE_LOG(LogMJ12DevPlugin, Error, TEXT("RequestLoadItemData: ItemDataTable is not set!"));
        Callback(nullptr);
        return;
    }

    FItemTableRow* Row = ItemDataTable->FindRow<FItemTableRow>(ItemID, TEXT("LoadItemData"));
    if (!Row)
    {
        UE_LOG(LogMJ12DevPlugin, Warning, TEXT("RequestLoadItemData: ItemID %s not found in ItemDataTable."), *ItemID.ToString());
        Callback(nullptr);
        return;
    }

    UItemData* NewItemData = NewObject<UItemData>(this); 
    NewItemData->ItemID = Row->ItemID;
    NewItemData->DisplayName = Row->DisplayName;
    NewItemData->StaticMesh = Row->StaticMesh;
    NewItemData->SkeletalMesh = Row->SkeletalMesh;
    NewItemData->Icon = Row->Icon;
    NewItemData->MaxStackSize = Row->MaxStackSize;
    NewItemData->Weight = Row->Weight;
    NewItemData->ItemTags = Row->ItemTags;
    NewItemData->EquipTargetSlotTag = Row->EquipTargetSlotTag;

    TArray<FSoftObjectPath> AssetsToLoad;
    if (Row->StaticMesh.ToSoftObjectPath().IsValid() && !Row->StaticMesh.Get()) AssetsToLoad.Add(Row->StaticMesh.ToSoftObjectPath());
    if (Row->SkeletalMesh.ToSoftObjectPath().IsValid() && !Row->SkeletalMesh.Get()) AssetsToLoad.Add(Row->SkeletalMesh.ToSoftObjectPath());
    if (Row->Icon.ToSoftObjectPath().IsValid() && !Row->Icon.Get()) AssetsToLoad.Add(Row->Icon.ToSoftObjectPath());

    if (AssetsToLoad.Num() > 0)
    {
        FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
        TSharedPtr<FStreamableHandle> Handle = Streamable.RequestAsyncLoad(AssetsToLoad, FStreamableDelegate::CreateUObject(this, &UInventoryComponent::OnAssetsLoaded, NewItemData, Callback, ItemID));
        ActiveLoadHandles.Add(ItemID, Handle);
    }
    else 
    {
        Callback(NewItemData);
    }
}

void UInventoryComponent::OnAssetsLoaded(UItemData* LoadedItemData, TFunction<void(UItemData*)> OriginalCallback, FName ItemID)
{
    ActiveLoadHandles.Remove(ItemID);

    if (IsValid(LoadedItemData))
    {
        OriginalCallback(LoadedItemData);
    }
    else
    {
        UE_LOG(LogMJ12DevPlugin, Warning, TEXT("OnAssetsLoaded: LoadedItemData for %s became invalid before callback execution."), *ItemID.ToString());
        OriginalCallback(nullptr);
    }
}


// --- UEquipmentComponent Implementation ---

UEquipmentComponent::UEquipmentComponent()
{
}

void UEquipmentComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps); 
    DOREPLIFETIME(UEquipmentComponent, EquippedItems);
}

void UEquipmentComponent::OnRep_EquippedItems()
{
    for (auto const& Pair : EquippedItems)
    {
        const FGameplayTag& SlotTag = Pair.Key;
        const FInventorySlot& Slot = Pair.Value;

        if (Slot.ItemID != NAME_None && !Slot.ItemDataInstance)
        {
            RequestLoadItemData(Slot.ItemID, [this, SlotTag, ItemID = Slot.ItemID](UItemData* LoadedData) {
                 if (EquippedItems.Contains(SlotTag) && EquippedItems[SlotTag].ItemID == ItemID)
                 {
                    EquippedItems[SlotTag].ItemDataInstance = LoadedData;
                    OnEquipmentSlotChanged.Broadcast(SlotTag, &EquippedItems[SlotTag]);
                 }
            });
        }
        else if (Slot.ItemID != NAME_None && Slot.ItemDataInstance)
        {
            OnEquipmentSlotChanged.Broadcast(SlotTag, &Slot);
        }
        else if (Slot.ItemID == NAME_None) 
        {
             OnEquipmentSlotChanged.Broadcast(SlotTag, nullptr);
        }
    }
}


bool UEquipmentComponent::CanEquipItem(const UItemData* ItemDataToEquip, FGameplayTag TargetEquipSlotTag) const
{
    if (!ItemDataToEquip)
    {
        UE_LOG(LogMJ12DevPlugin, Warning, TEXT("CanEquipItem: ItemDataToEquip is null."));
        return false;
    }

    if (!ItemDataToEquip->EquipTargetSlotTag.MatchesTag(TargetEquipSlotTag))
    {
        UE_LOG(LogMJ12DevPlugin, Log, TEXT("CanEquipItem: Item %s (targets %s) cannot be equipped in slot %s."),
            *ItemDataToEquip->ItemID.ToString(),
            *ItemDataToEquip->EquipTargetSlotTag.ToString(),
            *TargetEquipSlotTag.ToString());
        return false;
    }

    return true;
}

bool UEquipmentComponent::EquipItemFromInventory(FGameplayTag TargetEquipSlotTag, FName ItemIDFromInventory)
{
    if (GetOwnerRole() < ROLE_Authority)
    {
        UE_LOG(LogMJ12DevPlugin, Warning, TEXT("EquipItemFromInventory called on Client for %s. Denied."), *GetOwner()->GetName());
        return false;
    }

    if (TargetEquipSlotTag == FGameplayTag::EmptyTag || ItemIDFromInventory == NAME_None)
    {
        UE_LOG(LogMJ12DevPlugin, Warning, TEXT("EquipItemFromInventory: Invalid TargetEquipSlotTag or ItemIDFromInventory."));
        return false;
    }

    FInventorySlot ItemToEquipSlotRecord; // Use a record to pass data to lambda if needed, or capture necessary parts
    int32 ItemSlotIndex = -1;
    if (!FindItemSlot(ItemIDFromInventory, ItemToEquipSlotRecord, ItemSlotIndex))
    {
        UE_LOG(LogMJ12DevPlugin, Warning, TEXT("EquipItemFromInventory: Item %s not found in inventory."), *ItemIDFromInventory.ToString());
        return false;
    }

    // Define the core equip logic as a callable lambda or nested function
    // This makes it reusable after ItemData is confirmed loaded.
    auto PerformEquipLogic = [this, TargetEquipSlotTag, ItemIDFromInventory, ItemSlotIndex](UItemData* LoadedItemData)
    {
        if (!LoadedItemData)
        {
            UE_LOG(LogMJ12DevPlugin, Warning, TEXT("EquipItemFromInventory: LoadedItemData is null for %s after async load. Cannot equip."), *ItemIDFromInventory.ToString());
            // Optionally, broadcast a failure event here if needed
            return; // Explicitly not returning bool from lambda to match TFunction<void(UItemData*)> if RequestLoadItemData expects that for some paths.
                    // The outer function's bool return indicates initiation.
        }

        // Ensure the original slot data is up-to-date with the loaded ItemDataInstance
        // This is crucial because ItemToEquipSlotRecord might be a copy.
        // We need to work with the actual inventory item or ensure ItemToEquipSlotRecord is what we need.
        // For simplicity, let's re-fetch or assume the ItemDataInstance in the main inventory array (if applicable)
        // would be set by RequestLoadItemData's internal workings if it caches.
        // Given our RequestLoadItemData implementation, it creates a NEW UItemData.
        // The slot in the main inventory (if this item is from there) would need its ItemDataInstance updated.
        // The lambda in the original RequestLoadItemData call in AddItem handles associating this.
        // For EquipItemFromInventory, the ItemToEquipSlotRecord.ItemDataInstance will be null if it wasn't loaded before.
        // We now have `LoadedItemData`.

        FInventorySlot ActualItemToEquipSlot; // Re-fetch to ensure we have the latest state, especially ItemDataInstance
        int32 TempSlotIndex = -1; // Temporary for re-fetch
        if (!FindItemSlot(ItemIDFromInventory, ActualItemToEquipSlot, TempSlotIndex) || TempSlotIndex != ItemSlotIndex)
        {
             UE_LOG(LogMJ12DevPlugin, Error, TEXT("EquipItemFromInventory: Item %s disappeared from original slot %d during data load. Aborting."), *ItemIDFromInventory.ToString(), ItemSlotIndex);
             return;
        }
        // Crucially, ensure the ItemDataInstance used for CanEquipItem is the one just loaded.
        ActualItemToEquipSlot.ItemDataInstance = LoadedItemData;


        if (!CanEquipItem(ActualItemToEquipSlot.ItemDataInstance, TargetEquipSlotTag))
        {
            // CanEquipItem logs the reason
            return;
        }

        // If slot is already occupied, unequip old item first
        if (EquippedItems.Contains(TargetEquipSlotTag))
        {
            FName OldItemID = EquippedItems[TargetEquipSlotTag].ItemID;
            // Temporarily store the item being unequipped to avoid issues if it's the same item ID being swapped
            FInventorySlot UnequippedItemDetails = EquippedItems[TargetEquipSlotTag];

            if (!UnequipItem(TargetEquipSlotTag)) // Unequip first
            {
                UE_LOG(LogMJ12DevPlugin, Warning, TEXT("EquipItemFromInventory: Failed to unequip previous item from slot %s."), *TargetEquipSlotTag.ToString());
                // If UnequipItem failed to add back to inventory, and we add it back now, ensure it does not interfere with the item being equipped.
                // This part of logic can get complex if inventory space is an issue. AddItem has its own checks.
                return; // Failed to unequip, cannot proceed
            }
            UE_LOG(LogMJ12DevPlugin, Log, TEXT("EquipItemFromInventory: Unequipped %s to make space for %s in slot %s."), *OldItemID.ToString(), *ItemIDFromInventory.ToString(), *TargetEquipSlotTag.ToString());
        }

        // Create the equipped item slot (typically quantity 1 for equipment)
        FInventorySlot EquippedSlot(ItemIDFromInventory, 1);
        EquippedSlot.ItemDataInstance = ActualItemToEquipSlot.ItemDataInstance; // Use the loaded ItemData

        // Remove one instance from inventory BEFORE adding to equipped map
        // to handle cases where inventory is full and unequip->additem fails.
        if (!RemoveItem(ItemIDFromInventory, 1)) // Use base RemoveItem
        {
            UE_LOG(LogMJ12DevPlugin, Error, TEXT("EquipItemFromInventory: Failed to remove item %s from inventory for equip! Aborting equip."), *ItemIDFromInventory.ToString());
            // If the item we just unequipped needs to be restored to the slot, this logic would be more complex.
            // For now, assume failure to remove from inventory means we cannot proceed with equipping.
            // Consider if the previously unequipped item needs to be re-equipped or if its AddItem in UnequipItem was successful.
            return;
        }

        EquippedItems.Add(TargetEquipSlotTag, EquippedSlot);
        UE_LOG(LogMJ12DevPlugin, Log, TEXT("EquipItemFromInventory: Item %s equipped to slot %s."), *ItemIDFromInventory.ToString(), *TargetEquipSlotTag.ToString());

        if (GetNetMode() == NM_ListenServer || GetNetMode() == NM_DedicatedServer)
        {
            OnRep_EquippedItems();
        }
        else if (GetNetMode() == NM_Standalone)
        {
            OnEquipmentSlotChanged.Broadcast(TargetEquipSlotTag, &EquippedItems[TargetEquipSlotTag]);
        }
    };

    // If ItemData is already loaded in the slot from inventory, use it directly.
    if (ItemToEquipSlotRecord.ItemDataInstance && ItemToEquipSlotRecord.ItemDataInstance->AreAssetsLoaded())
    {
        UE_LOG(LogMJ12DevPlugin, Log, TEXT("EquipItemFromInventory: ItemData for %s already loaded. Proceeding synchronously."), *ItemIDFromInventory.ToString());
        PerformEquipLogic(ItemToEquipSlotRecord.ItemDataInstance);
    }
    else // ItemData not loaded or assets not ready, request load.
    {
        UE_LOG(LogMJ12DevPlugin, Log, TEXT("EquipItemFromInventory: ItemData for %s not loaded or assets pending. Requesting async load."), *ItemIDFromInventory.ToString());
        // Pass the ItemSlotIndex to ensure we can update the correct FInventorySlot's ItemDataInstance
        // The lambda passed to RequestLoadItemData in UInventoryComponent already handles setting ItemDataInstance for a given slot index.
        // We need to ensure that the 'ItemToEquipSlotRecord' gets its ItemDataInstance updated if it's a copy,
        // or that PerformEquipLogic uses the correctly updated instance from the main Inventory array.
        // The RequestLoadItemData in InventoryComponent updates the FInventorySlot in its Inventory TArray.
        // So, PerformEquipLogic should ideally re-fetch the FInventorySlot or be passed the ItemData directly.
        // The ItemSlotIndex is captured by PerformEquipLogic to potentially re-verify.

        RequestLoadItemData(ItemIDFromInventory, [this, PerformEquipLogic, ItemSlotIndex, ItemIDFromInventory](UItemData* LoadedData) {
            // After data is loaded (or load failed) by the base RequestLoadItemData:
            if (LoadedData)
            {
                 // If the item still exists in the original inventory slot, ensure its ItemDataInstance is updated.
                 // This is a bit redundant if RequestLoadItemData's callback (HandleItemDataLoaded) already did this,
                 // but double-checking doesn't hurt, or rely on FindItemSlot inside PerformEquipLogic.
                if (Inventory.IsValidIndex(ItemSlotIndex) && Inventory[ItemSlotIndex].ItemID == ItemIDFromInventory)
                {
                    Inventory[ItemSlotIndex].ItemDataInstance = LoadedData;
                }
            }
            // Now call the core equip logic with the loaded data (which might be null if load failed)
            PerformEquipLogic(LoadedData);
        });
    }

    return true; // Indicates the equip process has been initiated. Actual success is asynchronous.
}

bool UEquipmentComponent::EquipItemByCreatingInstance(FGameplayTag TargetEquipSlotTag, FName ItemIDToCreateAndEquip)
{
    if (GetOwnerRole() < ROLE_Authority)
    {
        UE_LOG(LogMJ12DevPlugin, Warning, TEXT("EquipItemByCreatingInstance called on Client. Denied."));
        return false;
    }

    if (TargetEquipSlotTag == FGameplayTag::EmptyTag || ItemIDToCreateAndEquip == NAME_None) return false;

    RequestLoadItemData(ItemIDToCreateAndEquip, [this, TargetEquipSlotTag, ItemIDToCreateAndEquip](UItemData* LoadedItemData){
        if (!LoadedItemData)
        {
            UE_LOG(LogMJ12DevPlugin, Warning, TEXT("EquipItemByCreatingInstance: Could not load ItemData for %s."), *ItemIDToCreateAndEquip.ToString());
            return;
        }

        if (!CanEquipItem(LoadedItemData, TargetEquipSlotTag))
        {
            return;
        }

        if (EquippedItems.Contains(TargetEquipSlotTag))
        {
            if (!UnequipItem(TargetEquipSlotTag))
            {
                UE_LOG(LogMJ12DevPlugin, Warning, TEXT("EquipItemByCreatingInstance: Failed to unequip previous item from slot %s."), *TargetEquipSlotTag.ToString());
                return;
            }
        }

        FInventorySlot NewEquippedSlot(ItemIDToCreateAndEquip, 1);
        NewEquippedSlot.ItemDataInstance = LoadedItemData;
        EquippedItems.Add(TargetEquipSlotTag, NewEquippedSlot);

        UE_LOG(LogMJ12DevPlugin, Log, TEXT("EquipItemByCreatingInstance: Item %s created and equipped to slot %s."), *ItemIDToCreateAndEquip.ToString(), *TargetEquipSlotTag.ToString());

        if (GetNetMode() == NM_ListenServer || GetNetMode() == NM_DedicatedServer) OnRep_EquippedItems();
        else if (GetNetMode() == NM_Standalone) OnEquipmentSlotChanged.Broadcast(TargetEquipSlotTag, &EquippedItems[TargetEquipSlotTag]);
    });

    return true; 
}


bool UEquipmentComponent::UnequipItem(FGameplayTag EquipSlotTag)
{
    if (GetOwnerRole() < ROLE_Authority)
    {
        UE_LOG(LogMJ12DevPlugin, Warning, TEXT("UnequipItem called on Client. Denied."));
        return false; 
    }

    if (!EquippedItems.Contains(EquipSlotTag))
    {
        UE_LOG(LogMJ12DevPlugin, Warning, TEXT("UnequipItem: No item in slot %s."), *EquipSlotTag.ToString());
        return false;
    }

    FInventorySlot UnequippedSlot = EquippedItems[EquipSlotTag];
    EquippedItems.Remove(EquipSlotTag);

    if (!AddItem(UnequippedSlot.ItemID, UnequippedSlot.Quantity)) 
    {
        UE_LOG(LogMJ12DevPlugin, Error, TEXT("UnequipItem: Failed to add item %s back to inventory! Item may be lost if not handled."), *UnequippedSlot.ItemID.ToString());
    }

    UE_LOG(LogMJ12DevPlugin, Log, TEXT("UnequipItem: Item %s unequipped from slot %s and returned to inventory."), *UnequippedSlot.ItemID.ToString(), *EquipSlotTag.ToString());

    if (GetNetMode() == NM_ListenServer || GetNetMode() == NM_DedicatedServer) OnRep_EquippedItems();
    else if (GetNetMode() == NM_Standalone) OnEquipmentSlotChanged.Broadcast(EquipSlotTag, nullptr);

    return true;
}

bool UEquipmentComponent::GetEquippedItem(FGameplayTag EquipSlotTag, FInventorySlot& OutEquippedItem) const
{
    if (EquippedItems.Contains(EquipSlotTag))
    {
        OutEquippedItem = EquippedItems[EquipSlotTag];
        return true;
    }
    return false;
}
