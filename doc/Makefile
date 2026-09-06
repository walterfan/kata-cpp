# Makefile for Sphinx documentation
#
# 现代 C++ 实战开发指南

# You can set these variables from the command line, and also
# from the environment for the first two.
SPHINXOPTS    ?=
UV            ?= uv
SPHINXBUILD   ?= $(UV) run sphinx-build
SOURCEDIR     = source
BUILDDIR      = build

# Put it first so that "make" without argument is like "make help".
help:
	@$(SPHINXBUILD) -M help "$(SOURCEDIR)" "$(BUILDDIR)" $(SPHINXOPTS) $(O)

.PHONY: help Makefile install livehtml clean-all serve dev quick

# Catch-all target: route all unknown targets to Sphinx using the new
# "make mode" option.  $(O) is meant as a shortcut for $(SPHINXOPTS).
%: Makefile
	@$(SPHINXBUILD) -M $@ "$(SOURCEDIR)" "$(BUILDDIR)" $(SPHINXOPTS) $(O)

# Install dependencies into .venv via uv
install:
	$(UV) sync

# Live rebuild on changes (requires sphinx-autobuild)
livehtml:
	$(UV) run sphinx-autobuild "$(SOURCEDIR)" "$(BUILDDIR)/html" $(SPHINXOPTS) $(O) --port 8000

# Clean all build files
clean-all:
	rm -rf $(BUILDDIR)/*

# Serve the built documentation
serve:
	$(UV) run python -m http.server 8000 --directory "$(BUILDDIR)/html"

# Build and serve
dev: html serve

# Quick build (faster, less strict)
quick:
	@$(SPHINXBUILD) -M html "$(SOURCEDIR)" "$(BUILDDIR)" -j auto $(SPHINXOPTS) $(O)

# Check links
linkcheck:
	@$(SPHINXBUILD) -b linkcheck "$(SOURCEDIR)" "$(BUILDDIR)/linkcheck" $(SPHINXOPTS) $(O)

# Generate PDF (requires latexmk)
pdf:
	@$(SPHINXBUILD) -M latexpdf "$(SOURCEDIR)" "$(BUILDDIR)" $(SPHINXOPTS) $(O)

# Generate EPUB
epub:
	@$(SPHINXBUILD) -M epub "$(SOURCEDIR)" "$(BUILDDIR)" $(SPHINXOPTS) $(O)
