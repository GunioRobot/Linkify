#!/bin/bash

case "$-" in
*i*)
    INTERACTIVE='x'
;;
esac

# Cygwin helper.
if [ -n "$WINDIR" -a -z "$INTERACTIVE" ]; then
    ls 2>/dev/null 1>&2
    
    if [ "$?" = "127" ]; then
        export CD=$*
        export HOME=/home/$USERNAME
        exec $SHELL -il
    fi
fi

test -f /etc/bash_completion && source $_
EXIT_TRAPS=''

cleanup() {
    _have apt-get && (sudo $NAME -qq autoremove; sudo $NAME -qq clean)
    perl -i -ne 'print unless $seen{$_}++' $HISTFILE
    rm -rf ~/.cpan/{build,sources}
}

reload() {
    [ -n "$EXIT_TRAPS" ] && eval "($EXIT_TRAPS)"
    exec $SHELL
}

# Disable tilde expansion.
_expand() {
    return 0
}

_have() {
    for NAME; do
        LOCATION=$(which $NAME 2>/dev/null)
        
        if [ -n "$LOCATION" ]; then
            eval "HAVE_$(echo $NAME | tr '[:lower:]-' '[:upper:]_')='x'"
            return 0
        fi
    done
    
    [ -z "$INTERACTIVE" ] && echo "Missing: $*" 1>&2
    return 1
}

shopt -s cdspell checkwinsize histappend
bind 'set completion-ignore-case on'
bind 'set expand-tilde off'
bind 'set mark-symlinked-directories on'

alias -- -='cd -'
alias ..='cd ..'
alias ...='cd ../..'

alias l='ls -CF'
alias ll='ls -lA'
alias ls='ls -Xh --color=auto --group-directories-first'
alias dir='ls -l'

alias sed='sed -r'
alias less='less -R'
alias grep='grep -E --color=auto'

_have dircolors && eval "$($NAME -b)"
_have lesspipe && eval "$($NAME)"

_have ack-grep ack && alias ack="$NAME --sort-files"
_have colordiff && alias diff=$NAME
_have colorgcc && alias gcc=$NAME && alias g++=$NAME
_have kwrite && export EDITOR=$LOCATION
_have nano && [ -z "$HAVE_KWRITE" ] && export EDITOR=$LOCATION
_have valgrind && alias vg="$NAME --tool=memcheck --leak-check=yes --show-reachable=yes"

export ACK_COLOR_FILENAME='dark blue'
export HISTCONTROL=ignoreboth
export PS1='\[\033[4;30;32m\]\u@\h\[\033[00m\]:\[\033[01;34m\]\w\[\033[00m\]\n\$ '

# Remove bright colors.
export LS_COLORS=$(echo $LS_COLORS | sed -e 's/=01;/=30;/g')

# Save history session to file and set xterm title.
export PROMPT_COMMAND='
history -a
echo -ne "\033]0;${USER}@${HOSTNAME}: ${PWD/$HOME/~}\007"
'

if [ -n "$HISTFILE" ]; then
    export HISTFILESIZE=$(($(wc -l $HISTFILE | cut -d ' ' -f1) + 1))
    export HISTSIZE=$HISTFILESIZE
fi

if [ "$(uname -o)" = "Cygwin" ]; then
    export TERM=cygwin
    export TEMP=/tmp
    export TMP=$TMP
    export PROMPT_COMMAND="
export HISTFILESIZE=\$((HISTFILESIZE + 1))
export HISTSIZE=\$HISTFILESIZE
$PROMPT_COMMAND
"
    bind '"\e[1;5C": forward-word'
    bind '"\e[1;5D": backward-word'
    bind '"\e[2;2~": paste-from-clipboard'
    [ -n "$CD" ] && cd "$(cygpath "$CD")" && unset CD
else
    export TERM=xterm
    export PROMPT_COMMAND="
export HISTFILESIZE=\$((\$(history 1 | awk '{print \$1}') + 3))
export HISTSIZE=\$HISTFILESIZE
$PROMPT_COMMAND
"
    if [ "$(stat --format=%i /)" != "2" ]; then
        echo "chroot:" $(uname -srmo)
        umask 0002
    fi
fi

if [ -n "$HAVE_NANO" -a -n "$INTERACTIVE" -a ! -e ~/.nanorc ]; then
    EXIT_TRAPS="rm ~/.nanorc; $EXIT_TRAPS"
    ls -1 /usr/share/nano/*.nanorc | sed -e 's/(.+)/include "\1"/' > ~/.nanorc
    cat << 'TEXT' >> ~/.nanorc
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

[ -n "$EXIT_TRAPS" ] && trap "($EXIT_TRAPS)" EXIT
