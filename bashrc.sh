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

# Disable tilde expansion.
_expand() {
    return 0
}

_have() {
    for NAME; do
        LOCATION=$(which $NAME 2>/dev/null)
        
        if [ -n "$LOCATION" ]; then
            eval "HAVE_$(echo $NAME | tr '[:lower:]-' '[:upper:]_')='$LOCATION'"
            return 0
        fi
    done
    
    [ -z "$INTERACTIVE" ] && echo "Missing: $*" 1>&2
    return 1
}

if [ -n "$INTERACTIVE" ]; then
    bind 'set completion-ignore-case on'
    bind 'set expand-tilde off'
    bind 'set mark-symlinked-directories on'
    bind '"\e[1;5C": forward-word'
    bind '"\e[1;5D": backward-word'
fi

shopt -s cdspell checkwinsize histappend

alias -- -='cd -'
alias ..='cd ..'
alias ...='cd ../..'
alias ....='cd ../../..'
alias .....='cd ../../../..'

alias l='ls -CFXh --color=auto --group-directories-first'
alias ll='l -l'
alias dir='l -lA'

alias sed='sed -r'
alias less='less -R'
alias grep='grep -E --color=auto'

_have dircolors && eval "$($NAME -b)"
_have lesspipe && eval "$($NAME)"

# Cache for "idiff".
_have kompare meld kdiff3

_have ack-grep ack && alias \
    ack="$NAME --sort-files" \
    ack0='ack -l --print0' \
    ack.="xargs -0 $LOCATION -l --print0 --sort-files" \
    0ack="xargs -0 $LOCATION --sort-files"

_have colordiff && alias diff=$NAME
_have colorgcc && alias gcc=$NAME g++=$NAME
_have kwrite && export EDITOR=$LOCATION
_have nano && [ -z "$HAVE_KWRITE" ] && export EDITOR=$LOCATION

_have source-highlight && s() {
    $HAVE_SOURCE_HIGHLIGHT --failsafe -n -t 4 -f esc -o STDOUT $@ | less
}

_have valgrind \
    && alias vg="$NAME --tool=memcheck --leak-check=yes --show-reachable=yes"

export ACK_COLOR_FILENAME='dark blue'
export HISTCONTROL=ignoreboth
export PS1='\[\033[4;30;32m\]\u@\h\[\033[00m\]:\[\033[01;34m\]\w\n\$\[\033[00m\] '

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
    export CYGWIN=nodosfilewarning
    export TERM=cygwin
    export TEMP=/tmp
    export TMP=$TMP
    export PROMPT_COMMAND="
export HISTFILESIZE=\$((HISTFILESIZE + 1))
export HISTSIZE=\$HISTFILESIZE
$PROMPT_COMMAND
"
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
        echo "* chroot:" $(uname -srmo)
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

CLEANUP=$(($(date +%s) - $(stat --format=%Y ~/.cleanup 2>/dev/null || echo 0)))

if [ "$CLEANUP" -gt "$((14 * 24 * 60 * 60))" ]; then
    echo "* Time to clean up!"
fi

[ -n "$EXIT_TRAPS" ] && trap "($EXIT_TRAPS)" EXIT

cleanup() {
    _have apt-get && (sudo $NAME -qq autoremove; sudo $NAME -qq clean)
    perl -i -ne 'print unless $seen{$_}++' $HISTFILE
    rm -rf ~/.cpan/{build,sources}
    touch ~/.cleanup
}

# Interactive "diff".
idiff() {
    if [ -n "$HAVE_KOMPARE" ]; then
        $HAVE_KOMPARE -c "$1" "$2"
    elif [ -n "$HAVE_MELD" ]; then
        $HAVE_MELD "$1" "$2"
    elif [ -n "$HAVE_KDIFF3" ]; then
        $HAVE_KDIFF3 "$1" "$2"
    else
        diff "$1" "$2" | less
    fi
}

reload() {
    [ -n "$EXIT_TRAPS" ] && eval "($EXIT_TRAPS)"
    exec $SHELL
}
