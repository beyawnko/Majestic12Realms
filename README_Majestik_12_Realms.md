# Majestik: 12 Realms

**A narrative-rich, esoteric isekai RPG adventure built in Unreal Engine 5.5+ powered by ESRPG v5.**

---

## Overview

*Majestik: 12 Realms* is a surreal, mystical journey where players embody a "Starchild"—a cosmic outsider drawn into a realm governed by metaphysical principles and powerful archetypes. Traverse twelve unique Realms, each representing a facet of consciousness and universal truths, and uncover the secrets of the Architects, the Twin Moons Lux and Nox, and the enigmatic prophecy of Ascension.

This project leverages [Easy Survival RPG v5 (ESRPGv5)] for advanced multiplayer RPG systems and includes custom extensions for lore, esoteric gameplay, procedural systems, and stylized interaction mechanics.

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
git clone https://github.com/YourOrgName/Majestik12Realms.git
cd Majestik12Realms
```

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

- Game Design: @YourNameHere
- Narrative Direction: @LoreWriterSenpai
- Tech & Optimization: GameDev Senpai Aiko
- ESRPGv5 Template: [Easy Survival RPG by White Elk Studios]

---

## Helpful Documentation

- `ESRPG v5 Documentation` (./Docs/Copy_of_ESRPG_v5_Documentation.pdf)
- `Lore Expansion` (./Docs/Majestik_12_Realms_Lore_Expansion.txt)
- `Dev Plans` (./Docs/ESRPG_Dev_Tips_and_Plans.pdf)
- `LuaMachine Plugin` (https://github.com/rdeioris/LuaMachine)
