#!/bin/bash

case "$-" in
*i*)
    INTERACTIVE='x'
;;
esac

# Cygwin helper.
if [ -n "$WINDIR" -a -z "$INTERACTIVE" ]; then
    ls > /dev/null 2>&1
    
    if [ "$?" = "127" ]; then
        export CD=$*
        export HOME=/home/$USERNAME
        exec $SHELL -il
    fi
fi

source /etc/bash_completion 2> /dev/null
EXIT_TRAPS=''

# Disable tilde expansion.
_expand() {
    return 0
}

_have() {
    for NAME; do
        LOCATION=$(which $NAME 2> /dev/null)
        
        if [ -n "$LOCATION" ]; then
            eval "HAVE_$(echo $NAME | tr '[:lower:]-' '[:upper:]_')='$LOCATION'"
            return 0
        fi
    done
    
    [ -n "$INTERACTIVE" ] && echo "* Missing: $@" 1>&2
    return 1
}

if [ -n "$INTERACTIVE" ]; then
    bind 'set completion-ignore-case on'
    bind 'set expand-tilde off'
    bind 'set mark-symlinked-directories on'
    bind '"\e[1;5C": forward-word'              # Ctrl + Right
    bind '"\e[1;5D": backward-word'             # Ctrl + Left
    bind '"\e[3;5~": kill-word'                 # Ctrl + Delete
    bind '"\e[2;5~": backward-kill-word'        # Ctrl + Insert
    bind '"\e[2~": unix-word-rubout'            # Insert
fi

shopt -s cdspell checkwinsize histappend

alias c='cd'
alias -- -='c -'
alias ..='c ..'
alias ...='c ../..'
alias ....='c ../../..'
alias .....='c ../../../..'

alias e='$EDITOR'
alias l='ls -CFXh --color=auto --group-directories-first'
alias ll='l -l'
alias dir='l -lA'

alias sed='sed -r'
alias less='less -x4 -cR'
alias grep='grep -E --color=auto'

# Allow non-breakable spaces (e.g. AltGr + Space).
_have setxkbmap && $NAME -option "nbsp:none"

_have dircolors && eval "$($NAME -b)"
_have lesspipe && eval "$($NAME)"

_have svn && alias \
    sco="$NAME co" \
    sre="$NAME revert" \
    sup="$NAME up"

_have ack-grep ack && alias f="$NAME --sort-files"
_have cpan && alias cpan="sudo PERL_AUTOINSTALL=1 PERL_MM_USE_DEFAULT=1 FTP_PASSIVE=1 $NAME"
_have ksshaskpass ssh-askpass && export SSH_ASKPASS=$LOCATION
_have kwrite nano && export EDITOR=$LOCATION
_have valgrind && alias vg="$NAME --tool=memcheck --leak-check=yes --show-reachable=yes"

export ACK_COLOR_FILENAME='dark blue'
export DISPLAY=:0.0
export GIT_PAGER=cat
export HISTCONTROL=ignoreboth
export PYTHONDONTWRITEBYTECODE=yes
export TRASH="$HOME/.local/share/Trash/files/"

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

PS1_USER_HOST='\u@\h'

if [ "$(uname -o)" = "Cygwin" ]; then
    alias sudo=''
    export CYGWIN=nodosfilewarning
    export TERM=cygwin
    export TEMP=/tmp
    export TMP=$TMP
    export PROMPT_COMMAND="
export HISTFILESIZE=\$((HISTFILESIZE + 1))
export HISTSIZE=\$HISTFILESIZE
$PROMPT_COMMAND
"
    bind '"\e[2;2~": paste-from-clipboard'      # Shift + Insert
    [ -n "$CD" ] && cd "$(cygpath "$CD")" && unset CD
else
    export TERM=xterm
    export PROMPT_COMMAND="
export HISTFILESIZE=\$((\$(history 1 | awk '{print \$1}') + 3))
export HISTSIZE=\$HISTFILESIZE
$PROMPT_COMMAND
"
    if [ "$(stat --format=%i /)" != "2" ]; then
        PS1_USER_HOST="($PS1_USER_HOST)"
        export CHROOT='x'
        [ -n "$INTERACTIVE" ] && echo "* chroot:" $(uname -srmo)
    fi
