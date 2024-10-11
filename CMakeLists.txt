# CMakeList.txt: cppserverstudio 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.8)

# 如果支持，请为 MSVC 编译器启用热重载。
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

set(CMAKE_C_COMPILER "/usr/bin/gcc")
set(CMAKE_CXX_COMPILER "/usr/bin/g++")

project ("cppserverstudio" LANGUAGES C CXX)

# 使能 C++11
# Enable C++11
set(CMAKE_CXX_STANDARD 11)

set(CMAKE_MAKE_PROGRAM "/usr/bin/ninja")

add_subdirectory(src)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET cppserverstudio PROPERTY CXX_STANDARD 20)
endif()

# TODO: 如有需要，请添加测试并安装目标。
