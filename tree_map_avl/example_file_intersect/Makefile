DIRS=src/bst src/tree_map_avl src/intersect src/intersect_cli test/ src/intersect_exe
BUILD_DIRS=$(DIRS:%=build-%)
DEBUG_DIRS=$(DIRS:%=debug-%)
CLEAN_DIRS=$(DIRS:%=clean-%)
CHECK_DIRS=$(DIRS:%=check-%)
PROFILE_DIRS=$(DIRS:%=profile-%)

all: SETUP $(BUILD_DIRS)

SETUP:
	@mkdir -p bin

$(BUILD_DIRS):
	$(MAKE) -C $(@:build-%=%)

clean: $(CLEAN_DIRS)
$(CLEAN_DIRS):
	$(MAKE) -C $(@:clean-%=%) clean

debug: $(DEBUG_DIRS)
$(DEBUG_DIRS):
	$(MAKE) -C $(@:debug-%=%) debug

profile: $(PROFILE_DIRS)
$(PROFILE_DIRS):
	$(MAKE) -C $(@:profile-%=%) profile

check: $(CHECK_DIRS)
$(CHECK_DIRS):
	$(MAKE) -C $(@:check-%=%) check
