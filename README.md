# Dialysis Calc

A Pebble smartwatch application designed to help dialysis patients and healthcare professionals calculate, track, and monitor fluid removal metrics during hemodialysis treatment sessions.

## Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Supported Devices](#supported-devices)
- [Installation](#installation)
- [Usage Guide](#usage-guide)
- [Technical Specifications](#technical-specifications)
- [Project Structure](#project-structure)
- [Building from Source](#building-from-source)
- [Data Models](#data-models)
- [Architecture](#architecture)
- [Contributing](#contributing)
- [License](#license)

---

## Overview

Dialysis Calc is a specialized medical utility application for Pebble smartwatches that assists in the management of hemodialysis treatments. Hemodialysis is a life-sustaining treatment for patients with kidney failure, where excess fluid and waste products are removed from the blood. Accurate tracking of fluid removal is critical for patient safety and treatment efficacy.

This application provides:

- **Pre-treatment calculations**: Determine target fluid removal goals based on current weight and prescribed dry weight
- **Real-time metrics**: Calculate ultrafiltration rate (UFR), optimistic/pessimistic removal ranges, and goal targets
- **Post-treatment analysis**: Track actual fluid removal, calculate variance from goals, and determine goal achievement percentage
- **Historical tracking**: Store up to 15 treatment records for trend analysis
- **Session recovery**: Automatically resume interrupted treatment sessions

### Why a Smartwatch App?

Dialysis treatments typically last 3-4 hours, during which patients remain relatively stationary. A smartwatch app provides:

1. **Convenient access** to treatment data without requiring a phone or tablet
2. **Quick reference** to metrics during treatment
3. **Discrete monitoring** in clinical settings
4. **Always-on-wrist availability** for patients and caregivers

---

## Features

### Core Functionality

| Feature | Description |
|---------|-------------|
| **Weight Input** | Enter pre-treatment weight with 0.1 kg precision (range: 30-200 kg) |
| **Dry Weight Target** | Set prescribed dry weight goal for the treatment |
| **Treatment Duration** | Configure session length from 1-8 hours in 15-minute increments |
| **Delta Tolerance** | Select acceptable weight variance threshold (0.2 or 0.4 kg) |
| **Goal Calculation** | Automatically compute target fluid removal (K = pre-weight - dry-weight) |
| **UFR Calculation** | Calculate ultrafiltration rate in kg/hour |
| **Range Estimation** | Determine optimistic and pessimistic removal estimates |
| **Post-Treatment Entry** | Record final weight after treatment completion |
| **Variance Analysis** | Calculate difference between actual and target removal |
| **Achievement Percentage** | Display goal completion as a percentage |
| **History Storage** | Circular buffer storing up to 15 completed treatments |
| **Session Recovery** | Resume in-progress treatments after app restart |

### User Interface Features

- **Dual input modes**: Navigate between fields or edit values with the same buttons
- **Button repeating**: Hold buttons to rapidly increment/decrement values
- **Long-press navigation**: Hold SELECT to advance between treatment phases
- **Haptic feedback**: Vibration feedback for mode changes and confirmations
- **Color-coded variance**: Green for over-goal, red for under-goal (on color displays)
- **Platform-adaptive layouts**: Optimized UI for round and rectangular displays

---

## Supported Devices

Dialysis Calc supports all Pebble smartwatch platforms:

| Platform | Watch Models | Display |
|----------|--------------|---------|
| **Aplite** | Pebble Original, Pebble Steel | 144×168 monochrome e-ink |
| **Basalt** | Pebble Time, Pebble Time Steel | 144×168 64-color LCD |
| **Chalk** | Pebble Time Round | 180×180 circular 64-color LCD |

The application automatically adapts its layout and color scheme based on the target platform.

---

## Installation

### From Pre-built Binary

1. Locate the compiled `pebble-app.pbw` file in the `build/` directory
2. Transfer to your phone via email, cloud storage, or direct transfer
3. Open the `.pbw` file with the Pebble app on your phone
4. Follow the prompts to install on your connected Pebble watch

### From Pebble App Store

*Note: As of 2024, the Pebble App Store is no longer operational. Use the pre-built binary method or build from source.*

### Using Pebble Tool (Development)

```bash
# Ensure your watch is connected via the Pebble mobile app
./pebble.sh install --phone <your-phone-ip>
```

---

## Usage Guide

### Workflow Overview

```
┌─────────────────────────────────────────────────────────────────┐
│                        TREATMENT WORKFLOW                        │
├─────────────────────────────────────────────────────────────────┤
│                                                                  │
│  1. PRE-TREATMENT PHASE                                         │
│     ├── Enter pre-treatment weight                              │
│     ├── Set dry weight target                                   │
│     ├── Configure treatment duration                            │
│     ├── Select delta tolerance                                  │
│     └── Review calculated metrics (K, UFR, ranges)              │
│              │                                                   │
│              ▼ [Long-press SELECT]                              │
│  2. POST-TREATMENT PHASE                                        │
│     ├── Enter post-treatment weight                             │
│     ├── Review actual removal                                   │
│     ├── View variance from goal                                 │
│     └── Check achievement percentage                            │
│              │                                                   │
│              ▼ [Press SELECT]                                   │
│  3. COMPLETION                                                  │
│     ├── Treatment saved to history                              │
│     └── Ready for next treatment                                │
│                                                                  │
└─────────────────────────────────────────────────────────────────┘
```

### Button Controls

#### Pre-Treatment Window

| Button | Navigation Mode | Editing Mode |
|--------|----------------|--------------|
| **UP** | Move to previous field | Increase field value |
| **DOWN** | Move to next field | Decrease field value |
| **SELECT** | Enter editing mode | Exit editing mode |
| **SELECT (long)** | Proceed to post-treatment | Proceed to post-treatment |
| **BACK** | Exit application | Exit editing mode |

#### Post-Treatment Window

| Button | Action |
|--------|--------|
| **UP** | Increase post-weight value |
| **DOWN** | Decrease post-weight value |
| **SELECT** | Save treatment and return to pre-treatment |
| **BACK** | Return to pre-treatment (discard post data) |

### Input Fields Explained

#### Pre-Weight
The patient's weight immediately before starting dialysis treatment, measured in kilograms. This is typically taken on a calibrated clinical scale.

- **Range**: 30.0 - 200.0 kg
- **Increment**: 0.1 kg
- **Precision**: 1 decimal place

#### Dry Weight
The target weight prescribed by the patient's nephrologist. This represents the patient's ideal weight with no excess fluid retention. Achieving dry weight indicates successful fluid removal.

- **Range**: 30.0 - 200.0 kg
- **Increment**: 0.1 kg
- **Precision**: 1 decimal place

#### Treatment Time
The scheduled duration of the dialysis session. Standard treatments typically range from 3-4 hours, though this can vary based on patient needs and prescription.

- **Range**: 1 hour - 8 hours
- **Increment**: 15 minutes
- **Display format**: H:MM (e.g., "3:30" for 3 hours 30 minutes)

#### Delta Selection
The acceptable tolerance range for fluid removal. This accounts for measurement variability and provides optimistic/pessimistic estimates.

- **Options**: 0.2 kg or 0.4 kg
- **Toggle**: SELECT button cycles between options

### Calculated Metrics Explained

#### K (Goal Removal)
The target amount of fluid to be removed during treatment.

```
K = Pre-weight - Dry weight
```

**Example**: Pre-weight 75.3 kg, Dry weight 72.0 kg → K = 3.3 kg

#### Optimistic Estimate
The minimum expected fluid removal (best-case scenario).

```
Optimistic = K - Delta
```

**Example**: K = 3.3 kg, Delta = 0.2 kg → Optimistic = 3.1 kg

#### Pessimistic Estimate
The maximum expected fluid removal (worst-case scenario).

```
Pessimistic = K + Delta
```

**Example**: K = 3.3 kg, Delta = 0.2 kg → Pessimistic = 3.5 kg

#### UFR (Ultrafiltration Rate)
The rate of fluid removal per hour, a critical safety metric. Excessively high UFR can cause complications such as hypotension or cramping.

```
UFR = K / Treatment Time (in hours)
```

**Example**: K = 3.3 kg, Time = 3.5 hours → UFR = 0.94 kg/hr

*Clinical note: UFR > 13 mL/kg/hr is generally considered high risk.*

#### Actual Removal
The measured amount of fluid removed after treatment completion.

```
Actual = Pre-weight - Post-weight
```

#### Variance
The difference between actual removal and the goal. Positive variance indicates over-achievement; negative indicates under-achievement.

```
Variance = Actual - K
```

**Display**: Shown with +/- prefix and color-coded on color displays (green for positive, red for negative)

#### Achievement Percentage
The percentage of the goal that was achieved.

```
Achievement = (Actual / K) × 100
```

---

## Technical Specifications

### Performance Characteristics

| Metric | Value |
|--------|-------|
| Memory footprint | < 10 KB RAM |
| Persistent storage | ~1 KB (15 records × ~64 bytes) |
| Battery impact | Minimal (standard watchapp) |
| Startup time | < 500 ms |
| Calculation overhead | Negligible (integer arithmetic only) |

### Numerical Precision

The application uses fixed-point arithmetic to maintain precision without floating-point overhead:

| Value Type | Internal Storage | Precision | Example |
|------------|------------------|-----------|---------|
| Weight | int32_t × 10 | 0.1 kg | 753 = 75.3 kg |
| UFR | int32_t × 100 | 0.01 kg/hr | 94 = 0.94 kg/hr |
| Percentage | int32_t × 10 | 0.1% | 1005 = 100.5% |
| Time | int16_t | 1 minute | 210 = 3h 30m |

### Storage Schema

The application uses Pebble's persistent storage API with the following key structure:

| Key | Purpose | Size |
|-----|---------|------|
| `0x0001` | In-progress treatment record | ~32 bytes |
| `0x0002` | History entry count | 4 bytes |
| `0x0100` - `0x010E` | History circular buffer (15 slots) | ~32 bytes each |

---

## Project Structure

```
pebble-app/
├── src/
│   └── c/                              # C source code
│       ├── main.c                      # Application entry point
│       │                               # - App initialization
│       │                               # - Window stack management
│       │                               # - Session recovery logic
│       │
│       ├── windows/                    # UI window implementations
│       │   ├── pre_treatment_window.c  # Pre-treatment input screen
│       │   ├── pre_treatment_window.h  # - Weight/time input handling
│       │   │                           # - Metric calculations display
│       │   │                           # - Navigation mode management
│       │   │
│       │   ├── post_treatment_window.c # Post-treatment results screen
│       │   └── post_treatment_window.h # - Post-weight entry
│       │                               # - Variance/percentage display
│       │                               # - Treatment completion
│       │
│       ├── data/                       # Data layer
│       │   ├── treatment_data.c        # Treatment record structures
│       │   ├── treatment_data.h        # - Metric calculations
│       │   │                           # - Data type definitions
│       │   │
│       │   ├── storage.c               # Persistent storage layer
│       │   └── storage.h               # - In-progress treatment
│       │                               # - History circular buffer
│       │                               # - Pebble persist API wrapper
│       │
│       └── ui/                         # UI utilities
│           ├── number_format.c         # Number formatting helpers
│           └── number_format.h         # - Weight string formatting
│                                       # - Time string formatting
│                                       # - Percentage formatting
│
├── resources/                          # Media resources (icons, fonts)
│
├── build/                              # Build output directory
│   ├── pebble-app.pbw                  # Compiled application bundle
│   ├── aplite/                         # Aplite platform build
│   ├── basalt/                         # Basalt platform build
│   └── chalk/                          # Chalk platform build
│
├── package.json                        # Pebble project configuration
├── wscript                             # WAF build script
├── pebble.sh                           # SDK wrapper script
└── README.md                           # This file
```

### File Descriptions

#### `src/c/main.c` (47 lines)
Application entry point implementing the Pebble app lifecycle:
- `init()`: Initializes storage, checks for in-progress treatments, pushes initial window
- `deinit()`: Cleans up window resources
- `main()`: Standard Pebble app main function with event loop

#### `src/c/windows/pre_treatment_window.c` (346 lines)
The primary input interface for pre-treatment data entry:
- Four input fields (pre-weight, dry-weight, time, delta)
- Dual-mode input system (navigation vs. editing)
- Real-time metric calculation and display
- Button handlers with repeat functionality
- Long-press detection for window transitions

#### `src/c/windows/post_treatment_window.c` (240 lines)
Post-treatment results and completion interface:
- Post-weight input with increment/decrement
- Calculated results display (actual removal, variance, percentage)
- Color-coded variance indication
- Treatment completion and history storage

#### `src/c/data/treatment_data.c` (69 lines)
Core data structures and calculation logic:
- `TreatmentRecord` structure definition
- `CalculatedMetrics` structure definition
- `treatment_calculate_metrics()`: Computes all derived values

#### `src/c/data/storage.c` (102 lines)
Persistent storage abstraction layer:
- In-progress treatment save/load/clear
- History circular buffer management
- Automatic overflow handling (overwrites oldest entry)

#### `src/c/ui/number_format.c` (56 lines)
String formatting utilities for display:
- `format_weight()`: Converts int32 to "XX.X" format
- `format_time()`: Converts minutes to "H:MM" format
- `format_percentage()`: Converts int32 to "XXX.X%" format

---

## Building from Source

### Prerequisites

1. **Pebble SDK 4.x** installed and configured
2. **Python 2.7** (required by Pebble SDK)
3. **ARM toolchain** (arm-none-eabi-gcc)

### SDK Installation

```bash
# Install Pebble SDK (if not already installed)
pip install pebble-sdk

# Install SDK core (if not already installed)
pebble sdk install latest
```

### Build Commands

```bash
# Navigate to project directory
cd /path/to/pebble-app

# Build for all platforms
./pebble.sh build

# Build for specific platform
./pebble.sh build --target aplite
./pebble.sh build --target basalt
./pebble.sh build --target chalk
```

### Build Output

After successful build, the following files are generated:

```
build/
├── pebble-app.pbw              # Installable app bundle (all platforms)
├── aplite/
│   └── pebble-app.elf          # Aplite binary
├── basalt/
│   └── pebble-app.elf          # Basalt binary
└── chalk/
    └── pebble-app.elf          # Chalk binary
```

### Installation to Watch

```bash
# Install via phone (phone IP required)
./pebble.sh install --phone <phone-ip-address>

# Install to emulator
./pebble.sh install --emulator aplite
./pebble.sh install --emulator basalt
./pebble.sh install --emulator chalk
```

### Running the Emulator

```bash
# Launch emulator with app
./pebble.sh build && ./pebble.sh install --emulator basalt

# View logs
./pebble.sh logs
```

---

## Data Models

### TreatmentRecord

The primary data structure representing a single dialysis treatment session.

```c
typedef struct {
    int32_t pre_weight;      // Pre-treatment weight (× 10)
    int32_t dry_weight;      // Target dry weight (× 10)
    int32_t post_weight;     // Post-treatment weight (× 10)
    int16_t treatment_time;  // Duration in minutes
    int16_t delta_selection; // 0 = 0.2 kg, 1 = 0.4 kg
    time_t  timestamp;       // Unix timestamp of session start
    bool    is_complete;     // Completion flag
} TreatmentRecord;
```

**Field Details:**

| Field | Type | Description | Example Value |
|-------|------|-------------|---------------|
| `pre_weight` | int32_t | Weight before treatment × 10 | 753 (75.3 kg) |
| `dry_weight` | int32_t | Target weight × 10 | 720 (72.0 kg) |
| `post_weight` | int32_t | Weight after treatment × 10 | 721 (72.1 kg) |
| `treatment_time` | int16_t | Duration in minutes | 210 (3h 30m) |
| `delta_selection` | int16_t | Tolerance index (0 or 1) | 0 (0.2 kg) |
| `timestamp` | time_t | Start time | 1706745600 |
| `is_complete` | bool | Completion status | true |

### CalculatedMetrics

Derived values computed from a TreatmentRecord.

```c
typedef struct {
    int32_t k_goal;          // Goal removal (× 10)
    int32_t optimistic;      // Minimum estimate (× 10)
    int32_t pessimistic;     // Maximum estimate (× 10)
    int32_t ufr;             // Ultrafiltration rate (× 100)
    int32_t actual_removal;  // Actual removed (× 10)
    int32_t variance;        // Difference from goal (× 10)
    int32_t percentage;      // Achievement % (× 10)
} CalculatedMetrics;
```

**Calculation Formulas:**

```c
k_goal      = pre_weight - dry_weight
optimistic  = k_goal - delta
pessimistic = k_goal + delta
ufr         = (k_goal * 600) / treatment_time  // Result × 100
actual      = pre_weight - post_weight
variance    = actual - k_goal
percentage  = (actual * 1000) / k_goal  // Result × 10
```

---

## Architecture

### Layer Diagram

```
┌─────────────────────────────────────────────────────────────────┐
│                         APPLICATION                              │
├─────────────────────────────────────────────────────────────────┤
│                                                                  │
│  ┌─────────────────────────────────────────────────────────┐    │
│  │                     UI LAYER                             │    │
│  │  ┌─────────────────────┐  ┌─────────────────────────┐   │    │
│  │  │ Pre-Treatment       │  │ Post-Treatment          │   │    │
│  │  │ Window              │  │ Window                  │   │    │
│  │  │                     │  │                         │   │    │
│  │  │ • Weight inputs     │  │ • Post-weight input     │   │    │
│  │  │ • Time input        │  │ • Results display       │   │    │
│  │  │ • Delta toggle      │  │ • Completion action     │   │    │
│  │  │ • Metrics display   │  │                         │   │    │
│  │  └─────────────────────┘  └─────────────────────────┘   │    │
│  │                     │              │                     │    │
│  │                     └──────┬───────┘                     │    │
│  │                            │                             │    │
│  │  ┌─────────────────────────▼─────────────────────────┐  │    │
│  │  │              UI UTILITIES                          │  │    │
│  │  │  • Number formatting (weight, time, percentage)    │  │    │
│  │  └───────────────────────────────────────────────────┘  │    │
│  └─────────────────────────────────────────────────────────┘    │
│                              │                                   │
│                              ▼                                   │
│  ┌─────────────────────────────────────────────────────────┐    │
│  │                   BUSINESS LOGIC LAYER                   │    │
│  │  ┌───────────────────────────────────────────────────┐  │    │
│  │  │              Treatment Data                        │  │    │
│  │  │  • TreatmentRecord structure                       │  │    │
│  │  │  • CalculatedMetrics structure                     │  │    │
│  │  │  • Metric calculation algorithms                   │  │    │
│  │  └───────────────────────────────────────────────────┘  │    │
│  └─────────────────────────────────────────────────────────┘    │
│                              │                                   │
│                              ▼                                   │
│  ┌─────────────────────────────────────────────────────────┐    │
│  │                    DATA LAYER                            │    │
│  │  ┌───────────────────────────────────────────────────┐  │    │
│  │  │              Storage                               │  │    │
│  │  │  • In-progress treatment (singleton)               │  │    │
│  │  │  • History circular buffer (15 entries)            │  │    │
│  │  │  • Pebble persist API wrapper                      │  │    │
│  │  └───────────────────────────────────────────────────┘  │    │
│  └─────────────────────────────────────────────────────────┘    │
│                              │                                   │
│                              ▼                                   │
│  ┌─────────────────────────────────────────────────────────┐    │
│  │                  PEBBLE SDK / PLATFORM                   │    │
│  │  • Window stack management                               │    │
│  │  • Button event handling                                 │    │
│  │  • Persistent storage API                                │    │
│  │  • Graphics and text rendering                           │    │
│  │  • Timer and vibration services                          │    │
│  └─────────────────────────────────────────────────────────┘    │
│                                                                  │
└─────────────────────────────────────────────────────────────────┘
```

### Window Navigation Flow

```
                    ┌──────────────┐
                    │   App Init   │
                    └──────┬───────┘
                           │
              ┌────────────▼────────────┐
              │  Check for in-progress  │
              │      treatment          │
              └────────────┬────────────┘
                           │
           ┌───────────────┴───────────────┐
           │                               │
     [None found]                   [Found]
           │                               │
           ▼                               ▼
┌──────────────────────┐      ┌──────────────────────┐
│  Pre-Treatment       │      │  Restore from        │
│  Window (fresh)      │      │  saved state         │
└──────────┬───────────┘      └──────────┬───────────┘
           │                             │
           └──────────────┬──────────────┘
                          │
                          ▼
           ┌──────────────────────────────┐
           │     Pre-Treatment Window     │◄─────┐
           │                              │      │
           │  • Edit pre-weight           │      │
           │  • Edit dry weight           │      │
           │  • Edit treatment time       │      │
           │  • Toggle delta              │      │
           │  • View calculated metrics   │      │
           └──────────────┬───────────────┘      │
                          │                      │
              [Long-press SELECT]                │
                          │                      │
                          ▼                      │
           ┌──────────────────────────────┐      │
           │    Post-Treatment Window     │      │
           │                              │      │
           │  • Edit post-weight          │      │
           │  • View actual removal       │      │
           │  • View variance             │      │
           │  • View achievement %        │      │
           └──────────────┬───────────────┘      │
                          │                      │
                  [Press SELECT]                 │
                          │                      │
                          ▼                      │
           ┌──────────────────────────────┐      │
           │    Save to History           │      │
           │    Clear in-progress         │      │
           │    Reset for next treatment  │──────┘
           └──────────────────────────────┘
```

---

## Contributing

### Development Setup

1. Clone the repository:
   ```bash
   git clone <repository-url>
   cd pebble-app
   ```

2. Ensure Pebble SDK is installed and configured

3. Build and test:
   ```bash
   ./pebble.sh build
   ./pebble.sh install --emulator basalt
   ```

### Code Style Guidelines

- Use 4-space indentation
- Follow existing naming conventions:
  - `snake_case` for functions and variables
  - `PascalCase` for type definitions
  - `UPPER_CASE` for constants and macros
- Keep functions focused and under 50 lines where possible
- Document public APIs with comments
- Use fixed-point arithmetic for all numerical calculations

### Testing

Currently, testing is performed manually via the Pebble emulator:

```bash
# Test on all platforms
./pebble.sh build
./pebble.sh install --emulator aplite
./pebble.sh install --emulator basalt
./pebble.sh install --emulator chalk
```

### Submitting Changes

1. Create a feature branch from `main`
2. Make your changes with descriptive commits
3. Test on all three platforms (aplite, basalt, chalk)
4. Submit a pull request with a clear description

---

## License

*License information not specified in the original project. Please add appropriate license file.*

---

## Acknowledgments

- Pebble SDK and community for the development platform
- Healthcare professionals who provided domain expertise for dialysis calculations

---

## Version History

| Version | Date | Changes |
|---------|------|---------|
| 1.0.0 | Initial | Initial release with core functionality |

---

## Disclaimer

This application is intended as a convenience tool for tracking dialysis treatment metrics. It is **not a medical device** and should not be used as a substitute for professional medical judgment. Always follow your healthcare provider's instructions for dialysis treatment management.
