# CMakeList.txt: cppserverstudio 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.8)


project ("cppserverstudio")

# 使能 C++11
# Enable C++11
set(CMAKE_CXX_STANDARD 11)

#set(CMAKE_MAKE_PROGRAM "/usr/bin/ninja")

#设置子目录
#Set subdirectories
set(SUB_DIR_LIST "Util" "NetWork")

foreach(SUB_DIR ${SUB_DIR_LIST})
    #遍历源文件
    #Traverse source file
    aux_source_directory(src/${SUB_DIR} SRC_LIST)
    include_directories(src/${SUB_DIR})
endforeach()

if(WIN32)
    set(LINK_LIB_LIST WS2_32 Iphlpapi shlwapi)
else()
    set(LINK_LIB_LIST)
endif()

#编译动态库
#Compile dynamic library
if(NOT IOS AND NOT ANDROID AND NOT WIN32)
    add_library(${PROJECT_NAME}_shared SHARED ${SRC_LIST})
    target_include_directories(${PROJECT_NAME}_shared PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
    target_link_libraries(${PROJECT_NAME}_shared ${LINK_LIB_LIST})
    set_target_properties(${PROJECT_NAME}_shared PROPERTIES OUTPUT_NAME "${PROJECT_NAME}")
    install(TARGETS ${PROJECT_NAME}_shared  ARCHIVE DESTINATION ${INSTALL_PATH_LIB} LIBRARY DESTINATION ${INSTALL_PATH_LIB})
endif()

#编译静态库
#Compile static library
add_library(${PROJECT_NAME}_static STATIC ${SRC_LIST})


add_subdirectory(tests)


# TODO: 如有需要，请添加测试并安装目标。
