ADD_SUBDIRECTORY(src)

INSTALL(DIRECTORY data/
        DESTINATION ${NL_SHARE_PREFIX}/werewolf/data
        COMPONENT data
        PATTERN "CVS" EXCLUDE
        PATTERN ".svn" EXCLUDE
        PATTERN "Makefile*" EXCLUDE)
        