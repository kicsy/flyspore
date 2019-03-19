aux_source_directory(. LIB_SRC_LEVEL2)

add_library(libLevel2 ${LIB_SRC_LEVEL2})

target_link_libraries(libLevel2 libLevel1)

set_property(TARGET libLevel2 PROPERTY CXX_STANDARD 17)