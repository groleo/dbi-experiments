include config
all:
	make -C mutatee/
	@echo ""
	@echo Choose on target:
	@echo "  make -C dynamorio"
	@echo "  make -C dyninst"
	@echo "  make -C pintool"
	@echo "  make ref"
clean:
	make -C dynamorio/ clean
	make -C dyninst/   clean
	make -C pintool/   clean
	make -C mutatee/   clean
ref:
	$(CHROME) --enable-gpu-client-hooks $(CHROME_ARGS)
