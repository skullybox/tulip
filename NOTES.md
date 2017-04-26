DEVELOPMENT ENVIRONMENT SETUP

VIM Settings
============

set number
syntax on
color elflord
set tabstop =2
set softtabstop =2
set shiftwidth =2
set expandtab
set autoindent
autocmd VimResized * wincmd =

TMUX Settings
============
# theme
set -g status-bg cyan
set -g status-fg black
set -g window-status-current-bg black
set -g window-status-current-fg cyan

# Shift arrow to switch windows
bind -n S-Left  previous-window
bind -n S-Right next-window

# No delay for escape key press
set -sg escape-time 0



GIT Settings
============

[user]
	name = 
	email = 
[alias]
	ls = log --color --graph --pretty=format:'%C(yellow)%h%Creset -%C(red)%d%Creset %s %Cgreen(%cr) %C(bold blue)<%an>%Creset' --abbrev-commit


Prompt Settings
============

LIGHT="$(tput setaf 8)"
FUSIA="$(tput setaf 5)"
RESET="$(tput sgr0)"

export PS1='${LIGHT}\u@\h \w${RESET} ->\n ${FUSIA}\d \@ ${RESET}\$ '
export PS2='${LIGHT}\u@\h \w${RESET} ->\n ${FUSIA}\d \@ ${RESET}\$ '
alias SKULL='git config user.email "skullybox@gmx.com" && git config user.name "skullybox"'#


