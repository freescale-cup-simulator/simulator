cmake_minimum_required(VERSION 2.8.7 FATAL_ERROR)
project(freescale_simulator CXX C)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};/usr/lib/OGRE/cmake/")
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(project_sources

    main.cc
    physics_simulation.cc
    vehicle_model.cc
    simulation_runner.cc
    control_algorithm.cc
    camera_simulator.cc
    global_renderer.cc
    logger.cc
    gui_controller.cc
    car3d.cc
    property.cc
    property_model.cc
)

set(project_headers

    physics_simulation.h
    vehicle_model.h
    simulation_runner.h
    control_algorithm.h
    common.h
    camera_simulator.h
    global_renderer.h
    logger.h
    gui_controller.h
    car3d.h
    property.h
    property_model.h
)

set(qrc
    ${CMAKE_SOURCE_DIR}/../resources/resources.qrc
)

find_package(Qt5Quick REQUIRED Qt5Quick5.2.1)
find_package(Qt5Widgets REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Quick_EXECUTABLE_COMPILE_FLAGS}")

include(ExternalProject)
ExternalProject_Add(
  ODE
  URL ${CMAKE_CURRENT_SOURCE_DIR}/deps/ode-0.12.tar.bz2
  URL_MD5 48fdd41fae1a7e7831feeded09826599
  BUILD_IN_SOURCE 1
  CONFIGURE_COMMAND ./configure --prefix=<INSTALL_DIR> --enable-static
    --disable-shared --enable-double-precision --with-trimesh=gimpact
    --disable-demos --with-drawstuff=none --enable-libccd
  BUILD_COMMAND make
  INSTALL_COMMAND make install
)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LUA lua)
if(NOT LUA_FOUND) # Debuan/Ubuntu have a different naming scheme
  pkg_check_modules(LUA REQUIRED lua5.2 )
endif()

ExternalProject_Get_Property(ODE INSTALL_DIR)

include_directories(SYSTEM
                    ${LUA_INCLUDE_DIRS}
                    ${INSTALL_DIR}/include
)

link_directories(${INSTALL_DIR}/lib/)
add_definitions(-DdDOUBLE)

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

find_package(OGRE REQUIRED)
find_package(OIS REQUIRED)
find_package(Boost COMPONENTS system thread date_time REQUIRED)

include_directories(SYSTEM
        ${Boost_INCLUDE_DIR}
        ${OGRE_INCLUDE_DIRS}
        ${OGRE_SAMPLES_INCLUDEPATH}
        ${OIS_INCLUDE_DIRS}
        ${CMAKE_BINARY_DIR}
)

add_subdirectory(libtrack EXCLUDE_FROM_ALL)
include_directories(libtrack
                    ${CMAKE_BINARY_DIR}/libtrack/)

add_subdirectory(libqmlogre EXCLUDE_FROM_ALL)
include_directories(libqmlogre
                    ${CMAKE_BINARY_DIR}/libqmlogre/)


message(STATUS "Build type is: ${CMAKE_BUILD_TYPE}")

qt5_add_resources(RC_SRCS ${qrc})

set(RESOURCE_DIR ${CMAKE_SOURCE_DIR}/../resources/)
set(LUA_SCRIPT_DIR ${CMAKE_SOURCE_DIR}/../lua/)

configure_file(config.h.template ${CMAKE_BINARY_DIR}/config.h)

add_executable(freescale_simulator

               ${project_sources}
               ${project_headers}
               ${RC_SRCS}
)


set_target_properties(freescale_simulator PROPERTIES AUTOMOC TRUE)
target_link_libraries(freescale_simulator
                      track
                      qmlogre
                      ode
                      ${LUA_LIBRARIES}
                      ${OGRE_LIBRARIES}
                      ${OIS_LIBRARIES}
                      ${Boost_LIBRARIES}
                      ${Qt5Quick_LIBRARIES}
                      ${Qt5Widgets_LIBRARIES}
)
