add_executable(ProtocolTests
    test_protocol.cpp
)

target_link_libraries(ProtocolTests
    PRIVATE
        ETFDecisionTerminal::Protocol
)

target_compile_features(ProtocolTests PRIVATE cxx_std_17)

add_test(
    NAME protocol_messages
    COMMAND ProtocolTests
)
