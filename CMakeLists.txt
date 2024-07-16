cmake_minimum_required(VERSION 3.10)
project(tinymembench C ASM)

# Set the compiler for Windows
if(WIN32)
    set(CMAKE_C_COMPILER "gcc")
endif()

# Set the C standard to C99
set(CMAKE_C_STANDARD 99)

# Set optimization flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2")

# Source files
set(SOURCES
    main.c
    util.c
    asm-opt.c
)

# Assembly files
set(ASM_SOURCES
    x86-sse2.S
    arm-neon.S
    aarch64-asm.S
    mips-32.S
)

# Add executable
add_executable(tinymembench ${SOURCES} ${ASM_SOURCES})

# Set the linker language
set_target_properties(tinymembench PROPERTIES LINKER_LANGUAGE C)

# Link math library
target_link_libraries(tinymembench m)

# Include directories
target_include_directories(tinymembench PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
