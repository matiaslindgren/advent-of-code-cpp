LLVM_VERSION ?= 17

SHELL ?= /bin/sh
CXX   := clang++-$(LLVM_VERSION)

INCLUDES ?= -I./include
LDFLAGS  ?= -lm -fuse-ld=lld -lc++
CXXFLAGS ?= \
	-std=c++23 \
	-stdlib=libc++ \
	-Wall \
	-Wpedantic \
	-Werror \
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
	CXXFLAGS += -g -O2 -fsanitize=address,undefined --rtlib=compiler-rt
endif

YEARS     := $(subst $(SRC)/,,$(wildcard $(SRC)/20??))
SRC_DIRS  := $(wildcard $(SRC)/*)
OUT_DIRS  := $(subst $(SRC)/,$(OUT_DIR)/,$(SRC_DIRS))
SRC_PATHS := $(wildcard $(SRC)/*/*.cpp)
OUT_FILES := $(basename $(SRC_PATHS:$(SRC)/%=%))
OUT_PATHS := $(addprefix $(OUT_DIR)/,$(OUT_FILES))

TESTS      := tests
TEST_SRC   := $(wildcard $(TESTS)/*.cpp)
TEST_FILES := $(addprefix $(OUT_DIR)/,$(basename $(TEST_SRC)))

TEST_OUT_DIR := $(OUT_DIR)/$(TESTS)

.PHONY: all
all: $(OUT_PATHS) $(TEST_FILES)

$(addsuffix /,$(OUT_DIRS) $(TEST_OUT_DIR)):
	mkdir -p $@

.PHONY: clean
clean:
	$(RM) -rv $(OUT)

.PHONY: fmt
fmt: $(SRC_PATHS)
	@clang-format --verbose -i $^


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


RUN_SOLUTIONS := $(addprefix run_,$(filter $(addsuffix %,$(YEARS)),$(OUT_FILES)))
.PHONY: $(RUN_SOLUTIONS)
$(RUN_SOLUTIONS): run_% : txt/input/% $(OUT_DIR)/%
	$(OUT_DIR)/$* < $<


RUN_TOOLS := $(addprefix run_,$(filter tools%,$(OUT_FILES)))
.PHONY: $(RUN_TOOLS)
$(RUN_TOOLS): run_% : $(OUT_DIR)/%
	@$(OUT_DIR)/$*


$(TEST_FILES): $(TEST_OUT_DIR)/%: $(TESTS)/%.cpp | $(TEST_OUT_DIR)/
	$(CXX) $(CXXFLAGS) $(INCLUDES) $< -o $@ $(LDFLAGS)

RUN_TEST_UTILS := $(addprefix run_,$(notdir $(TEST_FILES)))

.PHONY: test_utils
test_utils: $(RUN_TEST_UTILS)

.PHONY: $(RUN_TEST_UTILS)
$(RUN_TEST_UTILS): run_%: $(TEST_OUT_DIR)/%
	$<

.SECONDEXPANSION:

$(OUT_PATHS): $(OUT_DIR)/%: $(SRC)/%.cpp | $$(dir $(OUT_DIR)/%)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $< -o $@ $(LDFLAGS)

PERCENT := %
TEST_YEARS := $(subst $(OUT_DIR)/,test_,$(OUT_DIRS))
.PHONY: $(TEST_YEARS)
$(TEST_YEARS): test_% : $$(filter test_%$$(PERCENT),$(QUICK_TEST_TARGETS))
