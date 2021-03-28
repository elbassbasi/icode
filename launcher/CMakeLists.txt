if(WIN32)
if(MINGW)
  set(CMAKE_RC_COMPILER_INIT windres)
  ENABLE_LANGUAGE(RC)
  SET(CMAKE_RC_COMPILE_OBJECT
    "<CMAKE_RC_COMPILER> <FLAGS> -O coff <DEFINES> -i <SOURCE> -o <OBJECT>")
endif(MINGW)
add_executable(launch1 win_launch1.c win_launch.rc)
add_executable(launch2 win_launch2.c win_launch.rc)
endif(WIN32)

if (UNIX AND NOT APPLE)
add_executable(launch1 linux_launch1.c)
add_executable(launch2 linux_launch2.c)
endif()

if (UNIX AND APPLE)
add_executable(launch1 macos_launch1.c)
add_executable(launch2 macos_launch2.c)
endif()

set_target_properties(launch1
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build
)
set_target_properties(launch1 PROPERTIES OUTPUT_NAME "icode")
#
target_link_libraries(launch2 icode)
set_target_properties(launch2 PROPERTIES OUTPUT_NAME "icode")