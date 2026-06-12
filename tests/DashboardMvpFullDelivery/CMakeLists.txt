find_package(Qt6 COMPONENTS Core REQUIRED)

add_executable(DashboardMvpFullDelivery
    dashboard_mvp_full_delivery.cpp
)

target_link_libraries(DashboardMvpFullDelivery
    PRIVATE
        Qt6::Core
)

target_compile_features(DashboardMvpFullDelivery PRIVATE cxx_std_17)

add_test(
    NAME dashboard_mvp_full_delivery
    COMMAND
        DashboardMvpFullDelivery
        --source-root
        ${PROJECT_SOURCE_DIR}
)

set_tests_properties(dashboard_mvp_full_delivery PROPERTIES
    ENVIRONMENT_MODIFICATION
        "PATH=path_list_prepend:$<TARGET_FILE_DIR:Qt6::Core>"
)
