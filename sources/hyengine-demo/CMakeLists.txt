add_executable(hyengine-demo)

target_sources(hyengine-demo PRIVATE
    main.cpp
)

target_link_libraries(hyengine-demo PRIVATE hyengine)