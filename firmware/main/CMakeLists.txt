set(srcs_dirs 	"src"
				"icon"
				"src/gui"
				"src/utils"
				"src/dbg"
				"src/camera"
				"src/connectivity"
				"lv_fonts"
				"lv_lib_qrcode")

set(incdir 	"inc"
			"inc/gui"
			"inc/utils"
			"inc/dbg"
			"inc/camera"
			"inc/connectivity"
			"lv_lib_qrcode")	

set(COMPONENT_REQUIRES
    esp-azure
	lvgl
	lvgl_esp32_drivers)

idf_component_register(SRC_DIRS ${srcs_dirs}
                        INCLUDE_DIRS ${incdir})
					
component_compile_definitions(ADVANCED_DEBUG)

# spiffs_create_partition_image(storage ../spiffs FLASH_IN_PROJECT)
