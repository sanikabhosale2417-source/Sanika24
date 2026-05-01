cmake_minimum_required(VERSION 3.14)
project(HybridInventoryManager LANGUAGES C CXX)

set(CMAKE_C_STANDARD   11)
set(CMAKE_CXX_STANDARD 17)

# Compiler warnings
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -pedantic)
endif()

# Include directory
include_directories(include)

# C backend (compiled as C)
add_library(inventory_c STATIC src/inventory.c)
set_target_properties(inventory_c PROPERTIES LINKER_LANGUAGE C)

# Final executable (C++ sources linked with the C library)
add_executable(inventory_manager
    src/InventoryManager.cpp
    src/main.cpp
)
target_link_libraries(inventory_manager PRIVATE inventory_c)
