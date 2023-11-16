SHELL    := /bin/sh
CLANG    := clang-17
INCLUDES := -I./include
LDFLAGS  := -lm
CXXFLAGS := \
	-lc++ \
	-std=c++23 \
	-stdlib=libc++ \
	-O2 \
	-g \
	-Wall \
	-Wpedantic \
	-Werror \
	-fmodules \
	-fexperimental-library

ifeq ($(shell uname),Darwin)
	SDK_PATH := $(shell xcrun --show-sdk-path)
	LLVM_DIR := $(shell brew --prefix llvm)
	CLANG    := $(LLVM_DIR)/bin/$(CLANG)
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
SRC_PATHS := $(wildcard $(SRC)/*/*.cpp)
OUT_FILES := $(basename $(SRC_PATHS:$(SRC)/%=%))
OUT_PATHS := $(addprefix $(OUT)/,$(OUT_FILES))
OUT_DIRS  := $(subst $(SRC)/,$(OUT)/,$(SRC_DIRS))

.PHONY: all
all: $(OUT_PATHS)

.PHONY: clean
clean:
	$(RM) -rv $(OUT)

$(OUT):
	mkdir $@

$(OUT_DIRS): $(OUT)
	mkdir $@

$(OUT_PATHS): $(OUT)/%: $(SRC)/%.cpp | $(OUT_DIRS)
	$(CLANG) $(CXXFLAGS) $(INCLUDES) -o $@ $^ $(LDFLAGS)

RUN_TARGETS := $(addprefix run_,$(OUT_FILES))

.PHONY: $(RUN_TARGETS)
$(RUN_TARGETS): run_% : txt/input/% $(OUT)/%
	@$(OUT)/$* < $<

SOLUTIONS    := $(wildcard txt/correct/*/*)
TEST_TARGETS := $(subst txt/correct/,test_,$(SOLUTIONS))

.PHONY: test
test: $(TEST_TARGETS)

.PHONY: $(TEST_TARGETS)
$(TEST_TARGETS): test_% : $(OUT)/% | txt/input/%
	@printf '$*\n'; \
	time_log="$${TMPDIR:=/tmp}/time.tmp"; \
	result=$$(/usr/bin/env time -l -o "$$time_log" make --silent run_$*); \
	expect=$$(cat txt/correct/$*); \
	printf 'result: %s\n' "$$result"; \
	printf 'expect: %s\n' "$$expect"; \
	printf 'time:\n'; \
	cat "$$time_log"; \
	printf '\n'; \
	if [ "$$result" != "$$expect" ]; then \
		exit 1; \
	fi \
