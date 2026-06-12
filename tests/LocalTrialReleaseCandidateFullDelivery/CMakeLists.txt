find_package(Qt6 COMPONENTS Core REQUIRED)

add_executable(LocalTrialReleaseCandidateFullDelivery
    local_trial_release_candidate_full_delivery.cpp
)

target_link_libraries(LocalTrialReleaseCandidateFullDelivery
    PRIVATE
        Qt6::Core
)

target_compile_features(LocalTrialReleaseCandidateFullDelivery PRIVATE cxx_std_17)

add_test(
    NAME local_trial_release_candidate_full_delivery
    COMMAND
        LocalTrialReleaseCandidateFullDelivery
        --source-root
        ${PROJECT_SOURCE_DIR}
)

set_tests_properties(local_trial_release_candidate_full_delivery PROPERTIES
    ENVIRONMENT_MODIFICATION
        "PATH=path_list_prepend:$<TARGET_FILE_DIR:Qt6::Core>"
)
