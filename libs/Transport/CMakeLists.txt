set(ETFDT_QT6_ROOT
    ""
    CACHE PATH
    "Qt6 installation prefix containing Qt6Config.cmake, for example D:/Qt/6.6.3/msvc2019_64"
)

set(_etfdt_qt6_candidates
    "${ETFDT_QT6_ROOT}"
    "$ENV{QT6_ROOT}"
    "$ENV{Qt6_ROOT}"
    "D:/Qt/6.6.3/msvc2019_64"
    "C:/Qt/6.6.3/msvc2019_64"
)

find_package(Qt6 COMPONENTS Core Network QUIET
    PATHS
        ${_etfdt_qt6_candidates}
)

if(NOT Qt6_FOUND)
    message(FATAL_ERROR
        "Qt6 Core and Network development files were not found. Install Qt6 and configure with "
        "-DETFDT_QT6_ROOT=<Qt prefix> or add the Qt prefix to CMAKE_PREFIX_PATH. "
        "Windows aqt example: python -m aqt install-qt windows desktop 6.6.3 "
        "win64_msvc2019_64 -O D:/Qt --archives qtbase"
    )
endif()

add_library(ETFTransport STATIC
    src/FrameCodec.cpp
    src/LocalSocketClient.cpp
    src/LocalSocketServer.cpp
    src/Transport.cpp
)

add_library(ETFDecisionTerminal::Transport ALIAS ETFTransport)

target_include_directories(ETFTransport
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(ETFTransport
    PUBLIC
        ETFDecisionTerminal::CoreDomain
        ETFDecisionTerminal::Protocol
        Qt6::Core
        Qt6::Network
)

target_compile_features(ETFTransport PUBLIC cxx_std_17)

set_target_properties(ETFTransport PROPERTIES
    OUTPUT_NAME ETFTransport
)
