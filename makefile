all:
	make -C mutatee/
	@echo ""
	@echo Run test under a DBI framework:
	@echo
	@echo - DynamoRIO:
	@echo "  make dynamorio"
	@echo
	@echo - Dyninst:
	@echo "  make dyninst"
	@echo
	@echo - PinTool:
	@echo "  make pintool"
	@echo
	@echo - DynamoRIO experimental Probe API:
	@echo "  make probe-api"

clean:
	make -C dynamorio/ clean
	make -C dyninst/   clean
	make -C pintool/   clean
	make -C mutatee/   clean
	make -C probe-api/   clean

.PHONY: dynamorio
dynamorio:
	make -C $@

.PHONY: dyninst
dyninst:
	make -C $@

.PHONY: pintool
pintool:
	make -C $@

.PHONY: probe-api
probe-api:
	make -C $@
