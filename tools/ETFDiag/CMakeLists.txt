find_package(Qt6 COMPONENTS Core REQUIRED)

add_executable(ETFDiag
    src/main.cpp
)

target_link_libraries(ETFDiag
    PRIVATE
        Qt6::Core
)

target_compile_features(ETFDiag PRIVATE cxx_std_17)
