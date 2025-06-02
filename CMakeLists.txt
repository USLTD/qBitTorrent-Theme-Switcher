# CMakeList.txt : CMake project for qbittorent-theme-switcher, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.14)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("qbittorent-theme-switcher")

# fetch latest argparse
include(FetchContent)
FetchContent_Declare(
    argparse
    GIT_REPOSITORY https://github.com/p-ranav/argparse.git
)
FetchContent_MakeAvailable(argparse)

# Add source to this project's executable.
add_executable (qbittorent-theme-switcher "main.cpp")
target_link_libraries(qbittorent-theme-switcher argparse)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET qbittorent-theme-switcher PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
