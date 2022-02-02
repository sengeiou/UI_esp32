
set(boards_dir "")

if(CONFIG_BOARD_ESP32_LCDKIT)
    set(boards_dir "esp32-lcdkit")
elseif(CONFIG_BOARD_LAVAZZA_3_5)
    set(boards_dir "lavazza-3_5")
elseif(CONFIG_BOARD_LAVAZZA_4_3)
    set(boards_dir "lavazza-4_3")
endif()

message(STATUS "-----------Board Info---------")
message(STATUS "IDF_TARGET = ${IDF_TARGET}")
message(STATUS "Board DIR = ${boards_dir}")
message(STATUS "---------Board Info End---------")


idf_component_register( SRC_DIRS "${boards_dir}"
                        INCLUDE_DIRS "${boards_dir}"
                        REQUIRES bus)
