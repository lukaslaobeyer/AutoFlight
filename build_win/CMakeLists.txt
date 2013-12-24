################################
######## For WINDOWS  ##########
################################

cmake_minimum_required(VERSION 2.8.9)

project(AutoFlight)


set(CMAKE_PREFIX_PATH "C:/OpenCV/opencv; C:/FFMPEG; C:/Python33")

# C++11 and warnings
set(CMAKE_CXX_FLAGS "-Wall -std=c++11 -O2 -s")
# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Run moc automatically when needed
set(CMAKE_AUTOMOC ON)
# Set executable output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../release_windows)
# Add current directory to CMAKE_MODULE_PATH
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ".")

# Include libraries in the ../lib folder
include_directories("C:/AutoFlight/lib/include/gamepad")
include_directories("C:/AutoFlight/lib/include")

# Find the QtWidgets library
add_definitions(-DQT_NO_KEYWORDS)
find_package(Qt5Widgets)
find_package(Qt5WebKitWidgets)

# Find the Boost libraries
set(Python_ADDITIONAL_VERSIONS 3)
find_package(Boost 1.53 COMPONENTS python python3 system filesystem thread chrono date_time REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

# Needed on Windows
add_definitions(-DBOOST_THREAD_USE_LIB)
add_definitions(-DBOOST_CHRONO_INLINED)
add_definitions(-DBOOST_PYTHON_STATIC_LIB)
add_definitions(-DBOOST_ALL_NO_LIB)

# Find OpenCV
set(OpenCV_DIR "C:/OpenCV/opencv/build")
find_package(OpenCV REQUIRED)

# Find Python
find_package(PythonLibs REQUIRED)
include_directories(${PYTHON_INCLUDE_DIRS})

# Find FFMPEG and dependencies
set(FFMPEG_INCLUDE_DIR "C:/FFMPEG/include/")
set(FFMPEG_LIBRARIES
    "C:/FFMPEG/lib/libavcodec.dll.a"
    "C:/FFMPEG/lib/libavformat.dll.a"
    "C:/FFMPEG/lib/libavutil.dll.a" 
	"C:/FFMPEG/lib/libswscale.dll.a" )
include_directories(${FFMPEG_INCLUDE_DIR})

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
    "../src/tools/*.h"
    "../src/tools/*.cpp"
    "../src/imgprocui/*.h"
    "../src/imgprocui/*.cpp"
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
add_executable(AutoFlight WIN32 ${AutoFlight_SRC} ${AutoFlight_RCCS})

# Link
target_link_libraries(AutoFlight ${Boost_LIBRARIES})
target_link_libraries(AutoFlight ${OpenCV_LIBS})
target_link_libraries(AutoFlight ${FFMPEG_LIBRARIES})
target_link_libraries(AutoFlight ${PYTHON_LIBRARIES})
target_link_libraries(AutoFlight opengl32 wsock32 ws2_32)

# Link libraries in ../lib
target_link_libraries(AutoFlight "C:/AutoFlight/lib/release-win32/libstem_gamepad.a" winmm)
target_link_libraries(AutoFlight "C:/AutoFlight/lib/release-win32/libqscintilla2.a")

# Use the Widgets module from Qt 5
qt5_use_modules(AutoFlight Widgets PrintSupport WebKit WebKitWidgets)
