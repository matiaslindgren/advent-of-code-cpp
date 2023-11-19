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

SOLUTIONS    := $(wildcard txt/correct/*/*)
TEST_TARGETS := $(subst txt/correct/,test_,$(SOLUTIONS))

.PHONY: test
test: $(TEST_TARGETS)

.PHONY: fmt
fmt:
	find . -type f -name '*.cpp*' -exec clang-format -i {} \;

.PHONY: $(TEST_TARGETS)
$(TEST_TARGETS): test_% : $(OUT)/% | txt/input/%
	@./test_one.bash $*

$(MOD_OUT_PATHS): $(OUT)/$(MODULES)/%.pcm: $(SRC)/$(MODULES)/%.cppm | $(OUT)/$(MODULES)/
	$(CLANG) $(CXXFLAGS) $(INCLUDES) $< --precompile -o $@

.SECONDEXPANSION:

$(OUT_PATHS): $(OUT)/%: $(SRC)/%.cpp $(MOD_OUT_PATHS) | $$(dir $(OUT)/%)
	$(CLANG) $(CXXFLAGS) $(INCLUDES) $< -fprebuilt-module-path=$(OUT)/$(MODULES)/ $(MOD_OUT_PATHS) -o $@ $(LDFLAGS)

PERCENT := %
TEST_YEARS := $(subst $(OUT)/,test_,$(OUT_DIRS))
.PHONY: $(TEST_YEARS)
$(TEST_YEARS): test_% : $$(filter test_%$$(PERCENT),$(TEST_TARGETS))
