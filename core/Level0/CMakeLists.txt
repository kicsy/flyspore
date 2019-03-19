aux_source_directory(. LIB_SRC_LEVEL0)

add_library(libLevel0 ${LIB_SRC_LEVEL0})

set_property(TARGET libLevel0 PROPERTY CXX_STANDARD 17)