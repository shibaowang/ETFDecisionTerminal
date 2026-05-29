find_package(Python3 COMPONENTS Interpreter REQUIRED)

add_subdirectory(CoreDomain)
add_subdirectory(AccountingEngine)
add_subdirectory(AccountingFixtures)
add_subdirectory(AccountingNoWrite)
add_subdirectory(DataAccess)
add_subdirectory(DataService)
add_subdirectory(PositionListFirstStageScaffolding)
add_subdirectory(PositionListMapping)
add_subdirectory(PositionListRealActionNoWrite)
add_subdirectory(Protocol)
add_subdirectory(ServiceRuntime)
add_subdirectory(ServiceHost)
add_subdirectory(Transport)
add_subdirectory(DataServiceClient)
add_subdirectory(Diagnostics)
add_subdirectory(DevDocs)
add_subdirectory(ShellCore)
add_subdirectory(ShellAccountingControllerScaffolding)
add_subdirectory(ShellAccountingFakeClientBoundary)
add_subdirectory(ShellAccountingDataServiceAdapter)
add_subdirectory(ShellAccountingDataServiceAdapterLiveCallGate)
add_subdirectory(ShellAccountingDataServiceAdapterScaffolding)
add_subdirectory(ShellAccountingViewModel)
add_subdirectory(ShellAccountingPresenter)
add_subdirectory(ShellAccountingQmlBindingSmoke)
add_subdirectory(ShellAccountingQmlBindingSmokeRuntime)
add_subdirectory(ShellAccountingQmlStaticGate)
add_subdirectory(ShellAccountingServiceAdapter)
add_subdirectory(ShellServices)
add_subdirectory(ShellUiSmoke)
add_subdirectory(Watchdog)
add_subdirectory(ETFDiag)

add_test(
    NAME database_initial_schema
    COMMAND
        ${Python3_EXECUTABLE}
        ${CMAKE_CURRENT_SOURCE_DIR}/database/validate_schema.py
        --migration
        ${PROJECT_SOURCE_DIR}/migrations/001_initial_schema.sql
)

set_tests_properties(database_initial_schema PROPERTIES
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)
