add_library(dronecode_sdk_comm ${PLUGIN_LIBRARY_TYPE}
    comm.cpp
    comm_impl.cpp
)

target_link_libraries(dronecode_sdk_comm
    dronecode_sdk
)

set_target_properties(dronecode_sdk_comm
    PROPERTIES COMPILE_FLAGS ${warnings}
)

install(FILES
    include/plugins/comm/comm.h
    DESTINATION ${dronecode_sdk_install_include_dir}
)

install(TARGETS dronecode_sdk_comm
    #EXPORT dronecode_sdk-targets
    DESTINATION ${dronecode_sdk_install_lib_dir}
)

target_include_directories(dronecode_sdk_comm
PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)