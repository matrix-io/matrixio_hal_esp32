deploy:
	scp build/bootloader/bootloader.bin \
	$(APP_BIN) \
	build/partitions_singleapp.bin \
	$(RPI_HOST):/tmp

