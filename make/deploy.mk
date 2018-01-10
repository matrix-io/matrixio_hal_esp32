ESPTOOL := voice_esptool
ESPPORT := /dev/ttyS0
ESPTOOL_SERIAL := $(ESPTOOL) --port $(ESPPORT) --baud $(ESPBAUD) --before $(CONFIG_ESPTOOL_BEFORE) --after $(CONFIG_ESPTOOL_AFTER)
ESPTOOL_WRITE_FLASH=$(ESPTOOL_SERIAL) write_flash $(if $(CONFIG_ESPTOOL_COMPRESSED),-z,-u) $(ESPTOOL_WRITE_FLASH_OPTIONS)

DEPLOY_CMD:=$(ESPTOOL_WRITE_FLASH) $(ESPTOOL_ALL_FLASH_ARGS)

REMOTE_CMD:=$(DEPLOY_CMD) | sed -e "s=bootloader/==g" | sed -e "s=$(PWD)==g" | sed -e "s=/build=/tmp=g"

deploy:
	@echo "********************"	
	@echo "Copying files"
	@echo "********************"	
	scp build/bootloader/bootloader.bin \
	$(APP_BIN) \
	build/partitions_singleapp.bin \
	$(RPI_HOST):/tmp
	@echo ""
	@echo "********************"	
	@echo "To program the ESP32"
	@echo "********************"	
	@echo ssh $(RPI_HOST) $(REMOTE_CMD)

 
