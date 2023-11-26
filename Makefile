SHELL    := /bin/sh
CLANG    := clang++-17

INCLUDES :=
LDFLAGS  := -lm -fuse-ld=lld -lc++
CXXFLAGS := \
	-std=c++23 \
	-stdlib=libc++ \
	-O2 \
	-Wall \
	-Wpedantic \
	-Werror \
	-fmodules

ifeq ($(shell uname),Darwin)
	SDK_PATH := $(shell xcrun --show-sdk-path)
	LLVM_DIR := $(shell brew --prefix llvm)
	CLANG    := $(LLVM_DIR)/bin/clang-17
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

SRC       := src
OUT       := out
SRC_DIRS  := $(wildcard $(SRC)/*)
OUT_DIRS  := $(subst $(SRC)/,$(OUT)/,$(SRC_DIRS))
SRC_PATHS := $(wildcard $(SRC)/*/*.cpp)
OUT_FILES := $(basename $(SRC_PATHS:$(SRC)/%=%))
OUT_PATHS := $(addprefix $(OUT)/,$(OUT_FILES))

MODULES       := modules
MOD_SRC_PATHS := $(wildcard $(SRC)/$(MODULES)/*.cppm)
MOD_OUT_PATHS := $(subst .cppm,.pcm,$(subst $(SRC)/,$(OUT)/,$(MOD_SRC_PATHS)))
MOD_OUT_FILES := $(notdir $(MOD_OUT_PATHS))

.PHONY: all
all: $(OUT_PATHS)

.PHONY: clean
clean:
	$(RM) -rv $(OUT)

$(OUT)/:
	mkdir $@

$(addsuffix /,$(OUT_DIRS)): | $(OUT)/
	mkdir $@

RUN_TARGETS := $(addprefix run_,$(OUT_FILES))

.PHONY: $(RUN_TARGETS)
$(RUN_TARGETS): run_% : txt/input/% $(OUT)/%
	@$(OUT)/$* < $<

.PHONY: fmt
fmt:
	@find . -type f -name '*.cpp*' -exec clang-format --verbose -i {} \;

SOLUTIONS            := $(wildcard txt/correct/*/*)
TEST_QUICK_TARGETS   := $(subst txt/correct/,test_,$(SOLUTIONS))
TEST_VERBOSE_TARGETS := $(subst txt/correct/,test_verbose_,$(SOLUTIONS))

.PHONY: test
test: $(TEST_QUICK_TARGETS)

.PHONY: $(TEST_QUICK_TARGETS)
$(TEST_QUICK_TARGETS): test_% : txt/input/% $(OUT)/%
	@printf '%s\n' '$*'; $(OUT)/$* < $< | cmp - txt/correct/$*

.PHONY: test_verbose
test_verbose: $(TEST_VERBOSE_TARGETS)

.PHONY: $(TEST_VERBOSE_TARGETS)
$(TEST_VERBOSE_TARGETS): test_verbose_% : $(OUT)/% | txt/input/%
	@./test_one_verbose.bash $*

$(MOD_OUT_PATHS): $(OUT)/$(MODULES)/%.pcm: $(SRC)/$(MODULES)/%.cppm | $(OUT)/$(MODULES)/
	$(CLANG) $(CXXFLAGS) $(INCLUDES) $< --precompile -o $@

.SECONDEXPANSION:

$(OUT_PATHS): $(OUT)/%: $(SRC)/%.cpp $(MOD_OUT_PATHS) | $$(dir $(OUT)/%)
	$(CLANG) $(CXXFLAGS) $(INCLUDES) $< -fprebuilt-module-path=$(OUT)/$(MODULES)/ $(MOD_OUT_PATHS) -o $@ $(LDFLAGS)

PERCENT := %
TEST_YEARS := $(subst $(OUT)/,test_,$(OUT_DIRS))
.PHONY: $(TEST_YEARS)
$(TEST_YEARS): test_% : $$(filter test_%$$(PERCENT),$(TEST_QUICK_TARGETS))
