#-------------------
# Main Configuration
#-------------------
DIST_DIR = dist/

#---------------------
# Include Sub-Projects
#---------------------
SUB_PROJS   = sclpl sclpl-rdr
CLEAN_PROJS = $(addprefix clean-,$(SUB_PROJS))

#----------------
# Top-Level Tasks
#----------------
.PHONY: $(SUB_PROJS) $(CLEAN_PROJS)

all: $(DIST_DIR) $(SUB_PROJS)

$(SUB_PROJS):
	@$(MAKE) -C src/$@ release

$(DIST_DIR):
	@mkdir -p $(DIST_DIR)

$(CLEAN_PROJS):
	@$(MAKE) -C src/$(subst clean-,,$@) clean

clean: $(CLEAN_PROJS)
	@rm -rf $(DIST_DIR)

