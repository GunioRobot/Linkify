#!/bin/bash

# TODO: Don't resolve symbolic links.
# TODO: Use unlimited history.

if [ -z "$PS1" ]; then
    echo . $0
    exit
else
    # Install unless running interactively or read as a configuration file.
    if [ "$BASH_ARGV" != "$HOME/.bashrc" ]; then
        cp -bv $BASH_ARGV ~/.bashrc
        chmod -x ~/.bashrc
    fi
fi

[ -n `which dircolors` ] && eval "$(dircolors -b)"
[ -n `which lesspipe` ] && eval "$(lesspipe)"
[ -f /etc/bash_completion ] && source /etc/bash_completion

cleanup() {
    sudo apt-get -qq autoremove
    sudo apt-get -qq clean
    rm -rf ~/.cpan/build
    rm -rf ~/.cpan/sources
}

reload() {
    exec $SHELL
}

# Disable tilde expansion.
_expand() {
    return
}

shopt -s cdspell checkwinsize histappend
bind 'set completion-ignore-case on'
bind 'set expand-tilde off'

alias -- -='cd -'
alias ..='cd ..'
alias ...='cd ../..'
alias l='ls -CF'
alias ll='ls -lA'
alias ls='ls --color=auto'
alias dir='ls -l'
alias cd='cd -L'
alias vg='valgrind --tool=memcheck --leak-check=yes --show-reachable=yes'
alias nano='nano -w'
alias grep='grep --color=auto'
alias fgrep='fgrep --color=auto'
alias egrep='egrep --color=auto'

export EDITOR=$(which kwrite || echo $(which nano) -w)
export HISTCONTROL=ignoreboth
export PS1='\[\033[30;32m\]\u@\h\[\033[00m\]:\[\033[01;34m\]\w\[\033[00m\]\n\$ '
export TERM=xterm-color

# Flush history session and set xterm title.
export PROMPT_COMMAND='
history -a
unset -v HISTSIZE HISTFILESIZE
echo -ne "\033]0;${USER}@${HOSTNAME}: ${PWD/$HOME/~}\007"
'
