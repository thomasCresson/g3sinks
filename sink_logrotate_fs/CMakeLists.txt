project(logrotate_fs)

# Find ZLIB package
find_package(ZLIB REQUIRED)
include_directories(${ZLIB_INCLUDE_DIRS})
message("zlib found: ${ZLIB_FOUND} ")

# Define the logrotate_fs source directory
set(LOGROTATE_FS_SRC_DIR ${logrotate_fs_SOURCE_DIR}/src)

# Define the library itself
set(G3LOGROTATE_FS_LIB_NAME g3logrotatefs CACHE INTERNAL "")

if(CHOICE_BUILD_STATIC)
    add_library(${G3LOGROTATE_FS_LIB_NAME} STATIC "")
else()
    add_library(${G3LOGROTATE_FS_LIB_NAME} SHARED "")
endif()

# Ajout des sources contenues dans src
add_subdirectory(src)

target_include_directories(
    ${G3LOGROTATE_FS_LIB_NAME}
    PRIVATE
        ${LOGROTATE_FS_SRC_DIR}
)

# Define dependencies
target_link_libraries(
    ${G3LOGROTATE_FS_LIB_NAME}
    PUBLIC
        ${G3LOG_LIBRARY}
    PRIVATE
        ${ZLIB_LIBRARY}
)

message("target_link_libraries:  
         g3log: ${G3LOG_LIBRARY}, 
         zlib: ${ZLIB_LIBRARIES}" )

# set target properties
if (MSVC)
   set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

set_target_properties(
    ${G3LOGROTATE_FS_LIB_NAME}
    PROPERTIES SOVERSION ${VERSION}
)

if (UNIX)
   target_compile_options(${G3LOGROTATE_FS_LIB_NAME} PRIVATE)
endif()



# INSTALLATION
# ===================================================
set(G3LOGROTATE_FS_TARGETS g3logrotatefsTargets)

install(TARGETS
           ${G3LOGROTATE_FS_LIB_NAME}
        EXPORT
            ${G3LOGROTATE_FS_TARGETS}
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        INCLUDES DESTINATION include)

install(EXPORT ${G3LOGROTATE_FS_TARGETS}
  NAMESPACE G3::
  DESTINATION lib/cmake/g3sinks
  )

install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/src/g3sinks"
  DESTINATION include
  FILES_MATCHING
  PATTERN *.h*
  )

include(CMakePackageConfigHelpers)
write_basic_package_version_file("g3logrotatefsVersion.cmake"
  VERSION ${VERSION}
  COMPATIBILITY AnyNewerVersion
  )
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/g3logrotatefsVersion.cmake"
  DESTINATION lib/cmake/g3sinks
  )

