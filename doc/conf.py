# -*- coding: utf-8 -*-
import sys
import os

version = u'2.7'
release = u'2.7'
project = u'E-MailRelay'

extensions = [
	"myst_parser",
]
myst_disable_syntax = [
	"html_inline",
	"html_block"
]
myst_enable_extensions = [
	"attrs_block" ,
	"attrs_inline" ,
]
suppress_warnings = [
	"myst.xref_missing" ,
]
templates_path = ['_templates']
source_suffix = {
	'.txt': 'markdown',
	'.md': 'markdown',
	'.rst': 'restructuredtext',
}
root_doc = 'contents'
copyright = u'2026, Graeme Walker, SP' + u'DX-License-Identifier: FSFAP '
author = u'Graeme Walker'
language = 'en'
today_fmt = '%Y-%m-%d'
exclude_patterns = [
	'index.md',
]
pygments_style = 'sphinx'
todo_include_todos = False
html_theme = 'alabaster'
html_show_sphinx = False
html_favicon = 'emailrelay-icon-tiny.png'
html_last_updated_fmt = ''
html_show_copyright = True
htmlhelp_basename = 'emailrelaydoc'
highlight_language = 'none'
latex_elements = {}
latex_documents = []
man_pages = []
texinfo_documents = []
smartquotes = False

