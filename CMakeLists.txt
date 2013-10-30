cmake_minimum_required(VERSION 2.8.9 FATAL_ERROR)

project(freescale_simulator CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(project_sources

    main.cc
    physics_simulation.cc
    vehicle_model.cc
    simulation_runner.cc
    control_algorithm.cc
    camera_simulator.cc
    global_renderer.cc
)

set(project_headers

    physics_simulation.h
    vehicle_model.h
    simulation_runner.h
    control_algorithm.h
    common.h
    camera_simulator.h
    global_renderer.h
)

set(qrc
    resources/resources.qrc
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

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/")

find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LUA REQUIRED lua)
pkg_check_modules(ODE REQUIRED ode)
pkg_check_modules(ASSIMP REQUIRED assimp)

include_directories(SYSTEM
                    ${ODE_INCLUDE_DIRS}
                    ${ASSIMP_INCLUDE_DIRS}
                    ${LUA_INCLUDE_DIRS}
)

find_package(OGRE REQUIRED)
find_package(OIS REQUIRED)
find_package(Boost COMPONENTS system thread date_time REQUIRED)

include_directories(
        ${Boost_INCLUDE_DIR}
        ${OGRE_INCLUDE_DIRS}
        ${OGRE_SAMPLES_INCLUDEPATH}
        ${OIS_INCLUDE_DIRS}
)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${ODE_CFLAGS_OTHER}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${ODE_CFLAGS_OTHER}")

add_subdirectory(libtrack EXCLUDE_FROM_ALL)
include_directories(libtrack
                    ${CMAKE_BINARY_DIR}/libtrack/)

add_subdirectory(libqmlogre EXCLUDE_FROM_ALL)
include_directories(libqmlogre
                    ${CMAKE_BINARY_DIR}/libqmlogre/)


message(STATUS "Build type is: ${CMAKE_BUILD_TYPE}")

#qt5_wrap_cpp(moc_output ${project_headers})
qt5_add_resources (RC_SRCS ${qrc})

set(RESOURCE_DIR ${CMAKE_SOURCE_DIR}/resources/)
set(LUA_SCRIPT_DIR ${CMAKE_SOURCE_DIR}/lua/)

configure_file(config.h.template ${CMAKE_BINARY_DIR}/config.h)

add_executable(freescale_simulator

               ${project_sources}
               ${project_headers}
               ${RC_SRCS}
)

set_target_properties(freescale_simulator PROPERTIES AUTOMOC TRUE)
qt5_use_modules(freescale_simulator Core Gui Qml Quick)
target_link_libraries(freescale_simulator

                      track
                      qmlogre
                      ${LUA_LIBRARIES}
                      ${ODE_LIBRARIES}
                      ${ASSIMP_LIBRARIES}
                      ${OGRE_LIBRARIES}
                      ${OIS_LIBRARIES}
                      ${Boost_LIBRARIES}

)
