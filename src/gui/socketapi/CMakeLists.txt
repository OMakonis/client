target_sources(owncloudCore PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/socketapi.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/socketuploadjob.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/getJson.cpp
    )
    
if( APPLE )
    target_sources(owncloudCore PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/socketapisocket_mac.mm)
endif()
