add_executable(CoreDomainTests
    test_core_domain.cpp
)

target_link_libraries(CoreDomainTests
    PRIVATE
        ETFDecisionTerminal::CoreDomain
)

target_compile_features(CoreDomainTests PRIVATE cxx_std_17)

add_test(
    NAME coredomain_types
    COMMAND CoreDomainTests
)
