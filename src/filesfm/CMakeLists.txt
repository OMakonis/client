project(filesfm)

# find_package(Qt5 REQUIRED COMPONENTS Core Widgets)


add_library(filesfm STATIC 
    
    filesfm.cpp
    api.cpp
    api_file_actions.cpp
    commands.cpp
    files.cpp    
    folder.cpp
    folders.cpp
    logs.cpp
    messaging.cpp
    helpers.cpp
    logger.cpp
    request.cpp
    requestparameter.cpp
    shell_extensions.cpp

    filewatcher/file.cpp
    filewatcher/filewatcher.cpp
    filewatcher/filewatcher_run.cpp
    filewatcher/helpers.cpp
    filewatcher/process.cpp
)

# This won't work, sadly
# set_property(TARGET filesfm PROPERTY CXX_STANDARD 17)



#
# Win32 API
#
find_library(OLE32 ole32)
list(APPEND LIB_DEPS ${USER32})

find_library(NTDLL ntdll)
list(APPEND LIB_DEPS ${NTDLL})

find_library(SHELL32 shell32)
list(APPEND LIB_DEPS ${SHELL32})

find_library(UUID uuid)
list(APPEND LIB_DEPS ${UUID})






#
# CURL
#
FIND_PACKAGE(CURL)
IF(CURL_FOUND)
  include_directories("${CURL_INCLUDE_DIRS}/curl")
  # list(APPEND LIB_DEPS ${CURL_LIBRARIES})
  list(APPEND LIB_DEPS "C:/CraftRoot/lib/libcurl_imp.lib")
ELSE(CURL_FOUND)
  MESSAGE(FATAL_ERROR "Could not find the CURL library and development files.")
ENDIF(CURL_FOUND)


#
# JSON support
#
#include_directories(${CMAKE_SOURCE_DIR}/src/3rdparty/nlohmann)


#
# Linking
#
target_link_libraries(filesfm LINK_PUBLIC ${LIB_DEPS})