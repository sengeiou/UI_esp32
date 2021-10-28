idf_component_register(
    SRC_DIRS
        "src"
        "icon"
        "src/ui"
        "lv_fonts"
        "lv_lib_qrcode"
    INCLUDE_DIRS
        "include"
        "lv_lib_qrcode")

component_compile_definitions(ADVANCED_DEBUG)

#spiffs_create_partition_image(storage ../spiffs FLASH_IN_PROJECT)
