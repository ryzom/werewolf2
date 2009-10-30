FILE(GLOB SRC *.cpp)
FILE(GLOB PRIV_H *.h)
FILE(GLOB PUB_H ../include/wwcommon/*.h)

SOURCE_GROUP("Events" FILES					CGameEventServer.cpp
											../include/wwcommon/CGameEventServer.h)

SOURCE_GROUP("Events\\Sob Events" FILES		CSobAddEvent.cpp
											../include/wwcommon/CSobAddEvent.h											
											CSobRemoveEvent.cpp
											../include/wwcommon/CSobRemoveEvent.h
											CSobSpawnEvent.cpp
											../include/wwcommon/CSobSpawnEvent.h
											CSobStateRequestEvent.cpp
											../include/wwcommon/CSobStateRequestEvent.h											
											CSobUnspawnEvent.cpp
											../include/wwcommon/CSobUnspawnEvent.h
											ISobEvent.cpp
											../include/wwcommon/ISobEvent.h
											ISobStateChangeEvent.cpp
											../include/wwcommon/ISobStateChangeEvent.h)

SOURCE_GROUP("Events\\Sob Events\\Motion"  FILES CSobDREvent.cpp
											../include/wwcommon/CSobDREvent.h
											CSobMoveEvent.cpp
											../include/wwcommon/CSobMoveEvent.h
											CSobOrientEvent.cpp
											../include/wwcommon/CSobOrientEvent.h
											CSobStrafeEvent.cpp
											../include/wwcommon/CSobStrafeEvent.h)
											
SOURCE_GROUP("Events\\Game Events" FILES	CGameEventServer.cpp
											../include/wwcommon/CGameEventServer.h
											CGameSpawnRequestEvent.cpp
											../include/wwcommon/CGameSpawnRequestEvent.h
											CGameUnspawnRequestEvent.cpp
											../include/wwcommon/CGameUnspawnRequestEvent.h
											IGameEvent.cpp
											../include/wwcommon/IGameEvent.h)

IF(NOT WIN32)
  ADD_LIBRARY(wwcommon SHARED ${SRC})
ELSE(NOT WIN32)
  ADD_LIBRARY(wwcommon STATIC ${SRC} ${PUB_H})
ENDIF(NOT WIN32)

INCLUDE_DIRECTORIES(${LIBXML2_INCLUDE_DIR} ${NEL_INCLUDE_DIR})
TARGET_LINK_LIBRARIES(wwcommon ${NELMISC_LIBRARY})
SET_TARGET_PROPERTIES(wwcommon PROPERTIES VERSION ${NL_VERSION})

IF(WIN32)
  #SET_TARGET_PROPERTIES(wwcommon PROPERTIES COMPILE_FLAGS "/Yustdpch.h")
  #SET_SOURCE_FILES_PROPERTIES(stdpch.cpp PROPERTIES COMPILE_FLAGS "/Ycstdpch.h")
  SET_TARGET_PROPERTIES(wwcommon PROPERTIES 
	LINK_FLAGS "/NODEFAULTLIB:libcmt"
	DEBUG_POSTFIX "_d"
	RELEASE_POSTFIX "_r"
	PROJECT_LABEL "Library: WW Common")
ENDIF(WIN32)

ADD_DEFINITIONS(${LIBXML2_DEFINITIONS})

INSTALL(TARGETS wwcommon LIBRARY DESTINATION lib ARCHIVE DESTINATION lib COMPONENT libraries)
