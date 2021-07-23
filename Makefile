DIRS=src/bst test/bst_test
BUILD_DIRS=$(DIRS:%=build-%)
CLEAN_DIRS=$(DIRS:%=clean-%)
DEBUG_DIRS=$(DIRS:%=debug-%)
PROFILE_DIRS=$(DIRS:%=profile-%)

all: $(BUILD_DIRS)

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
