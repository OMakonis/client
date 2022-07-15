add_library(owncloud_vfs_cfapi MODULE
    cfapiwrapper.h
    cfapiwrapper.cpp
    hydrationjob.h
    hydrationjob.cpp
    vfs_cfapi.h
    vfs_cfapi.cpp
)

target_link_libraries(owncloud_vfs_cfapi 
    libsync
)

set_target_properties(owncloud_vfs_cfapi
  PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY
      ${BIN_OUTPUT_DIRECTORY}
    RUNTIME_OUTPUT_DIRECTORY
      ${BIN_OUTPUT_DIRECTORY}
    PREFIX
      ""
    AUTOMOC
      TRUE
    LIBRARY_OUTPUT_NAME
      ${APPLICATION_EXECUTABLE}sync_vfs_cfapi
    RUNTIME_OUTPUT_NAME
      ${APPLICATION_EXECUTABLE}sync_vfs_cfapi
)

target_include_directories("owncloud_vfs_cfapi" BEFORE PUBLIC ${CMAKE_CURRENT_BINARY_DIR} INTERFACE ${CMAKE_BINARY_DIR})

set(vfs_installdir "${PLUGINDIR}")

generate_export_header(owncloud_vfs_cfapi
    BASE_NAME owncloud_vfs_cfapi
    EXPORT_MACRO_NAME NEXTCLOUD_CFAPI_EXPORT
    EXPORT_FILE_NAME cfapiexport.h
)

install(TARGETS owncloud_vfs_cfapi
  LIBRARY DESTINATION "${KDE_INSTALL_PLUGINDIR}"
  RUNTIME DESTINATION "${KDE_INSTALL_PLUGINDIR}"
)
