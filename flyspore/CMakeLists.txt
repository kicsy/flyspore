aux_source_directory(. EXE_SRC_FLYSPORE)

add_executable(flyspore ${EXE_SRC_FLYSPORE})

target_include_directories(flyspore PRIVATE ${PROJECT_SOURCE_DIR})

target_link_libraries(flyspore libLevel1)

target_link_libraries(flyspore libLevel2)

set_property(TARGET flyspore PROPERTY CXX_STANDARD 17)