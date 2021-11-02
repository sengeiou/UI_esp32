set(srcs_dirs 	"src"
				"icon"
				"src/gui"
				"src/utils"
				"src/dbg"
				"src/connectivity"
				"lv_fonts"
				"lv_lib_qrcode")

set(incdir 	"inc"
			"inc/gui"
			"inc/utils"
			"inc/dbg"
			"inc/connectivity"
			"lv_lib_qrcode")	
		
set(resources 	"resources/index.html"
		"resources/menu.html")
					
idf_component_register(SRC_DIRS ${srcs_dirs}
                        INCLUDE_DIRS ${incdir}
                        EMBED_FILES ${resources})
					
component_compile_definitions(ADVANCED_DEBUG)

# spiffs_create_partition_image(storage ../spiffs FLASH_IN_PROJECT)
