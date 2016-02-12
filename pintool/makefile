include ../config

all: test

test: mutator
	$(PIN_ROOT)/pin.sh -probe -follow_execv -t obj-intel64/mutator.so  -- \
	$(APP) $(APP_ARGS)

mutator:
	$(MAKE) -f Makefile

clean:
	$(MAKE) -f Makefile clean
