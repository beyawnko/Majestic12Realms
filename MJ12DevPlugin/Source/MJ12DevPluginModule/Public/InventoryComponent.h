// InventoryComponent.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ItemTableRow.h" // For FItemTableRow (though primarily used in .cpp)
#include "ItemData.h"     // For UItemData
#include "InventoryComponent.generated.h"

// Forward declaration for FStreamableHandle
struct FStreamableHandle;

USTRUCT(BlueprintType)
struct FInventorySlot
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Inventory Slot") // ItemID is set internally
    FName ItemID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory Slot", meta=(ClampMin="0")) // Quantity can be modified
    int32 Quantity;

    UPROPERTY(BlueprintReadOnly, Category="Inventory Slot", Transient) // ItemData is runtime, not saved, reloaded from ItemID
    UItemData* ItemDataInstance;

    // Default constructor
    FInventorySlot() : ItemID(NAME_None), Quantity(0), ItemDataInstance(nullptr) {}
    FInventorySlot(FName InItemID, int32 InQuantity) : ItemID(InItemID), Quantity(InQuantity), ItemDataInstance(nullptr) {}

    bool operator==(const FName& InItemID) const
    {
        return ItemID == InItemID;
    }
};

// Delegate broadcast when a specific slot's data (especially UItemData) has been updated.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInventorySlotUpdated, const FInventorySlot&, UpdatedSlot);
// Delegate broadcast when the entire inventory has undergone a significant change (e.g., after initial replication and data loading).
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryReloaded);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MJ12DEVPLUGINMODULE_API UInventoryComponent : public UActorComponent // Changed API macro
{
    GENERATED_BODY()

public:
    UInventoryComponent();

    UPROPERTY(BlueprintAssignable, Category="Inventory")
    FOnInventorySlotUpdated OnInventorySlotUpdated;

    UPROPERTY(BlueprintAssignable, Category="Inventory")
    FOnInventoryReloaded OnInventoryReloaded;

    UPROPERTY(ReplicatedUsing=OnRep_Inventory, BlueprintReadOnly, Category="Inventory")
    TArray<FInventorySlot> Inventory;

    UFUNCTION(BlueprintCallable, Category="Inventory")
    bool AddItem(FName ItemID, int32 Quantity = 1);

    UFUNCTION(BlueprintCallable, Category="Inventory")
    bool RemoveItem(FName ItemID, int32 Quantity = 1);

    UFUNCTION(BlueprintCallable, Category="Inventory")
    bool RemoveItemAt(int32 SlotIndex, int32 Quantity = 1);

    UFUNCTION(BlueprintPure, Category="Inventory")
    int32 GetItemQuantity(FName ItemID) const;

    UFUNCTION(BlueprintPure, Category="Inventory")
    bool FindItemSlot(FName ItemID, FInventorySlot& OutSlot, int32& OutSlotIndex) const;

    UFUNCTION(BlueprintPure, Category="Inventory")
    UItemData* GetItemData(FName ItemID) const;


protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UFUNCTION()
    virtual void OnRep_Inventory();

    // Central function to load item data from DataTable and handle async asset loading
    // Callback is invoked once ItemData is created and its core assets (meshes, icon) are loaded or confirmed valid.
    void RequestLoadItemData(FName ItemID, TFunction<void(UItemData* LoadedData)> Callback);

    UPROPERTY(EditDefaultsOnly, Category="Inventory", meta=(RequiredAssetDataTags="RowStructure=ItemTableRow"))
    UDataTable* ItemDataTable;

private:
    // Helper to find an existing stackable slot or an empty slot
    int32 FindAvailableSlotIndex(FName ItemID, const UItemData* ItemDataDefinition) const;

    // Manages active async load requests for ItemData assets
    TMap<FName, TSharedPtr<FStreamableHandle>> ActiveLoadHandles;

    // Counter for pending OnRep loads to fire OnInventoryReloaded once
    int32 PendingOnRepLoads = 0;
    void HandleItemDataLoaded(FName ItemID, UItemData* LoadedData, int32 SlotIndex = -1, bool bFromOnRep = false);
};
