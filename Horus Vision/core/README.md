# Horus Vision Engine Core

## Overview

The core engine module (`hrs_engine.cpp`, `hrs_engine.h`) and the Horus Vision initialization (`hrs_horus.cpp`) are central to the Horus Vision project, orchestrating the overall operation, initialization, and management of the rendering engine.

## Features

- **Engine Initialization and Configuration**: Sets up the rendering environment, including window creation, scene setup, and renderer configuration.
- **Main Loop Management**: Handles the main application loop, processing input, updating state, and rendering frames.
- **Extensible Framework**: Designed for easy extension with additional features and integrations.

## Usage

Integrate the engine core into your project to leverage the Horus Vision rendering capabilities:

1. **Engine Initialization**:
   - Utilize `hrs_engine.h` to initialize the engine with necessary configurations.

2. **Starting the Engine**:
   - `hrs_horus.cpp` demonstrates starting the engine and entering the main application loop.

## Example

```cpp
#include "hrs_engine.h"

int main() {
    HorusEngine engine;
    engine.initialize(1280, 720, "Horus Vision Project");
    
    while(engine.isRunning()) {
        engine.processInput();
        engine.update();
        engine.render();
    }
    
    return 0;
}
