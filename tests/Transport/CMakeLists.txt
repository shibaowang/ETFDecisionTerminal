find_package(Qt6 COMPONENTS Core Network REQUIRED)

add_executable(TransportTests
    test_transport.cpp
)

target_link_libraries(TransportTests
    PRIVATE
        ETFDecisionTerminal::Transport
)

target_compile_features(TransportTests PRIVATE cxx_std_17)

add_test(
    NAME transport_frame_codec
    COMMAND TransportTests --frame-codec
)

add_test(
    NAME transport_local_socket_echo
    COMMAND TransportTests --local-socket
)

set_tests_properties(
    transport_frame_codec
    transport_local_socket_echo
    PROPERTIES
        ENVIRONMENT_MODIFICATION
            "PATH=path_list_prepend:$<TARGET_FILE_DIR:Qt6::Core>"
)
