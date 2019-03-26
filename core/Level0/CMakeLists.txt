aux_source_directory(. LIB_SRC_LEVEL0)

add_library(libLevel0 ${LIB_SRC_LEVEL0})

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_link_libraries(libLevel0 pthread)
endif()

set_property(TARGET libLevel0 PROPERTY CXX_STANDARD 17)