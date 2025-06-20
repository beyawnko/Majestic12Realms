# Majestik: 12 Realms

**A narrative-rich, esoteric isekai RPG adventure built in Unreal Engine 5.5+**

---

## Overview

*Majestik: 12 Realms* is a surreal, mystical journey where players embody a "Starchild"—a cosmic outsider drawn into a realm governed by metaphysical principles and powerful archetypes. Traverse twelve unique Realms, each representing a facet of consciousness and universal truths, and uncover the secrets of the Architects, the Twin Moons Lux and Nox, and the enigmatic prophecy of Ascension.

This project leverages advanced multiplayer RPG systems and includes custom extensions for lore, esoteric gameplay, procedural systems, and isekai manga/anime inspired stylized interaction mechanics. All technical docs will be for internal usage only, GitHub is only for code.

---

## Features

### Core Systems
- Multiplayer survival RPG foundation
- Inventory, crafting, quests, and dialogue systems
- Fully replicated building and container systems
- Attributes, status effects, and advanced combat
- Modular skill tree, abilities, and spell book

### Majestik Extensions
- Authentic Path System: Deep choice-based narrative and consequence system
- Psyche Meter: Mental state system affecting world perception and abilities
- Esoteric Practices: Rituals, glyphs, and incantations as game mechanics
- Cosmic Lore Engine: Dynamic NPC lore propagation and mystery unraveling
- 12 Unique Realms: Each with its own biome, challenges, Guardian, and philosophical theme
- Fractal Labyrinth: Rogue-lite subconscious dungeons linked to personal growth

---

## Getting Started

### Prerequisites
- Unreal Engine 5.5+
- Visual Studio 2022 with UE integrations
- Licensed Assets (the actual game files)
- Optional: LuaMachine Plugin for in-game scriptable ritual logic

### Installation

```bash
git clone https://github.com/beyawnko/Majestik12Realms.git
cd Majestik12Realms
```

0. Merge addional assets/GitHub content with default base project (obtained after onboarding).
1. Open `.uproject` in UE5.5+
2. Let Unreal rebuild modules (if required)
3. Configure Majestik via `BP_GameInstance`, `BP_PlayerManagerComponent`, and other core Blueprints.
4. Enable World Partition, Nanite, Data Layers, and Chaos for best performance and streaming (constantly changing).

---

## Realms of Majestik

| Realm | Theme | Guardian |
|-------|-------|----------|
| Boundless Expanse | Infinity & Cosmic Truths | Aethyric Serpent |
| Flame Eternal | Rebirth & Renewal | Phoenix |
| Crowned Victor's Domain | Power & Sovereignty | Lion |
| Whisperer's Labyrinth | Secrets & Duality | Wyrm |
| Crimson Garden | Desire & Sacrifice | Rose Saint |
| ... | ... | ... |

Each Realm has custom gameplay mechanics, environmental hazards, and Realm Keys required for progression.

---

## Developer Notes

### Memory Optimization (Block Asset Load from ESRPG default)
- Use soft references in `DT_Items` to reduce memory bloat.
- Employ async loading (e.g., `LoadSkeletalMeshAsync`) where possible.
- Custom asset loader via C++ recommended for larger modular asset sets.

### Strategy Systems (See 'Dev Tips and Plans.pdf`)
- LP/Minimax-based AI dynamic behavior
- Real-time difficulty balancing through dual LP solvers
- Trading system built as a zero-sum payoff matrix

### Lore System
- Dynamic quest branching and cosmic mystery logic
- Faction-based narrative trees (e.g., Order of Eternal Flame vs. Enlightened Order)
- Thematic consistency rooted in esoteric traditions and metaphysics

---

## License

This project is a proprietary game system using licensed components. Redistribution or commercial use requires appropriate license.

---

## Join the Journey

Embark on a metaphysical odyssey through time, space, and psyche.

> "Balance the Realms, seek the Architects, and let the Twin Moons guide you."

---

## Credits

- Game Design: @beyawnko
- Narrative Direction: @beyawnko, @LoreWriterSenpai
- Tech & Optimization: GameDev Senpai Beyawnko
---

## Helpful Documentation

- TBD

---

## MJ12DevPlugin

