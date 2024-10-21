cmake_minimum_required(VERSION 3.10)
project(geronimo_vk VERSION 0.1)
# Enable C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# Set policy CMP0077
if(POLICY CMP0077)
    cmake_policy(SET CMP0077 NEW)
endif()
if(POLICY CMP0079)
    cmake_policy(SET CMP0079 NEW) #sorcery to allow my fetched libpng to link with my fetched zlib
endif()
# Include FetchContent module
include(FetchContent)
# Find Vulkan
find_package(Vulkan REQUIRED)
# Fetch GLFW
FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG        3.3.10 # Specify the desired version
)
FetchContent_MakeAvailable(glfw)
# Fetch GLM
FetchContent_Declare(
  glm
  GIT_REPOSITORY https://github.com/g-truc/glm.git
  GIT_TAG        1.0.0 # Specify the desired version
)
FetchContent_MakeAvailable(glm)
# Fetch Assimp
# Disable all exporters
set(ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)
# Disable all importers by default
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)
# Enable FBX importer
set(ASSIMP_BUILD_FBX_IMPORTER ON CACHE BOOL "" FORCE)
# Enable glTF importer
set(ASSIMP_BUILD_GLTF_IMPORTER ON CACHE BOOL "" FORCE)
# Optionally, disable features you don't need
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
  assimp
  GIT_REPOSITORY https://github.com/assimp/assimp.git
  GIT_TAG v5.4.2 # or the version you need
)
FetchContent_MakeAvailable(assimp)
# Link zlib to Assimp
set(ASSIMP_USE_EXTERNAL_ZLIB ON)
set(ASSIMP_BUILD_ZLIB OFF)
set(ASSIMP_BUILD_TESTS OFF)
target_link_libraries(assimp  zlibstatic)
####get the files####
file(GLOB app_files "app/*.h" "app/*.cpp")
file(GLOB data_structures_files "data_structures/*.h" "data_structures/*.cpp")
file(GLOB shader_files "shaders/*.*")
file(GLOB scripts_files "*.py")
file(GLOB vk_files "vk/*.h" "vk/*.cpp")
file(GLOB utils_files "utils/*.h" "utils/*.cpp")
file(GLOB components_files "components/*.h" "components/*.cpp")
file(GLOB io_files "io/*.h" "io/*.cpp")

#####Group them in a nice organization#####
source_group("app" FILES ${app_files})
source_group("vk" FILES ${vk_files})
source_group("data structures" FILES ${data_structures_files})
source_group("shader" FILES ${shader_files})
source_group("scripts" FILES ${scripts_files})
source_group("utils" FILES ${utils_files})
source_group("components" FILES ${components_files})
source_group("io" FILES ${io_files})
#####Create the target#####
add_executable(geronimo_vk_demo
    ${app_files}
    ${vk_files}
    ${data_structures_files}
    ${shader_files}
    ${scripts_files}
    ${utils_files}
    ${components_files}
    ${io_files}
)
target_include_directories(geronimo_vk_demo PRIVATE .)
# Include GLM headers
target_include_directories(geronimo_vk_demo PRIVATE ${glm_SOURCE_DIR})

target_link_libraries(geronimo_vk_demo PRIVATE 
    Vulkan::Vulkan 
    glfw
    assimp)

target_compile_definitions(geronimo_vk_demo PRIVATE 
    VK_DEBUG_LEVEL=VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT # See VkDebugUtilsMessageSeverityFlagBitsEXT @vulkan_core.h
    MAX_FRAMES_IN_FLIGHT=1
    GLM_FORCE_RADIANS #GLM must use radians everywhere
    GLM_FORCE_DEFAULT_ALIGNED_GENTYPES #Force glm vector and matrix types to be aligned
    GLM_FORCE_DEPTH_ZERO_TO_ONE
    SCREEN_WIDTH=1024
    SCREEN_HEIGH=768
    GLM_ENABLE_EXPERIMENTAL
    GLM_FORCE_LEFT_HANDED 
)
target_compile_features(geronimo_vk_demo PRIVATE cxx_std_17)
add_custom_command(
    TARGET geronimo_vk_demo
    POST_BUILD
    COMMAND "C:/Windows/py.exe" "${CMAKE_SOURCE_DIR}/copy_assets.py" "${CMAKE_SOURCE_DIR}" 
)
add_custom_command(
    TARGET geronimo_vk_demo
    POST_BUILD
    COMMAND "C:/Windows/py.exe" "${CMAKE_SOURCE_DIR}/compile_shaders.py" "${CMAKE_SOURCE_DIR}" 
)