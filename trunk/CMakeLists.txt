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
  IF(NOT DEFINED ENV{QTDIR})
    SET(ENV{QTDIR} "c:/qt/4.5.0")
  ENDIF(NOT DEFINED ENV{QTDIR})
ENDIF(WIN32)

IF(DEFINED NEL_PREFIX_PATH)
  SET(CMAKE_INCLUDE_PATH "${NEL_PREFIX_PATH}/include;${CMAKE_INCLUDE_PATH}")
  SET(CMAKE_LIBRARY_PATH "${NEL_PREFIX_PATH}/lib;${CMAKE_LIBRARY_PATH}")
ELSE(DEFINED NEL_PREFIX_PATH)
  SET(NEL_PREFIX_PATH "" CACHE FILEPATH "NeL Installation Location")
ENDIF(DEFINED NEL_PREFIX_PATH)

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

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

#-----------------------------------------------------------------------------
#Platform specifics

FIND_PACKAGE(Threads REQUIRED)
FIND_PACKAGE(LibXml2 REQUIRED)

set(Boost_USE_STATIC_LIBS   ON)
set(Boost_USE_MULTITHREADED ON)
FIND_PACKAGE(Boost 1.38.0 COMPONENTS thread filesystem signals REQUIRED)

NL_SETUP_BUILD()

NL_CONFIGURE_CHECKS()

FIND_PACKAGE(NeL COMPONENTS nelmisc nelgeorges nel3d nelnet nelpacs REQUIRED)
FIND_PACKAGE(NeLNS COMPONENTS naming_service login_service welcome_service nel_launcher_qt)

###
# An example of finding NeL 3D and CEGUI Renderer.
###
IF(BUILD_CLIENT)
  FIND_PACKAGE(FreeType)
  FIND_PACKAGE(Jpeg)
  FIND_PACKAGE(PNG)

  FIND_PACKAGE(CEGUI)
  
  # Needed to find dependencies for nel_launcher_qt.
  FIND_PACKAGE(Qt4 REQUIRED)

  SET(JPEG_LIB jpeg)
  
 # FIND_PACKAGE(NeLSOUND)
 # FIND_PACKAGE(NeLSNDDRV)
ENDIF(BUILD_CLIENT)

IF(WIN32)
  INCLUDE(${CMAKE_ROOT}/Modules/Platform/Windows-cl.cmake)
  SET(WINSOCK2_LIB ws2_32.lib)
ENDIF(WIN32)

NL_SETUP_BUILD_FLAGS()

SET(DATA_DIR "${NL_SHARE_PREFIX}/games/werewolf" CACHE PATH "Werewolf data installation directory")
SET(ETC_DIR "${NL_ETC_PREFIX}/werewolf" CACHE PATH "Werewolf configuration directory")
 
ADD_DEFINITIONS(-DDATA_DIR="\\"${DATA_DIR}/\\"" -DETC_DIR="\\"${ETC_DIR}/\\"")


INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/wwcommon/include)

ADD_SUBDIRECTORY(wwcommon)
ADD_SUBDIRECTORY(wwscript)
ADD_SUBDIRECTORY(samples)

IF(BUILD_CLIENT)
  ADD_SUBDIRECTORY(client)
ENDIF(BUILD_CLIENT)

IF(BUILD_SERVER)
FIND_PACKAGE(MySQL)
ADD_SUBDIRECTORY(server)
ENDIF(BUILD_SERVER)


# packaging information
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Werewolf: Laurelin")
SET(CPACK_PACKAGE_VENDOR "Ironic Entertainment")
SET(CPACK_PACKAGE_DESCRIPTION_FILE ${CMAKE_SOURCE_DIR}/README)
SET(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_SOURCE_DIR}/COPYING)
SET(CPACK_PACKAGE_VERSION_MAJOR "${NL_VERSION_MAJOR}")
SET(CPACK_PACKAGE_VERSION_MINOR "${NL_VERSION_MINOR}")
SET(CPACK_PACKAGE_VERSION_PATCH "${NL_VERSION_PATCH}")
SET(CPACK_INSTALL_CMAKE_PROJECTS "${CMAKE_BINARY_DIR};Werewolf;ALL;/")
#SET(CPACK_PACKAGE_EXECUTABLES "werewolf-${NL_VERSION}" "werewolf")

