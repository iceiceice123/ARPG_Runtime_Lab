add_executable(arpg_runtime_app
    main.cpp
)

target_link_libraries(arpg_runtime_app
    PRIVATE
        arpg::runtime
        arpg_compile_options
)

set_target_properties(arpg_runtime_app PROPERTIES FOLDER "Apps")
