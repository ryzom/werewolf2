FILE(GLOB SRC *.cpp *.h)

FILE(GLOB ENTITY_DFN data\dfn\entity_def\*.dfn)
FILE(GLOB ENTITY_PROP_DFN data\dfn\entity_def\properties\*.dfn)
FILE(GLOB GEORGES_TYPES data\basics\*.typ)
FILE(GLOB ENTITIES data\*.entity_def)

ADD_EXECUTABLE(entitysample		${SRC})

SOURCE_GROUP(Data\\Basic_Types ${GEORGES_TYPES})
SOURCE_GROUP(Data\\Entity_Definitions ${ENTITY_DFN})
SOURCE_GROUP(Data\\Entity_Property_Definitions ${ENTITY_PROP_DFN})
SOURCE_GROUP(Data ${ENTITIES})

INCLUDE_DIRECTORIES(${LIBXML2_INCLUDE_DIR} ${NEL_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/wwscript/include)
TARGET_LINK_LIBRARIES(entitysample	${PLATFORM_LINKFLAGS} 
								${LIBXML2_LIBRARIES} 
								optimized ${NELMISC_LIBRARY}	debug ${NELMISC_LIBRARY_DEBUG}  
								optimized ${NELGEORGES_LIBRARY}	debug ${NELGEORGES_LIBRARY_DEBUG} 
								optimized ${NELPACS_LIBRARY}	debug ${NELPACS_LIBRARY_DEBUG} 
								wwcommon
								wwscript)

IF(WIN32)
  SET_TARGET_PROPERTIES(entitysample PROPERTIES 
	LINK_FLAGS "/SUBSYSTEM:CONSOLE"
	LINK_FLAGS_DEBUG "/NODEFAULTLIB:libc;libcmt;libcmtd;msvcrt"
	LINK_FLAGS_RELEASE "/NODEFAULTLIB:libc;libcmt;libcmtd;msvcrtd"
	PROJECT_LABEL "Samples: Entity Sample")
ENDIF(WIN32)

ADD_DEFINITIONS(${LIBXML2_DEFINITIONS})

INSTALL(TARGETS entitysample RUNTIME DESTINATION bin COMPONENT samples)
