# Setup
1) Install git, cmake, vulkan, python 3.
2) Clone repo.
3) init git submodules (git submodule update --init)
4) use cmake to generate the solution.
5) If you are not in windows or have your python 3 in another place then change then change the path to python 3 exe in cmakeLists.txt 
6) in compile_shaders.py change the path to glslc.exe based on your vk version and where it is installed

# Structure
- app/ the scene, windows and things not directly related to the graphics like the timer
- assets/ The meshes, textures, etc. but not the shaders
- components/ implementations and specializations of components from vk. Game objects
- data_structures/ Contains data structures for the project
- shaders/ the shaders
- utils/ miscellaneous utilities
- vk/ vulkan-related objects  

# Defines
- HASH_IS_NUMERIC : If present the hashes, used as keys in many tables, will be numeric and calculated using std::hash<std::string> if absent they'll be just unhashed std::strings. It exists to make debugging easier.
- MAX_FRAMES_IN_FLIGHT=number : Controls how many frames we render in parallel. While one frame is shown the other is being drawn. Must be > 0. Controls the size of the ring buffers.