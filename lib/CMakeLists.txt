cmake_minimum_required(VERSION 3.13)

# Set project details
project(data_library VERSION 0.9 DESCRIPTION "Fluor Data Dynamic Library")

# C++ version
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# CMAKE settings
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

# Add library target
add_library(data_library SHARED
    public/data_global.h

    #public/data_cytometers.h
    #data_cytometers.cpp

    public/data_factory.h
    src/data_factory.cpp
    
    public/data_spectrum.h
    src/data_spectrum.cpp
    
    public/data_fluorophores.h
    src/data_fluorophores.cpp

    public/data_styles.h
    src/data_styles.cpp
    
)

# set target properties
target_compile_options(data_library PRIVATE
    -g
    -Wall
    -Wextra
    -Wpedantic
    -Wconversion
    #-Wshadow
    -Wsign-compare
    -Wsign-conversion
    -Wsign-promo
)

set_target_properties(data_library PROPERTIES
    OUTPUT_NAME _data
    VERSION ${PROJECT_VERSION}
)

# Define DATALIB_LIBRARY to enable Q_DECL_EXPORT
add_compile_definitions(data_library PRIVATE DATALIB_LIBRARY)

# set include directories
target_include_directories(data_library PRIVATE public)
target_include_directories(data_library PRIVATE src)

# Add packages
find_package(Qt5Core CONFIG REQUIRED)
find_package(Qt5Gui CONFIG REQUIRED)
find_package(Qt5Widgets CONFIG REQUIRED)

# Link libraries
target_link_libraries(data_library PRIVATE Qt5::Core)
target_link_libraries(data_library PRIVATE Qt5::Widgets)

# target_source


########################################################
# cmake -S lib -B lib\build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
# CMAKE_SH:FILEPATH=CMAKE_SH-NOTFOUND
# mingw32-make -C .\lib\build