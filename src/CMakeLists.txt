aux_source_directory(. SRCLIST)

include_directories(../include)

add_executable(cppserverstudio ${SRCLIST})

set (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)