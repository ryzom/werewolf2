#-----------------------------------------------------------------------------
#
# Werewolf
#    Author: Matt Raykowski
#
# Notes:
#   * Changing install location: add -DCMAKE_INSTALL_PREFIX:PATH=/my/new/path
#   * Changing NeL prefix: add -DNEL_PREFIX_PATH:PATH=/my/nel/install
#
#-----------------------------------------------------------------------------

#-----------------------------------------------------------------------------
# Load some macros.
SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMakeModules;${CMAKE_MODULE_PATH}")
INCLUDE(CMakeModules/nel.cmake)
INCLUDE(CMakeModules/ConfigureChecks.cmake)

#-----------------------------------------------------------------------------
# Force out of source builds.
CHECK_OUT_OF_SOURCE()

IF(WIN32)
  SET(CMAKE_INCLUDE_PATH "${CMAKE_SOURCE_DIR}/../3rdParty/include;${CMAKE_INCLUDE_PATH}")
  SET(CMAKE_LIBRARY_PATH "${CMAKE_SOURCE_DIR}/../3rdParty/lib;${CMAKE_LIBRARY_PATH}")
ENDIF(WIN32)

IF(DEFINED NEL_PREFIX_PATH)
  SET(CMAKE_INCLUDE_PATH "${NEL_PREFIX_PATH}/include;${CMAKE_INCLUDE_PATH}")
  SET(CMAKE_LIBRARY_PATH "${NEL_PREFIX_PATH}/lib;${CMAKE_LIBRARY_PATH}")
ELSE(DEFINED NEL_PREFIX_PATH)
  SET(NEL_PREFIX_PATH "" CACHE FILEPATH "NeL Installation Location")
ENDIF(DEFINED NEL_PREFIX_PATH)

CMAKE_MINIMUM_REQUIRED(VERSION 2.4)

PROJECT(Werewolf CXX C)
SET(NL_VERSION_MAJOR 0)
SET(NL_VERSION_MINOR 1)
SET(NL_VERSION_PATCH 0)
SET(NL_VERSION "${NL_VERSION_MAJOR}.${NL_VERSION_MINOR}.${NL_VERSION_PATCH}")

#-----------------------------------------------------------------------------
# Redirect output files
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH  ${CMAKE_BINARY_DIR}/lib)

#-----------------------------------------------------------------------------
# Set CMake 2.6 Policies.
IF(COMMAND cmake_policy)
  # Works around warnings libraries linked against that don't
  # have absolute paths (e.g. -lpthreads)
  cmake_policy(SET CMP0003 NEW)

  # Works around warnings about escaped quotes in ADD_DEFINITIONS 
  # statements
  cmake_policy(SET CMP0005 OLD)
ENDIF(COMMAND cmake_policy)

#-----------------------------------------------------------------------------
# Set default config options
#
NL_SETUP_DEFAULT_OPTIONS()
NL_SETUP_PREFIX_PATHS()

#-----------------------------------------------------------------------------
# Override default options
#SET(WITH_COVERAGE       ON )
SET(WITH_3D	ON )

#-----------------------------------------------------------------------------
#Platform specifics

FIND_PACKAGE(Threads REQUIRED)
FIND_PACKAGE(LibXml2 REQUIRED)
FIND_PACKAGE(PythonLibs)

set(Boost_USE_STATIC_LIBS   ON)
set(Boost_USE_MULTITHREADED ON)
FIND_PACKAGE(Boost 1.37.0 COMPONENTS thread filesystem python signals REQUIRED)

NL_SETUP_BUILD()

NL_CONFIGURE_CHECKS()

FIND_PACKAGE(NeLMISC)
FIND_PACKAGE(NeLNET)
FIND_PACKAGE(NeLPACS)

###
# An example of finding NeL 3D and CEGUI Renderer.
###
IF(WITH_3D)
  FIND_PACKAGE(FreeType)
  FIND_PACKAGE(NeL3D)
  FIND_PACKAGE(Jpeg)
  
