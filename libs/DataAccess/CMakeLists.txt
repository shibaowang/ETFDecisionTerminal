find_package(SQLite3 QUIET)

if(NOT SQLite3_FOUND)
    set(ETFDT_SQLITE3_AMALGAMATION_DIR
        ""
        CACHE PATH
        "Directory containing sqlite3.c and sqlite3.h, used when find_package(SQLite3) is unavailable"
    )

    set(_etfdt_sqlite3_candidates
        "${ETFDT_SQLITE3_AMALGAMATION_DIR}"
        "$ENV{SQLITE3_AMALGAMATION_DIR}"
        "$ENV{SQLite3_ROOT}"
        "$ENV{SQLITE3_ROOT}"
        "C:/Program Files/QClaw/resources/app.asar.unpacked/node_modules/better-sqlite3/deps/sqlite3"
    )

    foreach(_candidate IN LISTS _etfdt_sqlite3_candidates)
        if(_candidate
           AND EXISTS "${_candidate}/sqlite3.c"
           AND EXISTS "${_candidate}/sqlite3.h")
            set(_etfdt_sqlite3_amalgamation_dir "${_candidate}")
            break()
        endif()
    endforeach()

    if(_etfdt_sqlite3_amalgamation_dir)
        message(STATUS "Using SQLite3 amalgamation: ${_etfdt_sqlite3_amalgamation_dir}")

        add_library(ETFSQLite3 STATIC
            "${_etfdt_sqlite3_amalgamation_dir}/sqlite3.c"
        )

        target_include_directories(ETFSQLite3
            PUBLIC
                "${_etfdt_sqlite3_amalgamation_dir}"
        )

        add_library(SQLite::SQLite3 ALIAS ETFSQLite3)
    else()
        message(FATAL_ERROR
            "SQLite3 development files were not found. Install SQLite3 development package "
            "or configure with -DETFDT_SQLITE3_AMALGAMATION_DIR=<dir containing sqlite3.c and sqlite3.h>. "
            "On Windows, vcpkg example: vcpkg install sqlite3:x64-windows and configure CMake with the vcpkg toolchain."
        )
    endif()
endif()

add_library(ETFDataAccess STATIC
    src/AccountingTradeFactReader.cpp
    src/AuditLogRepository.cpp
    src/DataAccess.cpp
    src/MigrationRunner.cpp
    src/ReadOnlyRepositoryBase.cpp
    src/Repositories.cpp
    src/ShellAccountingAuditWriteRepository.cpp
    src/ShellAccountingBrokerOrderDryRunRepository.cpp
    src/ShellAccountingManualEntryRepositoryScaffold.cpp
    src/ShellAccountingReadOnlyFactsQuery.cpp
    src/ShellAccountingSnapshotWriteRepository.cpp
    src/ShellAccountingTradeDraftConfirmationRepository.cpp
    src/ShellAccountingTradeDraftRepository.cpp
    src/SQLiteConnection.cpp
    src/Transaction.cpp
    src/TransactionRunner.cpp
)

add_library(ETFDecisionTerminal::DataAccess ALIAS ETFDataAccess)

target_include_directories(ETFDataAccess
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(ETFDataAccess
    PUBLIC
        ETFDecisionTerminal::CoreDomain
        ETFDecisionTerminal::Protocol
        SQLite::SQLite3
)

target_compile_features(ETFDataAccess PUBLIC cxx_std_17)

set_target_properties(ETFDataAccess PROPERTIES
    OUTPUT_NAME ETFDataAccess
)
