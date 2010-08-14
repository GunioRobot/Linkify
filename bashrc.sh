#!/bin/bash

[ -z "$PS1" ] && exit
[ -n `which dircolors` ] && eval "$(dircolors -b)"
[ -n `which lesspipe` ] && eval "$(lesspipe)"
[ -f /etc/bash_completion ] && source /etc/bash_completion

shopt -s cdspell checkwinsize histappend
bind 'set completion-ignore-case on'
bind 'set expand-tilde off'
bind 'set mark-symlinked-directories on'

alias -- -='cd -'
alias ..='cd ..'
alias ...='cd ../..'

alias l='ls -CF'
alias ll='ls -lA'
alias ls='\ls -Xh --color=auto --group-directories-first'
alias dir='ls -l'

alias vg='valgrind --tool=memcheck --leak-check=yes --show-reachable=yes'
alias diff=colordiff
alias less='\less -R'
alias nano='TERM=xterm \nano'

alias ack='ack-grep --sort-files'
alias grep='\grep -E --color=auto'
alias igrep='grep -i'
alias rgrep='grep -r'

export ACK_COLOR_FILENAME='dark blue'
export EDITOR=$(which kwrite || which nano)
export HISTCONTROL=ignoreboth
export PS1='\[\033[4;30;32m\]\u@\h\[\033[00m\]:\[\033[01;34m\]\w\[\033[00m\]\n\$ '
export TERM=xterm-color

# Remove bright colors.
export LS_COLORS=$(echo $LS_COLORS | sed -r 's/=01;/=30;/g')

# Sync history session to file and set xterm title.
export PROMPT_COMMAND='
history -a
unset -v HISTSIZE HISTFILESIZE
echo -ne "\033]0;${USER}@${HOSTNAME}: ${PWD/$HOME/~}\007"
'

TRAPS=''

# Install temporary Nano configuration.
if [ ! -e ~/.nanorc ]; then
    cat << 'TEXT' > ~/.nanorc && TRAPS="rm ~/.nanorc; $TRAPS"
include "/usr/share/nano/asm.nanorc"
include "/usr/share/nano/c.nanorc"
include "/usr/share/nano/html.nanorc"
include "/usr/share/nano/java.nanorc"
include "/usr/share/nano/man.nanorc"
include "/usr/share/nano/nanorc.nanorc"
include "/usr/share/nano/patch.nanorc"
include "/usr/share/nano/perl.nanorc"
include "/usr/share/nano/python.nanorc"
include "/usr/share/nano/ruby.nanorc"
include "/usr/share/nano/sh.nanorc"

set autoindent
set const
set morespace
set mouse
set noconvert
set nonewlines
set nowrap
set smarthome
set smooth
set suspend
set tabsize 4
set tabstospaces
TEXT
fi

# Install temporary colored output GCC aliases.
if [ ! -d ~/.bin ]; then
    mkdir ~/.bin && cd $_
    ln -s `which colorgcc` gcc
    ln -s `which colorgcc` g++
    TRAPS="rm -r ~/.bin; $TRAPS"
    cd ..
fi

[ -d ~/.bin ] && export PATH="~/.bin:$PATH"
[ -n "$TRAPS" ] && trap "($TRAPS)" EXIT

cleanup() {
    sudo apt-get -qq autoremove
    sudo apt-get -qq clean
    rm -rf ~/.cpan/build
    rm -rf ~/.cpan/sources
}

reload() {
    [ -n "$TRAPS" ] && eval "($TRAPS)"
    exec $SHELL
}

# Disable tilde expansion.
_expand() {
    return
}
