  cmake_minimum_required(VERSION 2.8.11 FATAL_ERROR)

  project(camera-simulator CXX)

  set(sources
    camerasimulator.cpp
    camerasimulator.h
    mainwindow.cpp
    mainwindow.h
    main.cpp
    mainwindow.ui
)

  if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING
      "Choose the type of build: Debug/Release" FORCE)
  endif()
  
  # Tell CMake to run moc when necessary:
  set(CMAKE_AUTOMOC ON)
  # As moc files are generated in the binary dir, tell CMake
  # to always look for includes there:
  set(CMAKE_INCLUDE_CURRENT_DIR ON)

  # Widgets finds its own dependencies.
  find_package(Qt5Widgets REQUIRED)
  
  include(FindIrrlicht.cmake)

  if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS_DEBUG
	"${CMAKE_CXX_FLAGS} -std=c++11 -O2 -ggdb3 -Wall -Wextra -pedantic")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -std=c++11 -O2")
  endif()

  add_subdirectory(libtrack EXCLUDE_FROM_ALL)
  
  include_directories(
    libtrack
    ${IRRLICHT_INCLUDE_DIRS}
  )
  
  message(STATUS "Build type is: ${CMAKE_BUILD_TYPE}")
  
  add_executable(camera-simulator ${sources})

  qt5_use_modules(camera-simulator Widgets)
  # Support UI files
  qt5_wrap_ui(ui_mainwindow.h mainwindow.ui)
  
  target_link_libraries(camera-simulator track ${IRRLICHT_LIBRARIES})
