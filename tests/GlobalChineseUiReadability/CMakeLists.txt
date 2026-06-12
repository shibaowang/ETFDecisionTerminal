add_executable(GlobalChineseUiReadability
    global_chinese_ui_readability_trial_feedback_fix.cpp
)

target_compile_features(GlobalChineseUiReadability PRIVATE cxx_std_17)
if(MSVC)
    target_compile_options(GlobalChineseUiReadability PRIVATE /utf-8)
endif()

add_test(
    NAME global_chinese_ui_readability_trial_feedback_fix
    COMMAND
        GlobalChineseUiReadability
        --source-root
        ${PROJECT_SOURCE_DIR}
)
