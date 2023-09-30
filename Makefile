SHELL    := /bin/sh
CLANG    := clang++-17
CXXFLAGS := \
	-std=c++23 \
	-stdlib=libc++ \
	-O2 \
	-g \
	-Wall \
	-Wpedantic \
	-Werror \
	-fmodules \
	-fsanitize=address,undefined

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
	$(CLANG) $(CXXFLAGS) -o $@ $^

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
	@printf '$*'; \
	result=$$(time make --silent run_$*); \
	expect=$$(cat txt/correct/$*); \
	printf 'result: %s\n' "$$result"; \
	printf 'expect: %s\n' "$$expect"; \
	printf '\n'; \
	if [[ "$$result" != "$$expect" ]]; then \
		exit 1; \
	fi \
