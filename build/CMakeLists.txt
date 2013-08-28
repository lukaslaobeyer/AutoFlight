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

# Tell cmake where Qt is
set(CMAKE_PREFIX_PATH "/usr/local/Qt-5.1.2")

# Include libraries in the ../lib folder
include_directories("/home/user/AutoFlight/lib/include/gamepad")
include_directories("/home/user/AutoFlight/lib/include/")

# Find the QtWidgets library
add_definitions(-DQT_NO_KEYWORDS)
find_package(Qt5Widgets)
find_package(Qt5WebKitWidgets)

# Find the Boost libraries
set(Python_ADDITIONAL_VERSIONS 3)
find_package(Boost 1.53 COMPONENTS python-py33 system filesystem thread chrono REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

# Find OpenCV
find_package(OpenCV REQUIRED)

# Find FFMPEG and dependencies
find_package(FFMPEG REQUIRED)
include_directories(${FFMPEG_INCLUDE_DIR})
find_package(ZLIB REQUIRED)

# Find Python
find_package(PythonLibs REQUIRED)
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
    "../src/qcustomplot/qcustomplot.h"
    "../src/qcustomplot/qcustomplot.cpp"
)

# Add the resources
qt5_add_resources(AutoFlight_RCCS "../src/resources.qrc")

# Create the AutoFlight executable
add_executable(AutoFlight ${AutoFlight_SRC} ${AutoFlight_RCCS})

target_link_libraries(AutoFlight ${Boost_LIBRARIES})
target_link_libraries(AutoFlight ${OpenCV_LIBS})
target_link_libraries(AutoFlight ${FFMPEG_LIBRARIES})
target_link_libraries(AutoFlight ${ZLIB_LIBRARIES})
target_link_libraries(AutoFlight ${PYTHON_LIBRARIES})
target_link_libraries(AutoFlight x264 va faac mp3lame fdk-aac swscale)

# Link libraries in ../lib
target_link_libraries(AutoFlight "/home/user/AutoFlight/lib/release-linux64/libstem_gamepad.a")
target_link_libraries(AutoFlight "/home/user/AutoFlight/lib/release-linux64/libqscintilla2.a")

# Use the Widgets module from Qt 5
qt5_use_modules(AutoFlight Widgets PrintSupport WebKit WebKitWidgets)
