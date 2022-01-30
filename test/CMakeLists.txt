project(test)

if (CHOICE_SINK_LOGROTATE)
   include_directories(${G3LOG_INCLUDE_DIR} ${g3sinks_SOURCE_DIR}/sink_logrotate/src)
   set(LOGROTATE_TEST_FILES FilterTest.cpp RotateFileTest.cpp RotateTestHelper.cpp)
   add_executable(test_logrotate ${TEST_MAIN} ${LOGROTATE_TEST_FILES})
   target_link_libraries(
     test_logrotate 
     PRIVATE gtest_main 
     PRIVATE ${G3LOG_LIBRARY}
     PRIVATE g3logrotate)
   add_test(test_logrotate test_logrotate)
endif()

if (CHOICE_SINK_LOGROTATE_FS)
    # Define the test executable
    set(TEST_LOGROTATE_FS_EXE test_logrotatefs)
    add_executable(${TEST_LOGROTATE_FS_EXE} "")

    # Add the sources and the headers
    target_sources(
        ${TEST_LOGROTATE_FS_EXE}
        PRIVATE
            ${TEST_MAIN}
            LogRotateFsUtilityTest.cpp
    )

    # Add the include directories
    target_include_directories(
        ${TEST_LOGROTATE_FS_EXE}
        PRIVATE
            ${G3LOG_INCLUDE_DIR}
            ${g3sinks_SOURCE_DIR}/sink_logrotate_fs/src
    )

    # Add the dependencies
    target_link_libraries(
        ${TEST_LOGROTATE_FS_EXE}
        PRIVATE
            gtest_main
            ${G3LOG_LIBRARY}
            ${G3LOGROTATE_FS_LIB_NAME}
    )

    add_test(${TEST_LOGROTATE_FS_EXE} ${TEST_LOGROTATE_FS_EXE})
endif()
