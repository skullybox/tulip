VIM Settings
============

color ron
set tabstop =2
set softtabstop =2
set shiftwidth =2
set expandtab

ENV Settings
============

export PS1='\[\e[1;33m\]\u@\h \w ->\n\[\e[1;36m\] \@ \d\$\[\e[m\] '
export PS2='\[\e[1;33m\]\u@\h \w ->\n\[\e[1;36m\] \@ \d\$\[\e[m\] '
alias P='cd ~/Desktop/tulip'
alias SKULL='git config --global user.email "skullybox@gmx.com" && git config --global user.name "skullybox"'

[alias]
  ls = log --color --graph --pretty=format:'%C(white)%h%Creset -%C(red)%d%Creset %s %C(cyan)(%cr) %C(bold red)<%an>%Creset' --abbrev-commit --

