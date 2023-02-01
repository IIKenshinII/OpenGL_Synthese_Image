cmake_minimum_required(VERSION 3.8)

project(RollerCoaster)

# Set the folder where the executables are created
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE})

# Include glimac
add_subdirectory(glimac)
add_subdirectory(assimp)

# Create a target for each TP
function(setup_tp TP_NUMBER)
    set(TARGET_NAME ${TP_NUMBER}_exe)
    add_executable(${TARGET_NAME})
    target_compile_features(${TARGET_NAME} PRIVATE cxx_std_11)

    # Enable many good warnings
    if (MSVC)
        target_compile_options(${TARGET_NAME} PRIVATE /WX /W3)
    else()
        target_compile_options(${TARGET_NAME} PRIVATE  -W -Wall -Wextra -Wpedantic -pedantic-errors)
    endif()
    
    # Add all the source and header files
    target_include_directories(${TARGET_NAME} PRIVATE ${TP_NUMBER})
    file(GLOB_RECURSE MY_SOURCES CONFIGURE_DEPENDS ${TP_NUMBER}/*)
    target_sources(${TARGET_NAME} PRIVATE ${MY_SOURCES})

    # Add glimac as a dependency
    target_link_libraries(${TARGET_NAME} glimac)
    target_link_libraries(${TARGET_NAME} assimp)

    # Copy the assets and the shaders to the output folder (where the executable is created)
    include("CMakeUtils/files_and_folders.cmake")
    Cool__target_copy_folder(${TARGET_NAME} ${TP_NUMBER}/assets)
    Cool__target_copy_folder(${TARGET_NAME} ${TP_NUMBER}/shaders)
    Cool__target_copy_folder(${TARGET_NAME} ${TP_NUMBER}/textures)
endfunction(setup_tp)

setup_tp(Rollercoaster)
