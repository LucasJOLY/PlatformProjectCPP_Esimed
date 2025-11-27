# compile the std lib as a module (for LLVM & MSVC only)

# extract the compiler directory
get_filename_component(COMPILER_DIR ${CMAKE_CXX_COMPILER} DIRECTORY)
if(MSVC)
    set(STD_BASE_DIR ${COMPILER_DIR}/../../../modules)
    set(STD_EXTENSION ixx)
else()
    # Try to find std.cppm in common locations
    set(STD_BASE_DIR ${COMPILER_DIR}/../share/libc++/v1)
    get_filename_component(STD_BASE_DIR "${STD_BASE_DIR}" ABSOLUTE)
    
    # Check if std.cppm exists at the calculated path
    if(NOT EXISTS "${STD_BASE_DIR}/std.cppm")
        # Try alternative paths for Homebrew on macOS
        if(APPLE)
            # Try /opt/homebrew/opt/llvm/share/libc++/v1
            if(EXISTS "/opt/homebrew/opt/llvm/share/libc++/v1/std.cppm")
                set(STD_BASE_DIR "/opt/homebrew/opt/llvm/share/libc++/v1")
            # Try to find in Cellar (version-specific path)
            else()
                file(GLOB LLVM_CELLAR_DIRS "/opt/homebrew/Cellar/llvm/*/share/libc++/v1")
                if(LLVM_CELLAR_DIRS)
                    list(GET LLVM_CELLAR_DIRS 0 STD_BASE_DIR)
                endif()
            endif()
        endif()
    endif()
    
    # Verify the file exists
    if(NOT EXISTS "${STD_BASE_DIR}/std.cppm")
        message(FATAL_ERROR "Cannot find std.cppm. Searched in: ${STD_BASE_DIR}")
    endif()
    
    set(STD_EXTENSION cppm)
endif()

# compile the std library
add_library(std-cxx-modules STATIC)
target_sources(std-cxx-modules
        PUBLIC
        FILE_SET moduleStd
        TYPE CXX_MODULES
        BASE_DIRS ${STD_BASE_DIR}
        FILES
        ${STD_BASE_DIR}/std.${STD_EXTENSION}
        ${STD_BASE_DIR}/std.compat.${STD_EXTENSION})

if(MSVC)
    target_compile_options(std-cxx-modules PRIVATE
            /std:c++latest
            /experimental:module
            /EHsc
            /nologo
            /W4
            /c
            /fp:fast
    )
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options(std-cxx-modules PRIVATE
                /Od
                /D_DEBUG
                /MDd
        )
    else()
        target_compile_options(std-cxx-modules PRIVATE
                /O2
                /MD
        )
    endif()
else()
    target_compile_options(std-cxx-modules
            PRIVATE
            -Wno-reserved-module-identifier
            -Wno-reserved-user-defined-literal
    )
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options(std-cxx-modules PRIVATE
                -O0
        )
    else()
        target_compile_options(std-cxx-modules PRIVATE
                -O3
        )
    endif()
endif()