SHELL    := /bin/sh
CLANG    := clang++-16
CXXFLAGS := \
	-std=c++2b \
	-stdlib=libc++ \
	-O2 \
	-g \
	-Wall \
	-Wpedantic \
	-Werror \
	-fmodules \
	-fsanitize=address,undefined

OUT       := out
SRC_PATHS := $(wildcard src/*/*.cpp)
OUT_FILES := $(basename $(SRC_PATHS:src/%=%))
OUT_PATHS := $(addprefix $(OUT)/,$(OUT_FILES))

.PHONY: all
all: $(OUT_PATHS)

.PHONY: clean
clean:
	rm -rv $(OUT)

$(OUT_PATHS): $(OUT)/%: src/%.cpp
	@mkdir -pv $(dir $@)
	@$(CLANG) $(CXXFLAGS) -o $@ $^

RUN_TARGETS := $(addprefix run,$(OUT_FILES))

.PHONY: $(RUN_TARGETS)
$(RUN_TARGETS): run% : txt/input/% $(OUT)/%
	@$(OUT)/$* < $<

TEST_TARGETS := $(addprefix test,$(OUT_FILES))

.PHONY: test
test: $(TEST_TARGETS)

.PHONY: $(TEST_TARGETS)
$(TEST_TARGETS): test% : $(OUT)/%
	@printf '$*'; \
	result=$$(time make --silent run$*); \
	expect=$$(cat txt/correct/$*); \
	printf 'result: %s\n' "$$result"; \
	printf 'expect: %s\n' "$$expect"; \
	printf '\n'; \
	if [[ "$$result" != "$$expect" ]]; then \
		exit 1; \
	fi \