This plugin, `MJ12DevPlugin`, is a foundational C++ framework designed to house common development tools, utilities, and custom gameplay systems for the *Majestik: 12 Realms* project.

### Purpose

*   **Centralization:** Provides a central location for custom C++ code, keeping it modular and separate from the main game project or other marketplace assets.
*   **Rapid Development:** Enables faster iteration on new gameplay mechanics, editor tools, and utility functions.
*   **Maintainability:** Simplifies updates and management of shared code components.

### Structure

The plugin is organized as follows:

*   `MJ12DevPlugin/` (Root directory)
    *   `MJ12DevPlugin.uplugin`: The plugin descriptor file.
    *   `Source/`: Contains the plugin's source code.
        *   `MJ12DevPluginModule/`: The primary module for the plugin.
            *   `Public/`: Public header files. Includes `MJ12DevPluginModule.h` (main module interface), `ItemTableRow.h`, `ItemData.h`, `InventoryComponent.h`, and `EquipmentComponent.h`.
            *   `Private/`: Private implementation files. Includes `MJ12DevPluginModule.cpp` and `MJ12DevPluginComponents.cpp` (containing implementations for inventory/equipment).
            *   `MJ12DevPluginModule.Build.cs`: The build script for the module.
    *   `Content/`: For any plugin-specific assets (currently empty).

### Getting Started with the Plugin

1.  **Ensure the plugin is in the project's `Plugins` directory.** If it's not already there (e.g., if cloned separately), copy the `MJ12DevPlugin` folder into your project's `Plugins` folder. Create a `Plugins` folder at the root of your Unreal project if it doesn't exist.
2.  **Enable the Plugin:** Open your Unreal Engine project, go to `Edit -> Plugins`, search for "MJ12 Development Plugin" (or "MJ12"), and ensure it's enabled. Restart the editor if prompted.
3.  **Compile:** The plugin code will be compiled automatically when you build your project. If you add new C++ classes to the plugin, you'll need to compile from your IDE (Visual Studio, Rider, etc.) or trigger a build from the Unreal Editor.

### Adding New Functionality

*   **New C++ Classes:** Add new `.h` files to `Source/MJ12DevPluginModule/Public/` and corresponding `.cpp` files to `Source/MJ12DevPluginModule/Private/`.
*   **Editor Modules/Tools:** For editor-specific functionality, you might add a new module of type "Editor" within the `Source` directory and update the `.uplugin` file accordingly.
*   **Content:** Add any plugin-specific Blueprints, materials, etc., to the `MJ12DevPlugin/Content/` directory.

This plugin serves as a starting point and is being expanded with core gameplay systems and specific tools.

#### Core Gameplay Systems

The `MJ12DevPlugin` now includes a foundational Inventory and Equipment system:

*   **Item Management:**
    *   Items are defined in DataTables using the `FItemTableRow` struct (`Source/MJ12DevPluginModule/Public/ItemTableRow.h`).
    *   Runtime item instances are represented by `UItemData` objects (`Source/MJ12DevPluginModule/Public/ItemData.h`), which support asynchronous loading of associated assets (meshes, icons).
*   **Inventory Component (`UInventoryComponent`):**
    *   Located in `Source/MJ12DevPluginModule/Public/InventoryComponent.h`.
    *   Manages a collection of items (`FInventorySlot`).
    *   Supports replication for multiplayer.
    *   Provides delegates (`OnInventorySlotUpdated`, `OnInventoryReloaded`) for UI and game logic to respond to changes.
    *   Handles asynchronous loading of item data when items are added or replicated.
*   **Equipment Component (`UEquipmentComponent`):**
    *   Located in `Source/MJ12DevPluginModule/Public/EquipmentComponent.h`.
    *   Manages equipped items, using `FGameplayTag` to define equipment slots (e.g., "Equipment.Slot.Weapon").
    *   Inherits from `UInventoryComponent`, potentially allowing shared inventory space or logic.
    *   Supports replication and provides a delegate (`OnEquipmentSlotChanged`) for updates.
    *   Handles asynchronous data loading for equipping items, including a refactored `EquipItemFromInventory` method to correctly manage data dependencies.

These systems provide a robust base for handling items and character equipment within the game.