fi

export PS1="\[\033[4;30;32m\]$PS1_USER_HOST\[\033[00m\]:\[\033[01;34m\]\w\n\\$\[\033[00m\] "

NANO_RC=~/.nanorc

if [ -n "$HAVE_NANO" -a -n "$INTERACTIVE" -a ! -e "$NANO_RC" ]; then
    EXIT_TRAPS="rm ~/.nanorc; $EXIT_TRAPS"
    ls -1 /usr/share/nano/*.nanorc | sed -e 's/(.+)/include "\1"/' > $NANO_RC
    cat << 'TEXT' > $NANO_RC
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

KDE_START_SSH_ADD=~/.kde/Autostart/ssh-add.sh

if [ -z "$KDE_FULL_SESSION" -o ! -e "$KDE_START_SSH_ADD" ]; then
    ssh-add < /dev/null 2> /dev/null
    
    if  [ -n "$KDE_FULL_SESSION" ]; then
        cat << 'TEXT' > $KDE_START_SSH_ADD && chmod +x $KDE_START_SSH_ADD
#!/bin/sh
ssh-add
TEXT
    fi
fi

if [ -n "$INTERACTIVE" ]; then
    CLEANUP=$(($(date +%s) - $(stat --format=%Y ~/.cleanup 2> /dev/null || echo 0)))
    
    if [ "$CLEANUP" -gt "$((14 * 24 * 60 * 60))" ]; then
        echo "* Time to clean up!"
    fi
fi

[ -n "$EXIT_TRAPS" ] && trap "($EXIT_TRAPS)" EXIT

REAL_BASH_SOURCE=$(readlink $BASH_SOURCE)
SHOW_PY=$(dirname $REAL_BASH_SOURCE 2> /dev/null)"/show.py"

if [ -e "$SHOW_PY" ]; then
    alias s=$SHOW_PY
    alias diff='s'
    export GIT_EXTERNAL_DIFF=$SHOW_PY
    export ACK_PAGER=$SHOW_PY
else
    alias s='less'
    _have colordiff && alias diff=$NAME
fi

for BASHRC in $(echo $BASH_SOURCE $REAL_BASH_SOURCE); do
    for BASHRC_CHILD in $(ls -1 $BASHRC.* 2> /dev/null); do
        source $BASHRC_CHILD
        [ -n "$INTERACTIVE" ] && echo "* Loaded: $BASHRC_CHILD"
    done
done

_in_git() {
    (cd "$1" && git symbolic-ref HEAD > /dev/null 2>&1)
}

_in_svn() {
    svn info "$1" > /dev/null 2>&1
}

cleanup() {
    _have apt-get && (sudo $NAME -qq autoremove; sudo $NAME -qq clean)
    perl -i -ne 'print unless $seen{$_}++' $HISTFILE
    sudo rm -rf ~/.cpan/{build,sources}
    touch ~/.cleanup
}

ff() {
    find $@ -a ! -name '*.svn-base'
}

reload() {
    [ -n "$EXIT_TRAPS" ] && eval "($EXIT_TRAPS)"
    exec $SHELL
}

sci() {
    local REPO=$(if [ -z "$1" ]; then echo .; else echo "$1"; fi)
    
    if _in_git $REPO; then
        (cd $REPO && git commit -a)
    elif _in_svn $REPO; then
        svn commit $REPO
    fi
}

sdi() {
    local REPO=$(if [ -z "$1" ]; then echo .; else echo "$1"; fi)
    
    if _in_git $REPO; then
        (cd $REPO && git diff)
    elif _in_svn $REPO; then
        svn diff $REPO
    fi
}

sst() {
    local REPO=$(if [ -z "$1" ]; then echo .; else echo "$1"; fi)
    
    if _in_git $REPO; then
        (cd $REPO && git status)
    elif _in_svn $REPO; then
        svn status $REPO
    fi
}
