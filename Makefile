EXEC = CHIP8-emu

.PHONY: clean

# If the first argument is "run"...
ifeq (run,$(firstword $(MAKECMDGOALS)))
  # use the rest as arguments for "run"
  RUN_ARGS := $(wordlist 2,$(words $(MAKECMDGOALS)),$(MAKECMDGOALS))
  # ...and turn them into do-nothing targets
  $(eval $(RUN_ARGS):;@:)
endif

all:
	$(MAKE) -C src all

clean:
	$(MAKE) -C src clean

run:
	./$(EXEC) $(RUN_ARGS)
