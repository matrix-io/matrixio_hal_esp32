MV_ESPTOOLPY_SRC := voice_esptool

ESPPORT := /dev/ttyS0

MV_ESPTOOLPY := $(MV_ESPTOOLPY_SRC) --chip esp32

MV_ESPTOOLPY_SERIAL := $(MV_ESPTOOLPY) --port $(ESPPORT) --baud $(ESPBAUD) --before $(CONFIG_ESPTOOLPY_BEFORE) --after $(CONFIG_ESPTOOLPY_AFTER)

ESPTOOL_WRITE_FLASH := $(MV_ESPTOOLPY_SERIAL) write_flash $(if $(CONFIG_ESPTOOL_COMPRESSED),-z,-u) $(ESPTOOL_WRITE_FLASH_OPTIONS)

DEPLOY_CMD := $(shell echo $(ESPTOOL_WRITE_FLASH) $(ESPTOOL_ALL_FLASH_ARGS) | sed -e "s=bootloader/==g" | sed -e "s=$(PWD)==g" | sed -e "s=/build=/tmp=g")

deploy:
	@echo ""
	@echo "*********************************"	
	@echo "Copying files to the Raspberry PI"
	@echo "*********************************"	
	scp build/bootloader/bootloader.bin \
	$(APP_BIN) \
	build/partitions_singleapp.bin \
	$(RPI_HOST):/tmp
	@echo ""
	@echo "*********************************"	
	@echo "Programmig the ESP32"
	@echo "*********************************"	
	ssh $(RPI_HOST) $(DEPLOY_CMD)

 
