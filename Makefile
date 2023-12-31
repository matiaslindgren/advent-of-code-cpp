SHELL ?= /bin/sh
CXX   := clang++-17

INCLUDES ?=
LDFLAGS  ?= -lm -fuse-ld=lld -lc++
CXXFLAGS ?= \
	-std=c++23 \
	-stdlib=libc++ \
	-Wall \
	-Wpedantic \
	-Werror \
	-fmodules \
	-pthread

ifeq ($(shell uname), Darwin)
	SDK_PATH := $(shell xcrun --show-sdk-path)
	LLVM_DIR := $(shell brew --prefix llvm)
	CXX      := $(LLVM_DIR)/bin/clang-17
	LDFLAGS  := \
		$(LDFLAGS) \
		-L$(LLVM_DIR)/lib/c++ \
		-Wl,-rpath,$(LLVM_DIR)/lib/c++,-syslibroot,$(SDK_PATH)
	INCLUDES += \
		-nostdinc++ \
		-nostdlib++ \
		-isysroot $(SDK_PATH) \
		-isystem $(LLVM_DIR)/include/c++/v1
endif

SRC := src
OUT := out

FAST ?= 0
ifeq ($(FAST), 1)
	OUT_DIR  := $(OUT)/fast
	CXXFLAGS += -O3 -march=native
else
	OUT_DIR  := $(OUT)/debug
	CXXFLAGS += -g -O2 -fsanitize=address,undefined
endif

SRC_DIRS  := $(wildcard $(SRC)/*)
OUT_DIRS  := $(subst $(SRC)/,$(OUT_DIR)/,$(SRC_DIRS))
SRC_PATHS := $(wildcard $(SRC)/*/*.cpp)
OUT_FILES := $(basename $(SRC_PATHS:$(SRC)/%=%))
OUT_PATHS := $(addprefix $(OUT_DIR)/,$(OUT_FILES))

MODULES       := modules
MOD_SRC_PATHS := $(wildcard $(SRC)/$(MODULES)/*.cppm)
MOD_OUT_PATHS := $(subst .cppm,.pcm,$(subst $(SRC)/,$(OUT_DIR)/,$(MOD_SRC_PATHS)))

.PHONY: all
all: $(OUT_PATHS)

$(addsuffix /,$(OUT_DIRS)):
	mkdir -p $@

.PHONY: clean
clean:
	$(RM) -rv $(OUT)

.PHONY: fmt
fmt: $(SRC_PATHS) $(MOD_SRC_PATHS)
	@clang-format --verbose -i $^


$(MOD_OUT_PATHS): $(OUT_DIR)/$(MODULES)/%.pcm: $(SRC)/$(MODULES)/%.cppm | $(OUT_DIR)/$(MODULES)/
	$(CXX) $(CXXFLAGS) $(INCLUDES) $< --precompile -o $@


RUN_TARGETS := $(addprefix run_,$(OUT_FILES))

.PHONY: $(RUN_TARGETS)
$(RUN_TARGETS): run_% : txt/input/% $(OUT_DIR)/%
	$(OUT_DIR)/$* < $<


SOLUTIONS            := $(wildcard txt/correct/*/*)
QUICK_TEST_TARGETS   := $(subst txt/correct/,test_,$(SOLUTIONS))
VERBOSE_TEST_TARGETS := $(subst txt/correct/,test_verbose_,$(SOLUTIONS))

.PHONY: test
test: $(QUICK_TEST_TARGETS)

.PHONY: $(QUICK_TEST_TARGETS)
$(QUICK_TEST_TARGETS): test_% : txt/input/% $(OUT_DIR)/%
	@printf '%s\n' '$*'; $(OUT_DIR)/$* < $< | cmp - txt/correct/$*

.PHONY: test_verbose
test_verbose: $(VERBOSE_TEST_TARGETS)

.PHONY: $(VERBOSE_TEST_TARGETS)
$(VERBOSE_TEST_TARGETS): test_verbose_% : $(OUT_DIR)/% txt/input/% txt/correct/%
	@./scripts/test_one_verbose.bash $^


.PHONY: run_sysinfo
run_sysinfo: $(OUT_DIR)/tools/sysinfo
	@$^

.SECONDEXPANSION:

$(OUT_PATHS): $(OUT_DIR)/%: $(SRC)/%.cpp $(MOD_OUT_PATHS) | $$(dir $(OUT_DIR)/%)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $< -fprebuilt-module-path=$(OUT_DIR)/$(MODULES)/ $(MOD_OUT_PATHS) -o $@ $(LDFLAGS)

PERCENT := %
TEST_YEARS := $(subst $(OUT_DIR)/,test_,$(OUT_DIRS))
.PHONY: $(TEST_YEARS)
$(TEST_YEARS): test_% : $$(filter test_%$$(PERCENT),$(QUICK_TEST_TARGETS))
