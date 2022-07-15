add_library(owncloud_vfs_cfapi MODULE
    cfapiwrapper.h
    cfapiwrapper.cpp
    hydrationjob.h
    hydrationjob.cpp
    vfs_cfapi.h
    vfs_cfapi.cpp
)

set_target_properties(owncloud_vfs_cfapi PROPERTIES OUTPUT_NAME "${synclib_NAME}_vfs_cfapi")

target_link_libraries(owncloud_vfs_cfapi
    libsync
)
INSTALL(TARGETS owncloud_vfs_cfapi DESTINATION "${KDE_INSTALL_PLUGINDIR}")

