# Add sources and headers to the lib
target_sources(
    ${G3LOGROTATE_FS_LIB_NAME}
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/g3sinks/LogRotateFsUtility.h
        ${CMAKE_CURRENT_LIST_DIR}/g3sinks/LogRotateFs.h
        ${CMAKE_CURRENT_LIST_DIR}/LogRotateFsUtility.cpp
        ${CMAKE_CURRENT_LIST_DIR}/LogRotateFs.cpp
)
