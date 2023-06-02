# volumetric-clouds
A real-time GPU volumetric clouds renderer via OpenGL, based on the SIGGRAPH 2015 talk <a href="http://killzone.dl.playstation.net/killzone/horizonzerodawn/presentations/Siggraph15_Schneider_Real-Time_Volumetric_Cloudscapes_of_Horizon_Zero_Dawn.pdf">The Real-time Volumetric Cloudscapes of Horizon: Zero Dawn</a> by Andrew Schneider.

<a href="https://depersonalizc.github.io/portfolio/clouds.html"><img src="./resources/images/showcase.png" alt="Collage of realtime render results from the volumetric clouds renderer" width="100%"></img></a>

## How can I run this?
### Prerequisites
- **Qt 6**: In order to build and run this program, you will need to have Qt 6 installed on your machine. See https://www.qt.io/product/qt6 for more information.
- **OpenGL with GLSL 4.3 or higher**: Required for GPU rendering and compute shaders/SSBO.
### Build & Run
We have included a `CMakeList.txt` file for building the program (only tested on Linux at this point). If you already have Qt installed, you can build and run the program by following these steps:
- Launch Qt Creator and click "Open Project"
- Select and open `CMakeLists.txt` from the root directory of the project
- Go to Projects - Run Settings - Run, and change "Working directory" to the root directory of the project
- Click the "Run" button to build and start the program

## Key Features
- Organic clouds shape generation with 3D fractal Worley noise
- Dynamic clouds movement over time by offsetting noise textures
- Physically-based volume rendering with anistropic scattering to account for directional dependence of light scattering within the clouds
- Rendering compatability with solid objects by compositing with depth and color buffers
- Rich customization options to craft your own clouds: control density, coverage, shape, and lighting of the clouds in the GUI
- Realistic sky that varies throughout a day by modeling atmospheric scattering

## Optimizations
- Optimized raymarching with adaptive step sizes and backtracking
- GL Compute Shader to efficiently generate 3D Worley noise textures

