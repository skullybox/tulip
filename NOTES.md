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
	ls = log --color --graph --pretty=format:'%C(white)%h%Creset -%C(green)%d%Creset %C(magenta)%s %C(white)(%cr) %C(white)<%an>%Creset' --abbrev-commit


Prompt Settings
============

LIGHT="$(tput setaf 8)"
FUSIA="$(tput setaf 5)"
RESET="$(tput sgr0)"

export PS1='${LIGHT}\u@\h \w${RESET} ->\n${FUSIA}\d \@ ${RESET}\n---\n\$ '
export PS2='${LIGHT}\u@\h \w${RESET} ->\n${FUSIA}\d \@ ${RESET}\n---\n\$ '
alias SKULL='git config user.email "skullybox@gmx.com" && git config user.name "skullybox"'#


MISC. NOTES
============
--
RSA
--
p,q = primes
e = 3
d = decryption key
n = pq
T = totient
M = message
C = cipher

public = e,n
private = d

steps: mult, mod, exponentiate

enc: M^e mod(n) = C
dec: c^d mod(n) = M

selecting a large prime requires some testing to determine likelyhood of
prime. use fermat's theorm. 
select prime and test with A in range of 1 < A < prime -1

A^(prime-1) != 1 mod(prime) (likely prime)



ENV Settings
============

curl -sLf https://spacevim.org/install.sh | bash
curl -fsSL https://raw.githubusercontent.com/robbyrussell/oh-my-zsh/master/tools/install.sh | bash
curl -sSL https://get.rvm.io | bash -s stable