#  IF(WITH_DRIVER_OPENGL)
#    FIND_PACKAGE(OpenGL)
#    FIND_PACKAGE(NeLDriverGL)
#  ENDIF(WITH_DRIVER_OPENGL)

  IF(WITH_CEGUI)
    FIND_PACKAGE(CEGUI)
    FIND_PACKAGE(NeLCEGUIRenderer)
  ENDIF(WITH_CEGUI)

  SET(JPEG_LIB jpeg)
ENDIF(WITH_3D)

###
# An example of finding sound and a driver.
###
IF(WITH_SOUND)
  FIND_PACKAGE(NeLSOUND)
  FIND_PACKAGE(NeLSNDDRV)
#  IF(WITH_DRIVER_OPENAL)
#    FIND_PACKAGE(OpenAL)
#    FIND_PACKAGE(NeLDriverAL)
#  ENDIF(WITH_DRIVER_OPENAL)
ENDIF(WITH_SOUND)

IF(WIN32)
  INCLUDE(${CMAKE_ROOT}/Modules/Platform/Windows-cl.cmake)
  SET(WINSOCK2_LIB ws2_32.lib)
ENDIF(WIN32)

NL_SETUP_BUILD_FLAGS()

#IF(UNIX)
#  ADD_DEFINITIONS(-DDATA_DIR="\\"${NL_SHARE_PREFIX}/yourAppName/\\"")
#ENDIF(UNIX)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/wwcommon/include)

ADD_SUBDIRECTORY(wwcommon)
ADD_SUBDIRECTORY(client)

#INSTALL(DIRECTORY data/
#        DESTINATION ${NL_SHARE_PREFIX}/yourAppName/data
#        COMPONENT data
#        PATTERN "CVS" EXCLUDE
#        PATTERN ".svn" EXCLUDE
#        PATTERN "Makefile*" EXCLUDE)

# packaging information
#SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "NeL MMORPG Framework - Hello World Demo")
#SET(CPACK_PACKAGE_VENDOR "NeL")
#SET(CPACK_PACKAGE_DESCRIPTION_FILE ${CMAKE_SOURCE_DIR}/README)
#SET(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_SOURCE_DIR}/COPYING)
#SET(CPACK_PACKAGE_VERSION_MAJOR "${NL_VERSION_MAJOR}")
#SET(CPACK_PACKAGE_VERSION_MINOR "${NL_VERSION_MINOR}")
#SET(CPACK_PACKAGE_VERSION_PATCH "${NL_VERSION_PATCH}")
#SET(CPACK_INSTALL_CMAKE_PROJECTS "${CMAKE_BINARY_DIR};HelloWorld;ALL;/")
#SET(CPACK_PACKAGE_EXECUTABLES "helloworld-${NL_VERSION}" "helloworld")
#
### Source Packages
#SET(CPACK_PACKAGE_FILE_NAME "helloworld-${NL_VERSION}")
#SET(CPACK_SOURCE_PACKAGE_FILE_NAME "helloworld-${NL_VERSION}")
#IF(WIN32)
#  SET(CPACK_GENERATOR "NSIS")
#  SET(CPACK_SOURCE_GENERATOR "ZIP")
#ELSE(WIN32)
#  SET(CPACK_GENERATOR "TGZ")
#  SET(CPACK_SOURCE_GENERATOR "TGZ")
#ENDIF(WIN32)
#set(CPACK_SOURCE_IGNORE_FILES
#        "~$"
#        "\\\\.cvsignore$"
#        "^${CMAKE_SOURCE_DIR}.*/CVS/"
#        "^${CMAKE_SOURCE_DIR}.*/\\\\.svn/"
#        "^${CMAKE_SOURCE_DIR}/debian/"
#        "^${CMAKE_SOURCE_DIR}/old/")
#INCLUDE(${CMAKE_ROOT}/Modules/CPack.cmake)

