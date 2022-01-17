#
# Component Makefile
#

ifdef CONFIG_BOARD_ESP32_LCDKIT
    COMPONENT_SRCDIRS := esp32-lcdkit
else ifdef CONFIG_BOARD_LAVAZZA_3_5
    COMPONENT_SRCDIRS := lavazza-3_5
else ifdef CONFIG_BOARD_LAVAZZA_4_3
    COMPONENT_SRCDIRS := lavazza-4_3
endif

COMPONENT_ADD_INCLUDEDIRS := $(COMPONENT_SRCDIRS)

# $(COMPONENT_SRCDIRS)/board.o:
# 	@echo "-----------Board Info---------"
# 	@echo "IDF_TARGET = $(IDF_TARGET)"
# 	@echo "Board DIR = $(COMPONENT_SRCDIRS)"
# 	@echo "---------Board Info End---------"
