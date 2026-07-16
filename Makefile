# Names and Paths
BUILD_DIR    := build
CONFIG_DIR   := config
TOOLS_DIR    := tools
OBJDIFF_DIR  := $(TOOLS_DIR)/objdiff
EXPECTED_DIR := expected
DOCKERFILE   := ./Dockerfile
DOCKER       := $(shell sh -c 'command -v docker || true')
OBJDIFF      := $(OBJDIFF_DIR)/objdiff
PYTHON       := python3

# Detect host OS and architecture
uname_S := $(shell sh -c 'uname -s 2>/dev/null || echo not')
uname_M := $(shell sh -c 'uname -m 2>/dev/null || echo not')

# Tools
# The binary tools in this repo require linux/x86_64 -- if we're on that platform,
# use prebuild Gears and do build directly on the host
ifeq ($(uname_S)-$(uname_M),Linux-x86_64)
	GEARS        := $(TOOLS_DIR)/gears/prebuilt/gears
	NEED_DOCKER := false
	NEED_GEARS := false
else
	# Use the compiled version of Gears
	GEARS        := $(TOOLS_DIR)/gears/target/release/gears
	# flag used below to run build in a docker container
	NEED_DOCKER := true
	# flag used below to trigger gears build automatically
	NEED_GEARS := true
endif

# Settings
ifeq ($(uname_S),Linux)
	NUMPROC ?= $(shell nproc)
endif
ifeq ($(uname_S),Darwin)
	NUMPROC ?= $(shell sysctl -n hw.logicalcpu)
endif

# Rules
default: all

all: build

ifeq ($(NEED_GEARS),true)
# add make targets to build Gears, and add dependencies.
$(GEARS):
	cd $(TOOLS_DIR)/gears && cargo build --release

ifneq ($(NEED_DOCKER), true)
build: $(GEARS)
endif
clean: $(GEARS)
endif

ifeq ($(NEED_DOCKER),true)
# provide a useful error for non-linux/x86_64 platforms
ifeq (,$(DOCKER))
$(error Docker is required for platform $(uname_S)/$(uname_M))
endif

# make target to build the container
docker:
	docker buildx build --platform linux/amd64 . -t ethos:latest --quiet
# build target runs build in the container. note that files get written to the
# host filesystem, not the container filesystem.
build: docker
	docker run --rm -it --platform=linux/amd64 -v$(PWD):/xenogears-decomp ethos:latest /bin/bash -c '. /.venv/bin/activate && make build'
else
build:
	$(MAKE) clean; \
	$(GEARS) matching; \
	grep -q '^ApplyMatrixSV = ' linker/undefined_funcs_auto.field.txt || \
		sed -i '/^ApplyMatrix = /a ApplyMatrixSV = 0x80049D3C;' linker/undefined_funcs_auto.field.txt; \
	grep -q '^g_Heap = ' linker/undefined_syms_auto.field.txt || \
		echo 'g_Heap = 0x80059320;' >> linker/undefined_syms_auto.field.txt; \
	ninja -t clean; \
	ninja -j$(NUMPROC)
endif

check: clean build
	@sha256sum --check $(CONFIG_DIR)/checksum.sha

objdiff-config:
	$(MAKE) clean; \
	$(GEARS) report; \
	grep -q '^ApplyMatrixSV = ' linker/undefined_funcs_auto.field.txt || \
		sed -i '/^ApplyMatrix = /a ApplyMatrixSV = 0x80049D3C;' linker/undefined_funcs_auto.field.txt; \
	grep -q '^g_Heap = ' linker/undefined_syms_auto.field.txt || \
		echo 'g_Heap = 0x80059320;' >> linker/undefined_syms_auto.field.txt; \
	ninja -t clean; \
	ninja -j$(NUMPROC); \
	mkdir -p $(EXPECTED_DIR); \
	rm -rf $(EXPECTED_DIR)/asm; \
	mv build/asm $(EXPECTED_DIR)/asm; \
	$(PYTHON) $(OBJDIFF_DIR)/objdiff_generate.py $(OBJDIFF_DIR)/config.yaml

report: objdiff-config
	@$(OBJDIFF) report generate > $(BUILD_DIR)/progress.json

# Do NOT delete $(EXPECTED_DIR): objdiff baselines live there and are expensive
# to regenerate. gears clean already wipes build artifacts.
clean:
	@$(GEARS) clean

### Settings
.SECONDARY:
# `build` and `check` must be phony: a `build/` output directory would otherwise
# make Make treat the build target as already up-to-date (no-op).
.PHONY: all clean default build check objdiff-config report
SHELL = /bin/bash -e -o pipefail
