# CHANGELOG

## [1.7.0] - 2026-02-12
### Added
- **Speed Selection Menu**: Added `SKOROST' (1-3)` menu at the end of the setup.
    - `1: LO` (Slow/Smooth)
    - `2: MID` (Standard)
    - `3: HI` (Fast)
- **Decimal Point**: Mapped key `B` to enter a decimal point (`.`) for precise metric inputs (e.g., `0.2` mm).

### Changed
- **Motor Synchronization**: Fixed logic so both the Main Motor and Traverse Motor (Carriage) respect the selected speed delay, ensuring consistent torque and smoothness.
- **UI Consistency**: Updated "Length" and "Diameter" input screens to match the "Wire Diameter" screen style (2-line layout with " MM" suffix).
- **Code Comments**: Translated all source code comments in `Main.ino`, `lcd.cpp`, and `keypad.cpp` to Russian.

### Fixed
- **Winding Type Bug**: Removed stray character printing during the "Angled/Normal" selection.
- **Compilation Errors**: Resolved redefinition of `wiremm` and scope issues in input loops.

## [1.6.0] - 2026-02-12
### Changed
- **Input Logic**: Completely removed "Core Type (1-3)" and "AWG" presets.
- **Metric Inputs**: Switching to direct input of:
    - Coil Length (mm)
    - Core Diameter (mm)
    - Wire Diameter (mm)

## [1.5.0] - 2026-02-12
### Added
- **Emergency Stop**: Pressing `F` during winding now immediately stops the machine (`STOP!` on screen).

### Changed
- **Text**: Translated all English prompts to Russian (Transliteration) for better usability (e.g., `DOMOI`, `REZHIM`, `VITKI`).
- **Memory Optimization**: Wrapped all static strings in `F()` macro to save RAM.

## [1.4.0] - 2026-02-12
### Changed
- **Keypad Mapping**: Remapped 4x4 keypad to:
    - `E` = Enter
    - `F` = Backspace/Stop
    - `A` = Mode A (Inductance)
    - `D` = Mode D (Turns)
- **Homing**: Increased homing speed (250us delay).
- **Motor Noise**: Tuned stepper delays (40-50us) to reduce resonance and noise.
