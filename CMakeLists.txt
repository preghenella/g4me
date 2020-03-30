### @author: Roberto Preghenella
### @email: preghenella@bo.infn.it

set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

project(g4me)
cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

#cmake_policy(SET CMP0074 NEW)
cmake_policy(SET CMP0065 NEW)

### c++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

### check whether install prefix is default
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/install CACHE PATH "default install path" FORCE)
endif(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

### find Geant4 package
find_package(Geant4 REQUIRED ui_all vis_all)
include(${Geant4_USE_FILE})

### find ROOT package
find_package(ROOT REQUIRED COMPONENTS RIO)
include(${ROOT_USE_FILE})

### find Pythia8 package
find_package(Pythia REQUIRED)
include_directories(${Pythia_INCLUDE_DIR})

#find_package(HepMC REQUIRED)
#include(${HEPMC3_USE_FILE})

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

### add subdirectories
add_subdirectory(src)
add_subdirectory(share)
