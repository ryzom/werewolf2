@echo off

set CP=WEB-INF/classes;WEB-INF/lib/dom4j-1.5.2.jar;WEB-INF/lib/jaxen-1.1-beta-5.jar

java -cp %CP% server.ServerConfCreator %*
