# Majestik: 12 Realms

**A narrative-rich, esoteric isekai RPG adventure built in Unreal Engine 5.5+ powered by ESRPG v5.**

---

## Overview

*Majestik: 12 Realms* is a surreal, mystical journey where players embody a "Starchild"—a cosmic outsider drawn into a realm governed by metaphysical principles and powerful archetypes. Traverse twelve unique Realms, each representing a facet of consciousness and universal truths, and uncover the secrets of the Architects, the Twin Moons Lux and Nox, and the enigmatic prophecy of Ascension.

This project leverages [Easy Survival RPG v5 (ESRPGv5)] for advanced multiplayer RPG systems and includes custom extensions for lore, esoteric gameplay, procedural systems, and isekai manga/anime inspired stylized interaction mechanics. All technical docs will be for internal usage only, GitHub is only for code.

---

## Features

### Core Systems (Powered by ESRPGv5)
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
- ESRPGv5 Template (licensed and imported)
- Optional: LuaMachine Plugin for in-game scriptable ritual logic

### Installation

```bash
git clone https://github.com/beyawnko/Majestik12Realms.git
cd Majestik12Realms
```

0. Merge addional assets/GitHub content with default ESRPGv5 base project from Marketplace.
1. Open `.uproject` in UE5.5+
2. Let Unreal rebuild modules (if required)
3. Configure ESRPGv5 via `BP_GameInstance`, `BP_PlayerManagerComponent`, and other core Blueprints.
4. Enable World Partition, Data Layers, and Chaos for best performance and streaming.

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

### Strategy Systems (See `ESRPG Dev Tips and Plans.pdf`)
- LP/Minimax-based AI dynamic behavior
- Real-time difficulty balancing through dual LP solvers
- Trading system built as a zero-sum payoff matrix

### Lore System
- Dynamic quest branching and cosmic mystery logic
- Faction-based narrative trees (e.g., Order of Eternal Flame vs. Enlightened Order)
- Thematic consistency rooted in esoteric traditions and metaphysics

---

## License

This project is a proprietary game system using licensed ESRPGv5 components. Redistribution or commercial use requires appropriate ESRPG license.

---

## Join the Journey

Embark on a metaphysical odyssey through time, space, and psyche.

> "Balance the Realms, seek the Architects, and let the Twin Moons guide you."

---

## Credits

- Game Design: @beyawnko
- Narrative Direction: @beyawnko, @LoreWriterSenpai
- Tech & Optimization: GameDev Senpai Beyawnko
- ESRPGv5 Template: [Easy Survival RPG on Fab/Unreal Marketplace]

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
            *   `Public/`: Public header files. Includes `MJ12DevPluginModule.h` (main module interface) and `MyTestActor.h` (a sample actor).
            *   `Private/`: Private implementation files. Includes `MJ12DevPluginModule.cpp` and `MyTestActor.cpp`.
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

This plugin serves as a starting point and will be expanded with more specific tools and systems as development progresses.
