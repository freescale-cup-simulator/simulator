cmake_minimum_required(VERSION 2.8.7 FATAL_ERROR)

project(freescale_simulator CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(project_sources

    main.cc
    physics_simulation.cc
    vehicle_model.cc
    simulation_runner.cc
    control_algorithm.cc
    camera_simulator.cc
)

set(project_headers

    physics_simulation.h
    vehicle_model.h
    simulation_runner.h
    control_algorithm.h
    common.h
    camera_simulator.h
)

find_package(Qt5Gui REQUIRED)
include_directories(${Qt5Gui_INCLUDE_DIRS})
add_definitions(${Qt5Gui_DEFINITIONS})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Gui_EXECUTABLE_COMPILE_FLAGS}")

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_SOURCE_DIR}/cmake/")
find_package(Irrlicht REQUIRED)
include_directories(${IRRLICHT_INCLUDE_DIRS})

find_package(PkgConfig REQUIRED)
pkg_check_modules(ODE REQUIRED ode)
pkg_check_modules(ASSIMP REQUIRED assimp)

pkg_check_modules(LUA lua)
if(NOT LUA_FOUND) # Debuan/Ubuntu have a different naming scheme
  pkg_check_modules(LUA lua5.2 REQUIRED)
endif()

include_directories(SYSTEM
                    ${ODE_INCLUDE_DIRS}
                    ${ASSIMP_INCLUDE_DIRS}
                    ${LUA_INCLUDE_DIRS}
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ODE_CFLAGS_OTHER}")

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING
  "Choose the type of build: Debug/Release" FORCE)
endif()

include(CheckCXXCompilerFlag)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  CHECK_CXX_COMPILER_FLAG("-std=c++11" STDCPP11_SUPPORTED)
  if(STDCPP11_SUPPORTED)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
  else()
    CHECK_CXX_COMPILER_FLAG("-std=c++0x" STDCPP0X_SUPPORTED)
    if(STDCPP0X_SUPPORTED)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
    else()
      message(FATAL_ERROR "Compiler doesn't appear to support C++11 or at least C++0x")
    endif()
  endif()
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -O0 -ggdb3 -Wall -Wextra -pedantic")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O3")
else()
  message(FATAL_ERROR "Unsupported compiler")
endif()

add_subdirectory(libtrack EXCLUDE_FROM_ALL)
include_directories(libtrack
                    ${CMAKE_BINARY_DIR}/libtrack/)

message(STATUS "Build type is: ${CMAKE_BUILD_TYPE}")

set(RESOURCE_DIR ${CMAKE_SOURCE_DIR}/resources/)
set(LUA_SCRIPT_DIR ${CMAKE_SOURCE_DIR}/lua/)

configure_file(config.h.template ${CMAKE_BINARY_DIR}/config.h)

add_executable(freescale_simulator

               ${project_sources}
               ${project_headers}
)

set_target_properties(freescale_simulator PROPERTIES AUTOMOC TRUE)
target_link_libraries(freescale_simulator

                      track
                      ${LUA_LIBRARIES}
                      ${ODE_LIBRARIES}
                      ${IRRLICHT_LIBRARIES}
                      ${ASSIMP_LIBRARIES}
                      ${Qt5Gui_LIBRARIES}
)
