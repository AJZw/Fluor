cmake_minimum_required(VERSION 3.13)

# Set project details
project(fluor VERSION 0.9 DESCRIPTION "Fluor executable")

# C++ version
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# CMAKE settings
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Set executable
add_executable(fluor
    widgets/src/main.cpp
    widgets/header/application.h
    widgets/src/application.cpp

    widgets/header/main_controller.h
    widgets/src/main_controller.cpp

    widgets/header/central_controller.h
    widgets/src/central_controller.cpp

    widgets/header/toolbar_controller.h
    widgets/src/toolbar_controller.cpp
    widgets/header/toolbar_buttons.h
    widgets/src/toolbar_buttons.cpp

    widgets/header/fluor_controller.h
    widgets/src/fluor_controller.cpp
    widgets/header/fluor_lineedit.h
    widgets/src/fluor_lineedit.cpp
    widgets/header/fluor_buttons.h
    widgets/src/fluor_buttons.cpp
    widgets/header/fluor_scrollarea.h
    widgets/src/fluor_scrollarea.cpp
    
    widgets/header/laser_controller.h
    widgets/src/laser_controller.cpp
    widgets/header/laser_lineedit.h
    widgets/src/laser_lineedit.cpp
    widgets/header/laser_buttons.h
    widgets/src/laser_buttons.cpp

    resources/resources.qrc
)

# set target properties
target_compile_options(fluor PRIVATE
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

set_target_properties(fluor PROPERTIES
    # OUTPUT_NAME _data
    VERSION ${PROJECT_VERSION}
)

# set include directories to build private CSS interpreter
target_include_directories(fluor PRIVATE widgets/header)
target_include_directories(fluor PRIVATE lib/public)

# Add packages
find_package(Qt5Core CONFIG REQUIRED)
find_package(Qt5Gui CONFIG REQUIRED)
find_package(Qt5Widgets CONFIG REQUIRED)

# Link libraries
target_link_libraries(fluor PRIVATE Qt5::Core)
target_link_libraries(fluor PRIVATE Qt5::Gui)
target_link_libraries(fluor PRIVATE Qt5::Widgets)
target_link_libraries(fluor PRIVATE ${PROJECT_SOURCE_DIR}/lib/build/lib_data.dll.a)

# Copy necessary files
add_custom_command(
    TARGET fluor POST_BUILD 
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${PROJECT_SOURCE_DIR}/lib/build/lib_data.dll              
    $<TARGET_FILE_DIR:fluor>
)
add_custom_command(
    TARGET fluor POST_BUILD 
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${PROJECT_SOURCE_DIR}/data              
    $<TARGET_FILE_DIR:fluor>/data
)


########################################################
# cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug 
# or RelWithDebInfo
# CMAKE_SH:FILEPATH=CMAKE_SH-NOTFOUND
# mingw32-make -C build