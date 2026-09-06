# Root Makefile for Beautiful C++ (docs + kata examples)
#
# Docs: Sphinx in doc/ (see README.md, fabfile.py)
# Katas: kata/example, kata/chatgpt, kata/algorithm, kata/pattern, kata/apr

DOC_DIR       := doc
DOC_HTML      := $(DOC_DIR)/build/html
EXAMPLE_DIR   := kata/example
CHATGPT_DIR   := kata/chatgpt
ALGORITHM_DIR := kata/algorithm
PATTERN_DIR   := kata/pattern
APR_DIR       := kata/apr
BLD           := bld
JOBS          ?= 4

.DEFAULT_GOAL := help

.PHONY: help all doc doc-install doc-clean doc-serve livehtml publish-doc \
        example chatgpt algorithm pattern apr build clean

help:
	@echo "Beautiful C++ — make targets"
	@echo ""
	@echo "  Docs"
	@echo "    make doc-install   Install Sphinx deps (uv sync in doc/)"
	@echo "    make doc           Build HTML docs (cd doc && make html)"
	@echo "    make livehtml      Live rebuild + preview on :8000"
	@echo "    make doc-serve     Serve already-built HTML on :8000"
	@echo "    make doc-clean     Remove Sphinx output"
	@echo "    make publish-doc   Commit docs and push gh-pages subtree (fab publish_doc)"
	@echo ""
	@echo "  Katas"
	@echo "    make example       Build kata/example (Conan + CMake presets)"
	@echo "    make chatgpt       Build kata/chatgpt (Conan + CMake)"
	@echo "    make algorithm     Configure and build kata/algorithm"
	@echo "    make pattern        Configure and build kata/pattern"
	@echo "    make apr           Configure and build kata/apr (APR demos)"
	@echo "    make build          Build all kata projects"
	@echo "    make all            Docs + all katas"
	@echo "    make clean          Remove kata/doc build directories"

all: doc build

# --- documentation (README + fabfile make_doc) ---

doc-install:
	$(MAKE) -C $(DOC_DIR) install

doc:
	$(MAKE) -C $(DOC_DIR) html

livehtml:
	$(MAKE) -C $(DOC_DIR) livehtml

doc-serve:
	$(MAKE) -C $(DOC_DIR) serve

doc-clean:
	$(MAKE) -C $(DOC_DIR) clean-all

# fabfile.py: publish_doc
publish-doc: doc
	touch $(DOC_HTML)/.nojekyll
	git add $(DOC_DIR)
	git commit -m "update doc"
	git subtree push --prefix $(DOC_HTML) origin gh-pages

# --- kata examples ---

example:
	cd $(EXAMPLE_DIR) && ./build.sh

chatgpt:
	cd $(CHATGPT_DIR) && ./build.sh

algorithm:
	cmake -S $(ALGORITHM_DIR) -B $(ALGORITHM_DIR)/$(BLD)
	cmake --build $(ALGORITHM_DIR)/$(BLD) -j$(JOBS)

pattern:
	cmake -S $(PATTERN_DIR) -B $(PATTERN_DIR)/$(BLD)
	cmake --build $(PATTERN_DIR)/$(BLD) -j$(JOBS)

apr:
	$(MAKE) -C $(APR_DIR) build JOBS=$(JOBS)

build: example chatgpt algorithm pattern apr

clean: doc-clean
	rm -rf $(EXAMPLE_DIR)/build $(CHATGPT_DIR)/build \
		$(ALGORITHM_DIR)/$(BLD) $(PATTERN_DIR)/$(BLD) $(APR_DIR)/$(BLD)
