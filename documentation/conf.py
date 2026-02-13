# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

extensions = ['sphinx.ext.autosectionlabel', 'breathe']
templates_path = ['_templates']
exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store']
autosectionlabel_prefix_document = True
breathe_domain_by_extension = {'h': 'c'}
#html_theme = 'haiku'
html_static_path = []
latex_elements = {'releasename': 'Version'}
