FILE(GLOB SRC *.cpp *.h controllers/*.cpp controllers/*.h entities/*.cpp entities/*.h interfaces/*.cpp interfaces/*.h listeners/*.cpp listeners/*.h selectors/*.cpp selectors/*.h tasks/*.cpp tasks/*.h views/*.cpp views/*.h)

DECORATE_NEL_LIB("wwcommon")
SET(WWCOMMON_LIB ${LIBNAME})

LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})

ADD_EXECUTABLE(werewolf ${SRC})

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

# On Unix we'll need to include config.h
IF(UNIX)
  INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR})
ENDIF(UNIX)


INCLUDE_DIRECTORIES(${LIBXML2_INCLUDE_DIR} ${NELMISC_INCLUDE_DIRS} ${CEGUI_INCLUDE_DIRS} ${PYTHON_INCLUDE_PATH} ${PNG_INCLUDE_DIR})
TARGET_LINK_LIBRARIES(werewolf	${PLATFORM_LINKFLAGS} 
								${LIBXML2_LIBRARIES} 
								${NELMISC_LIBRARY} 
								${NELPACS_LIBRARY} 
								${NELNET_LIBRARY} 
								${NELSOUND_LIBRARY} 
								${NELSNDDRV_LIBRARY} 
								${NELLIGO_LIBRARY} 
								${NELGEORGES_LIBRARY} 
								${NEL3D_LIBRARY} 
								${JPEG_LIBRARY} 
								${FREETYPE_LIBRARY} 
								${WWCOMMON_LIB} 
								${CEGUI_LIBRARY} 
								${PYTHON_LIBRARIES} 
								${Boost_LIBRARIES} 
								${PNG_LIBRARIES})
IF(WIN32)
  SET_TARGET_PROPERTIES(werewolf PROPERTIES LINK_FLAGS "/NODEFAULTLIB:libcmt /SUBSYSTEM:WINDOWS")
ENDIF(WIN32)
ADD_DEFINITIONS(${LIBXML2_DEFINITIONS})

INSTALL(TARGETS werewolf RUNTIME DESTINATION bin)
