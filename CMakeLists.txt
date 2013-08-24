cmake_minimum_required(VERSION 2.8.11 FATAL_ERROR)

project(freescale_simulator CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(project_sources

    main.cc
    physics_simulation.cc
    vehicle_model.cc
    simulation_runner.cc
    control_algorithm.cc
)

set(project_headers

    physics_simulation.h
    vehicle_model.h
    simulation_runner.h
    control_algorithm.h
    common.h
)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING
  "Choose the type of build: Debug/Release" FORCE)
endif()

if(CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS_DEBUG
      "${CMAKE_CXX_FLAGS} -std=c++11 -O2 -ggdb3 -Wall -Wextra -pedantic")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -std=c++11 -O2")
endif()

find_package(Qt5Core REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LUA REQUIRED lua)

find_package(Bullet REQUIRED)
include_directories(SYSTEM ${BULLET_INCLUDE_DIRS})

add_subdirectory(libtrack EXCLUDE_FROM_ALL)
include_directories(libtrack)

message(STATUS "Build type is: ${CMAKE_BUILD_TYPE}")


qt5_wrap_cpp(moc_output ${project_headers})

set(RESOURCE_DIR ${CMAKE_SOURCE_DIR}/resources/)
set(LUA_SCRIPT_DIR ${CMAKE_SOURCE_DIR}/lua/)

configure_file(config.h.template ${CMAKE_BINARY_DIR}/config.h)

add_executable(freescale_simulator

               ${project_sources}
               ${project_headers}
               ${moc_output}
)

target_link_libraries(freescale_simulator

                      track
                      Qt5::Core
                      ${LUA_LIBRARIES}
                      ${BULLET_LIBRARIES}
                      BulletWorldImporter
)
