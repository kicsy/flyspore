aux_source_directory(. LIB_SRC_LEVEL1)

add_library(libLevel1 ${LIB_SRC_LEVEL1})

#target_link_libraries(libLevel1 libLevel0)

set_property(TARGET libLevel1 PROPERTY CXX_STANDARD 17)