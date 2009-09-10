ADD_SUBDIRECTORY(src)

INSTALL(FILES werewolf_default.cfg DESTINATION etc/werewolf)
INSTALL(DIRECTORY data/
        DESTINATION share/games/werewolf/data
        COMPONENT data
        PATTERN "CVS" EXCLUDE
        PATTERN ".svn" EXCLUDE
        PATTERN "Makefile*" EXCLUDE)
        
