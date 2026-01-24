# N-Body Simulation

## built with C++ and OpenGL

A real-time 3D gravitational N-body simulation built with C++ and OpenGL. Watch celestial bodies orbit, interact, and
collide in a fully interactive 3D environment with realistic physics.

### Controls

- **WASD**: Move camera
- **Mouse**: Look around (when in camera mode)
- **Space**: Move camera up
- **Left Ctrl**: Move camera down
- **TAB**: Toggle between camera and menu mode
- **P**: Pause/unpause simulation
- **ESC**: Exit application

## Build Instructions

### Prerequisites

- **C++ Compiler**: C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- **CMake**: Version 3.20 or higher
- **OpenGL**: Version 4.6 or higher
- **Git**: For cloning the repository

### Dependencies

All dependencies are automatically fetched by CMake during configuration:

- **GLFW** (3.4) - Window and input handling
- **GLM** (1.0.1) - Mathematics library
- **ImGui** (1.91.5) - GUI library
- **GLAD** - OpenGL loader (included in repository)

### Building on Linux/macOS

```bash
# Clone the repository
git clone https://github.com/yourusername/n_body_simulation_GL.git
cd n_body_simulation_GL

# Create build directory
mkdir build
cd build

# Configure (downloads dependencies automatically)
cmake ..

# Build
cmake --build .

# Run the simulation
./n_body_simulation_GL
```

### Building on Windows

#### Using Visual Studio

```bash
# Clone the repository
git clone https://github.com/yourusername/n_body_simulation_GL.git
cd n_body_simulation_GL

# Create build directory
mkdir build
cd build

# Generate Visual Studio solution (downloads dependencies automatically)
cmake ..

# Build from command line
cmake --build . --config Release

# Or open n_body_simulation_GL.sln in Visual Studio and build from there

# Run the simulation
Release\n_body_simulation_GL.exe
```

#### Using MinGW

```bash
# Clone the repository
git clone https://github.com/yourusername/n_body_simulation_GL.git
cd n_body_simulation_GL

# Create build directory
mkdir build
cd build

# Configure with MinGW (downloads dependencies automatically)
cmake -G "MinGW Makefiles" ..

# Build
cmake --build .

# Run the simulation
n_body_simulation_GL.exe
```