# NSIS Specific Packing Setup
SET(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "Werewolf")
SET(CPACK_NSIS_MODIFY_PATH "ON")
SET(CPACK_NSIS_MUI_ICON ${CMAKE_SOURCE_DIR}/resources/ww_icon.ico)
SET(CPACK_NSIS_MUI_UNIICON ${CMAKE_SOURCE_DIR}/resources/ww_icon.ico)
SET(CPACK_PACKAGE_ICON ${CMAKE_SOURCE_DIR}/resources\\\\werewolf_icon.bmp)
SET(CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_INSTALL_DIRECTORY} Werewolf: Laurelin")
SET(CPACK_NSIS_HELP_LINK "http:\\\\\\\\www.opennel.org\\\\display\\\\confluence\\\\WW\\\\Home")
SET(CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\www.opennel.org\\\\display\\\\confluence\\\\WW\\\\Home")
SET(CPACK_NSIS_CONTACT "matt.raykowski@gmail.com")  
 
## Source Packages
SET(CPACK_PACKAGE_FILE_NAME "werewolf-${NL_VERSION}")
SET(CPACK_SOURCE_PACKAGE_FILE_NAME "werewolf-${NL_VERSION}")
IF(WIN32)
  SET(CPACK_GENERATOR "NSIS")
  #SET(CPACK_GENERATOR "NSIS;ZIP")
  #SET(CPACK_SOURCE_GENERATOR "ZIP")
ELSE(WIN32)
  SET(CPACK_GENERATOR "TGZ")
  SET(CPACK_SOURCE_GENERATOR "TGZ")
ENDIF(WIN32)
set(CPACK_SOURCE_IGNORE_FILES
	"~$"
	"\\\\.cvsignore$"
	"^${CMAKE_SOURCE_DIR}.*/CVS/"
	"^${CMAKE_SOURCE_DIR}.*/\\\\.svn/"
	"^${CMAKE_SOURCE_DIR}/debian/"
	"^${CMAKE_SOURCE_DIR}/old/")
IF(WIN32)
  IF(NOT CMAKE_BUILD_TYPE STREQUAL "Release")
    # Install "Debug" specific stuff here.
    SET(CMAKE_INSTALL_DEBUG_LIBRARIES TRUE)
    message(status "install debug libraries.")
  ELSE(NOT CMAKE_BUILD_TYPE STREQUAL "Release")
    # Install "Release" Specific stuff here.
    SET(CMAKE_INSTALL_DEBUG_LIBRARIES TRUE)
  ENDIF(NOT CMAKE_BUILD_TYPE STREQUAL "Release")
  
  
  IF(BUILD_CLIENT)
    # Install CEGUI and its dependencies.
    INSTALL(FILES "${CEGUI_LIB_DIR}/CEGUIBase.dll" DESTINATION bin)
    INSTALL(FILES "${CEGUI_LIB_DIR}/CEGUIDevilImageCodec.dll" DESTINATION bin)
    INSTALL(FILES "${CEGUI_LIB_DIR}/CEGUIExpatParser.dll" DESTINATION bin)
    INSTALL(FILES "${CEGUI_LIB_DIR}/CEGUIFalagardWRBase.dll" DESTINATION bin)
    INSTALL(FILES "${CEGUI_LIB_DIR}/Devil.dll" DESTINATION bin)
    INSTALL(FILES "${CEGUI_LIB_DIR}/ILU.dll" DESTINATION bin)
    
    # Install Qt dependencies for the launcher.
    INCLUDE(${QT_USE_FILE})
    INSTALL(FILES 
        "${QT_LIBRARY_DIR}/QtGuid4.dll"        
        "${QT_LIBRARY_DIR}/QtXmld4.dll"
        "${QT_LIBRARY_DIR}/QtCored4.dll"
        DESTINATION bin)
        
     # Install the Qt Launcher.
     INSTALL(FILES ${NELNS_NEL_LAUNCHER_BIN} DESTINATION bin COMPONENT client)
  ENDIF(BUILD_CLIENT)
  
  IF(BUILD_SERVER)
    INSTALL(FILES 
		${NELNS_NAMING_SERVICE_BIN} 
		${NELNS_NAMING_SERVICE_CFG} 
		${NELNS_LOGIN_SERVICE_BIN} 
		${NELNS_LOGIN_SERVICE_CFG}
		${NELNS_WELCOME_SERVICE_BIN} 
		${NELNS_WELCOME_SERVICE_CFG}
		DESTINATION sbin 
		COMPONENT server)
  ENDIF(BUILD_SERVER)
    
  # Only the tools require MFC.
  IF(WITH_TOOLS)
    SET(CMAKE_INSTALL_MFC_LIBRARIES TRUE)
  ENDIF(WITH_TOOLS)
  INCLUDE(InstallRequiredSystemLibraries)
ENDIF(WIN32)

INCLUDE(CPack)

INCLUDE(CMakePackaging.txt)

## Debian Packages
#INCLUDE(UseDebian)
#IF(DEBIAN_FOUND)
#  ADD_DEBIAN_TARGETS(nel)
#ENDIF(DEBIAN_FOUND)
