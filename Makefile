LLVM_VERSION ?= 18

SHELL ?= /bin/sh
CXX   := clang++-$(LLVM_VERSION)
TIDY  := clang-tidy-$(LLVM_VERSION)

INCLUDES ?= -I./include -I./ndvec
LDFLAGS  ?= -fuse-ld=lld -lm -lc++
CXXFLAGS ?= \
	-std=c++23 \
	-stdlib=libc++ \
	-Wall \
	-Wpedantic \
	-Werror \
	-pthread
SANITIZE :=

ifeq ($(shell uname), Darwin)
	SDK_PATH := $(shell xcrun --show-sdk-path)
	LLVM_DIR := $(shell brew --prefix llvm)
	CXX      := $(LLVM_DIR)/bin/clang-$(LLVM_VERSION)
	TIDY     := $(LLVM_DIR)/bin/clang-tidy
	INCLUDES += \
		-nostdinc++ \
		-nostdlib++ \
		-isysroot $(SDK_PATH) \
		-isystem $(LLVM_DIR)/include/c++/v1
	LDFLAGS  += \
		-L$(LLVM_DIR)/lib/c++ \
		-Wl,-rpath,$(LLVM_DIR)/lib/c++,-syslibroot,$(SDK_PATH)
endif

SRC := src
OUT := out

FAST ?= 0
ifeq ($(FAST), 1)
	OUT_DIR  := $(OUT)/fast
	CXXFLAGS += -O3 -march=native
else
	OUT_DIR  := $(OUT)/debug
	CXXFLAGS += -g -O2
	SANITIZE += -fsanitize=address,undefined
endif

YEARS     := $(subst $(SRC)/,,$(wildcard $(SRC)/20??))
SRC_DIRS  := $(wildcard $(SRC)/*)
OUT_DIRS  := $(subst $(SRC)/,$(OUT_DIR)/,$(SRC_DIRS))
SRC_PATHS := $(wildcard $(SRC)/*/*.cpp)
OUT_FILES := $(basename $(SRC_PATHS:$(SRC)/%=%))
OUT_PATHS := $(addprefix $(OUT_DIR)/,$(OUT_FILES))
OBJ_PATHS := $(addsuffix .o,$(OUT_PATHS))

TESTS      := tests
TEST_SRC   := $(wildcard $(TESTS)/*.cpp)
TEST_FILES := $(addprefix $(OUT_DIR)/,$(basename $(TEST_SRC)))

TEST_OUT_DIR := $(OUT_DIR)/$(TESTS)

.PHONY: all
all: $(OUT_PATHS) $(TEST_FILES)

$(addsuffix /,$(OUT_DIR) $(OUT_DIRS) $(TEST_OUT_DIR)):
	mkdir -p $@

.PHONY: clean
clean:
	$(RM) -rv $(OUT)

.PHONY: fmt
fmt: $(SRC_PATHS) $(wildcard include/*.hpp)
	@clang-format --verbose -i $^


JQ_MAKE_COMPILE_COMMANDS := [inputs|{\
	directory: "$(abspath .)", \
	command: ., \
	file: match("('$(SRC)'[^ ]*)").captures[0].string, \
	output: match("-o ([^ ]+)").captures[0].string \
	}]

$(OUT_DIR)/compile_commands.json: $(OUT_DIR)/
	@$(MAKE) --always-make --dry-run \
		| grep -wE '^\S*clang' \
		| jq -nR '$(JQ_MAKE_COMPILE_COMMANDS)' > $@


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

LINT_TARGETS := $(addprefix lint_,$(OUT_FILES))
.PHONY: $(LINT_TARGETS)
$(LINT_TARGETS): lint_% : $(OUT_DIR)/compile_commands.json $(SRC)/%.cpp
	$(TIDY) --quiet -p $^

.PHONY: lint
lint: $(LINT_TARGETS)


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

$(OBJ_PATHS): $(OUT_DIR)/%.o: $(SRC)/%.cpp | $$(dir $(OUT_DIR)/%)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(OUT_PATHS): $(OUT_DIR)/%: $(OUT_DIR)/%.o
	$(CXX) $(SANITIZE) $< -o $@ $(LDFLAGS)


PERCENT := %
TEST_YEARS := $(subst $(OUT_DIR)/,test_,$(OUT_DIRS))
.PHONY: $(TEST_YEARS)
$(TEST_YEARS): test_% : $$(filter test_%$$(PERCENT),$(QUICK_TEST_TARGETS))
