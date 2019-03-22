aux_source_directory(. LIB_SRC_LEVEL0)

add_library(libLevel0 ${LIB_SRC_LEVEL0})

target_link_libraries(libLevel0 pthread)

set_property(TARGET libLevel0 PROPERTY CXX_STANDARD 17)