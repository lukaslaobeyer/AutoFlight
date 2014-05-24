# May be required to run CMake with  -DPYTHON_EXECUTABLE=/usr/bin/python3 -DPYTHON_LIBRARY=/usr/lib/x86_64-linux-gnu/libpython3.4m.so -DPYTHON_INCLUDE_DIR=/usr/include/python3.4 -DPYTHON_INCLUDE_DIR2=/usr/include/python3.4


cmake_minimum_required(VERSION 2.8.9)

project(AutoFlight)

# C++11 and warnings
#set(CMAKE_CXX_FLAGS "-Wall -std=c++11 -O2 -s")
set(CMAKE_CXX_FLAGS "-Wall -std=c++11  -g -O0")
#                                      ^^^^^^ for debugging only
# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Run moc automatically when needed
set(CMAKE_AUTOMOC ON)
# Set executable output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../release_linux)
# Add current directory to CMAKE_MODULE_PATH
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ".")

set(QT_USE_QTWEBKIT TRUE)

add_definitions(-DBOOST_ASIO_ENABLE_HANDLER_TRACKING=TRUE)

# Tell cmake where Qt is
#set(CMAKE_PREFIX_PATH "/home/lukas/workspace/libs/qt5/qtbase")
set(CMAKE_PREFIX_PATH "/usr/local/Qt-5.3.1/")

# Include libraries in the ../lib folder
include_directories("../lib/include/gamepad")
include_directories("../lib/include/")

# Find the QtWidgets library
add_definitions(-DQT_NO_KEYWORDS)
find_package(Qt5Widgets)
find_package(Qt5WebKitWidgets)

# Find the Boost libraries
set(Python_ADDITIONAL_VERSIONS 3)
find_package(Boost 1.53 COMPONENTS program_options python-py34 system filesystem thread chrono date_time REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

# Find OpenCV
find_package(OpenCV REQUIRED)

# Find FFMPEG and dependencies
find_package(FFMPEG REQUIRED)
include_directories(${FFMPEG_INCLUDE_DIR})
find_package(ZLIB REQUIRED)

# Find Python
find_package(PythonLibs 3 REQUIRED)
include_directories(${PYTHON_INCLUDE_DIRS})

file(GLOB AutoFlight_SRC
    "../src/*.h"
    "../src/*.cpp"
    "../src/widgets/*.h"
    "../src/widgets/*.cpp"
    "../src/dialogs/*.h"
    "../src/dialogs/*.cpp"
    "../src/ardrone/*.h"
    "../src/ardrone/*.cpp"
    "../src/ardrone/atcommands/*.h"
    "../src/ardrone/atcommands/*.cpp"
    "../src/ardrone/navdata/*.h"
    "../src/ardrone/navdata/*.cpp"
    "../src/ardrone/video/*.h"
    "../src/ardrone/video/*.cpp"
    "../src/autoscript/*.h"
    "../src/autoscript/*.cpp"
    "../src/autoscript/opencv/*.h"
    "../src/autoscript/opencv/*.cpp"
    "../src/autoscript/opencv/*.hpp"
    "../src/tools/*.h"
    "../src/tools/*.cpp"
    "../src/imgprocui/*.h"
    "../src/imgprocui/*.cpp"
    "../src/imgproc/*.h"
    "../src/imgproc/*.cpp"
    "../src/imgprocui/nodeseditor/*.h"
    "../src/imgprocui/nodeseditor/*.cpp"
    
    "../lib/include/qcustomplot/*.h"
    "../lib/include/qcustomplot/*.cpp"
    "../lib/include/pugixml/*.hpp"
    "../lib/include/pugixml/*.cpp"
    "../lib/include/glm/*.hpp"
    "../lib/include/glm/core/*.hpp"
)

# Add the resources
qt5_add_resources(AutoFlight_RCCS "../src/resources.qrc")

# Create the AutoFlight executable
add_executable(AutoFlight ${AutoFlight_SRC} ${AutoFlight_RCCS})

# Update BUILD_NUMBER in afconstants.h header
add_custom_target(
  generate_build_number
  COMMAND python3 update_build_number.py
)
add_dependencies(AutoFlight generate_build_number)

target_link_libraries(AutoFlight ${Boost_LIBRARIES})
target_link_libraries(AutoFlight ${OpenCV_LIBS})
target_link_libraries(AutoFlight ${FFMPEG_LIBRARIES})
target_link_libraries(AutoFlight ${ZLIB_LIBRARIES})
target_link_libraries(AutoFlight ${PYTHON_LIBRARIES})
target_link_libraries(AutoFlight GL x264 swscale)

# Link libraries in ../lib
target_link_libraries(AutoFlight "/home/lukas/workspace/AutoFlight/lib/release-linux64/libstem_gamepad.a")
target_link_libraries(AutoFlight "/home/lukas/workspace/AutoFlight/lib/release-linux64/libqscintilla2.a")

# Use the Widgets module from Qt 5
qt5_use_modules(AutoFlight Widgets PrintSupport WebKit WebKitWidgets)
