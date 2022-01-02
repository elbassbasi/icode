file(GLOB src_swtp "../swt/src_swtp/*.cpp")
file(GLOB src_icode_core "src/core/*.cpp")
file(GLOB src_icode_managers "src/managers/*.cpp")
file(GLOB src_icode_db "src/db/*.cpp")
file(GLOB src_icode_model "src/model/*.cpp")
file(GLOB src_icode_text "src/text/*.cpp")
file(GLOB src_icode_resources "src/resources/*.cpp")
file(GLOB src_icode_ui "src/ui/*.cpp")
file(GLOB src_icode_ui_managers "src/ui/managers/*.cpp")
file(GLOB src_icode_ui_commands "src/ui/commands/*.cpp")
file(GLOB src_icode_ui_views "src/ui/views/*.cpp")
file(GLOB src_icode_zlib "src/zlib/*.c")

add_library(icode SHARED ${src_swtp} ${src_icode_core} ${src_icode_managers} ${src_icode_db} ${src_icode_model} ${src_icode_text} 
			${src_icode_ui} ${src_icode_ui_managers} ${src_icode_ui_commands} ${src_icode_ui_views} ${src_icode_zlib} ${src_icode_resources})
target_include_directories(icode PRIVATE ../swt)
target_include_directories(icode PRIVATE .)
target_compile_definitions(icode PRIVATE ICODEBUILD)
#target_link_directories(icode PRIVATE ${CMAKE_BINARY_DIR}/bin/plugins)
#link_directories(${PROJECT_BINARY_DIR}/plugins)
target_link_libraries(icode PRIVATE swtp)


 add_custom_target(icode-copy-files ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/data
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/../plugins/icode
    )