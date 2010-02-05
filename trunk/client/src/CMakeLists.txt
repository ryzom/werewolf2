FILE(GLOB SRC *.cpp *.h)
FILE(GLOB TASK_SRC tasks/*.cpp tasks/*.h)
FILE(GLOB CTRLR_SRC controllers/*.cpp controllers/*.h)
FILE(GLOB ENTITY_SRC entities/*.cpp entities/*.h)
FILE(GLOB IFACES_SRC interfaces/*.cpp interfaces/*.h)
FILE(GLOB LISTENERS_SRC listeners/*.cpp listeners/*.h)
FILE(GLOB SELECTOR_SRC selectors/*.cpp selectors/*.h)
FILE(GLOB VIEW_SRC views/*.cpp views/*.h)
FILE(GLOB RESOURCES_RC *.rc)

SOURCE_GROUP(Tasks FILES ${TASK_SRC})
SOURCE_GROUP(Controllers FILES ${CTRLR_SRC})
SOURCE_GROUP(Entities FILES ${ENTITY_SRC})
SOURCE_GROUP(Interfaces FILES ${IFACES_SRC})
SOURCE_GROUP(Listeners FILES ${LISTENERS_SRC})
SOURCE_GROUP(Selectors FILES ${SELECTOR_SRC})
SOURCE_GROUP(Views FILES ${VIEW_SRC})
SOURCE_GROUP(Resources FILES ${RESOURCES_RC})

ADD_EXECUTABLE(werewolf	WIN32	${SRC}
								${TASK_SRC}
								${CTRLR_SRC}
								${ENTITY_SRC}
								${IFACES_SRC}
								${LISTENERS_SRC}
								${SELECTOR_SRC}
								${VIEW_SRC}
								${RESOURCES_RC})

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

# On Unix we'll need to include config.h
IF(UNIX)
  INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR})
ENDIF(UNIX)


INCLUDE_DIRECTORIES(${LIBXML2_INCLUDE_DIR} ${NEL_INCLUDE_DIR} ${CEGUI_INCLUDE_DIRS} ${PNG_INCLUDE_DIR})
TARGET_LINK_LIBRARIES(werewolf	optimized {NELMISC_LIBRARY}		debug ${NELMISC_LIBRARY_DEBUG} 
								optimized ${NELPACS_LIBRARY}	debug ${NELPACS_LIBRARY_DEBUG}
								optimized ${NELNET_LIBRARY}		debug ${NELNET_LIBRARY_DEBUG} 
								optimized ${NELSOUND_LIBRARY}	debug ${NELSOUND_LIBRARY_DEBUG} 
								optimized ${NELSNDDRV_LIBRARY}	debug ${NELSNDDRV_LIBRARY_DEBUG} 
								optimized ${NELLIGO_LIBRARY}	debug ${NELLIGO_LIBRARY_DEBUG} 
								optimized ${NELGEORGES_LIBRARY}	debug ${NELGEORGES_LIBRARY_DEBUG} 
								optimized ${NEL3D_LIBRARY}		debug ${NEL3D_LIBRARY_DEBUG}
								optimized ${CEGUI_LIBRARY}		debug ${CEGUI_LIBRARY_DEBUG} 
								${PLATFORM_LINKFLAGS} 
								${LIBXML2_LIBRARIES} 
								${JPEG_LIBRARY} 
								${FREETYPE_LIBRARY} 
								${PNG_LIBRARIES}
								wwcommon 
								wwscript)
IF(WIN32)
  SET_TARGET_PROPERTIES(werewolf PROPERTIES 
	LINK_FLAGS_DEBUG "/NODEFAULTLIB:libc;libcmt;libcmtd;msvcrt"
	LINK_FLAGS_RELEASE "/NODEFAULTLIB:libc;libcmt;libcmtd;msvcrtd"
	PROJECT_LABEL "Werewolf Client")
ENDIF(WIN32)
ADD_DEFINITIONS(${LIBXML2_DEFINITIONS})

INSTALL(TARGETS werewolf RUNTIME DESTINATION bin COMPONENT client)
