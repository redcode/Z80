set noexpandtab
set shiftwidth=8
set tabstop=8
autocmd BufWritePre * %s/\s\+$//e
