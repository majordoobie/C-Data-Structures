DIRS=src/bst src/tree_map_avl src/heap src/stack_adt test
BUILD_DIRS=$(DIRS:%=build-%)
CLEAN_DIRS=$(DIRS:%=clean-%)
DEBUG_DIRS=$(DIRS:%=debug-%)
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
