#Copyright (c) 2019, SenseTime Group.
#All rights reserved.

set(GLOG_INCLUDE_DIR_HINTS "" CACHE PATH "Glog include directory")
set(GLOG_LIBRARY_DIR_HINTS "" CACHE PATH "Glog library directory")

unset(GLOG_FOUND)
unset(GLOG_INCLUDE_DIRS)
unset(GLOG_LIBRARIES)

include(FindPackageHandleStandardArgs)

list(APPEND GLOG_CHECK_INCLUDE_DIRS
    /usr/local/include
    /usr/local/homebrew/include
    /opt/local/var/macports/software
    /opt/local/include
    /usr/include)
list(APPEND GLOG_CHECK_PATH_SUFFIXES
    glog/include
    glog/Include
    Glog/include
    Glog/Include
    src/windows)

list(APPEND GLOG_CHECK_LIBRARY_DIRS
    /usr/local/lib
    /usr/local/homebrew/lib
    /opt/local/lib
    /usr/lib)
list(APPEND GLOG_CHECK_LIBRARY_SUFFIXES
    glog/lib
    glog/Lib
    Glog/lib
    Glog/Lib
    x64/Release)

find_path(GLOG_INCLUDE_DIRS
    NAMES
    glog/logging.h
    PATHS
    ${GLOG_INCLUDE_DIR_HINTS}
    ${GLOG_CHECK_INCLUDE_DIRS}
    PATH_SUFFIXES
    ${GLOG_CHECK_PATH_SUFFIXES})
find_library(GLOG_LIBRARIES
    NAMES
    glog
    libglog
    PATHS
    ${GLOG_LIBRARY_DIR_HINTS}
    ${GLOG_CHECK_LIBRARY_DIRS}
    PATH_SUFFIXES
    ${GLOG_CHECK_LIBRARY_SUFFIXES})

if (GLOG_INCLUDE_DIRS AND GLOG_LIBRARIES)
    set(GLOG_FOUND TRUE)
    message(STATUS "Found Glog")
    message(STATUS "  Includes : ${GLOG_INCLUDE_DIRS}")
    message(STATUS "  Libraries : ${GLOG_LIBRARIES}")
else()
    if(Glog_FIND_REQUIRED)
        message(FATAL_ERROR "Could not find Glog")
    endif()
endif()
