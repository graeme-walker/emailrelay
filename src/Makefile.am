#
## SPDX-FileCopyrightText: 2026 Graeme Walker <graeme_walker@users.sourceforge.net>
## SPDX-License-Identifier: GPL-3.0-or-later
#

SUBDIRS = glib gssl gnet gauth gstore gsmtp gpop win32 gfilters gverifiers main gui
EXTRA_DIST = .clang-tidy .clang-format
CLEANFILES = compile_commands.json

TIDY ?= clang-tidy

.PHONY: cdb
cdb: compile_commands.json

compile_commands.json: Makefile
	@chmod +x $(top_srcdir)/libexec/make2cdb || true
	$(top_srcdir)/libexec/make2cdb > $@.tmp && mv $@.tmp $@

.PHONY: tidy
tidy: compile_commands.json
	@chmod +x $(top_srcdir)/libexec/make2cdb || true
	@touch $(top_srcdir)/gconfig_defs.h
	$(top_srcdir)/libexec/make2cdb --list | xargs -- "$(TIDY)" -p . || true

.PHONY: format
format:
	@chmod +x $(top_srcdir)/libexec/make-format || true
	$(top_srcdir)/libexec/make-format -f "$(FORMAT)" -d $(top_srcdir)

