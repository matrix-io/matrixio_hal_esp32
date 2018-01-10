ESPTOOLPY := voice_esptool
ESPPORT := ttyS0
ESPTOOLPY_SERIAL := $(ESPTOOLPY) --port $(ESPPORT) --baud $(ESPBAUD) --before $(CONFIG_ESPTOOLPY_BEFORE) --after $(CONFIG_ESPTOOLPY_AFTER)
ESPTOOLPY_WRITE_FLASH=$(ESPTOOLPY_SERIAL) write_flash $(if $(CONFIG_ESPTOOLPY_COMPRESSED),-z,-u) $(ESPTOOL_WRITE_FLASH_OPTIONS)

DEPLOY_CMD:=$(ESPTOOLPY_WRITE_FLASH) $(ESPTOOL_ALL_FLASH_ARGS)
deploy:
	scp build/bootloader/bootloader.bin \
	$(APP_BIN) \
	build/partitions_singleapp.bin \
	$(RPI_HOST):/tmp
	@echo "In the RASPBERRY PI"
	@echo $(DEPLOY_CMD) | sed -e 's:'$(PWD)/xxx/'::g'
