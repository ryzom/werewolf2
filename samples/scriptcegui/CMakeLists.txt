FILE(GLOB SRC *.cpp *.h)

ADD_EXECUTABLE(script_cegui_sample WIN32 ${SRC})

ADD_DEFINITIONS(-DCEGUI_DATA_DIR="\\"${NL_SHARE_PREFIX}/games/werewolf/data/\\"")

INCLUDE_DIRECTORIES(${LIBXML2_INCLUDE_DIR} ${CEGUI_INCLUDE_DIRS} ${NEL_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/wwscript/include)

TARGET_LINK_LIBRARIES(script_cegui_sample
						optimized ${NELMISC_LIBRARY}	debug ${NELMISC_LIBRARY_DEBUG}
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
  SET_TARGET_PROPERTIES(script_cegui_sample PROPERTIES 
	LINK_FLAGS_DEBUG "/NODEFAULTLIB:libc;libcmt;libcmtd;msvcrt"
	LINK_FLAGS_RELEASE "/NODEFAULTLIB:libc;libcmt;libcmtd;msvcrtd"
	PROJECT_LABEL "Samples: CEGUI Script Demo")
ENDIF(WIN32)
ADD_DEFINITIONS(${LIBXML2_DEFINITIONS})

INSTALL(TARGETS script_cegui_sample RUNTIME DESTINATION bin COMPONENT samples)
INSTALL(DIRECTORY datafiles/ 
	DESTINATION share/script_cegui_sample/datafiles
	COMPONENT samples
	PATTERN "CVS" EXCLUDE
	PATTERN ".svn" EXCLUDE
	PATTERN "Makefile*" EXCLUDE)
