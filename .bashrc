#!/bin/bash

case "$-" in
*i*)
    INTERACTIVE=x
;;
esac

# Cygwin helper.
if [ -n "$WINDIR" -a -z "$INTERACTIVE" ]; then
    ls > /dev/null 2>&1
    
    if [ "$?" = '127' ]; then
        export CD=$@
        export HOME="/home/$USERNAME"
        exec $SHELL -il
    fi
fi

source /etc/bash_completion 2> /dev/null

# Disable tilde expansion only.
_expand() {
    eval cur=$cur
}

_have() {
    for NAME; do
        LOCATION=$(which $NAME 2> /dev/null)
        
        if [ -n "$LOCATION" ]; then
            eval "HAVE_$(echo $NAME | tr '[:lower:]-' '[:upper:]_')='$LOCATION'"
            return 0
        fi
    done
    
    [ -n "$INTERACTIVE" ] && echo "* Missing: $@" >&2
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
alias grep='grep -E --color=auto'

# Allow non-breakable spaces (e.g. AltGr + Space).
_have setxkbmap && $NAME -option 'nbsp:none'

_have dircolors && eval "$($NAME -b)"
_have lesspipe && eval "$($NAME)"

_have ack-grep ack && alias f="$NAME --sort-files"
_have cpan && alias cpan="PERL_AUTOINSTALL=1 PERL_MM_USE_DEFAULT=1 FTP_PASSIVE=1 $NAME"
_have ksshaskpass ssh-askpass && export SSH_ASKPASS=$LOCATION
_have kwrite nano && export EDITOR=$LOCATION

export ACK_COLOR_FILENAME='dark blue'
export DISPLAY=:0.0
export HISTCONTROL=ignoreboth
export LESS='-x4 -cR'
export PYTHONDONTWRITEBYTECODE=x

# Remove bright colors.
export LS_COLORS=$(echo $LS_COLORS | sed -e 's/=01;/=30;/g')

# Save history session to file and set xterm title.
export PROMPT_COMMAND='
history -a
echo -ne "\033]0;${USER}@${HOSTNAME}: ${PWD/$HOME/~}\007"
'

ps1_user_host='\u@\h'

if [ "$(uname -o)" = 'Cygwin' ]; then
    export CYGWIN=nodosfilewarning
    export TERM=cygwin
    export TEMP=/tmp
    export TMP='$TMP'
    
    bind '"\e[2;2~": paste-from-clipboard'      # Shift + Insert
    [ -n "$CD" ] && cd "$(cygpath "$CD")" && unset CD
else
    export TERM=xterm
    
    if [ "$(stat --format=%i /)" != '2' ]; then
        ps1_user_host="($ps1_user_host)"
        export CHROOT=x
        [ -n "$INTERACTIVE" ] && echo "* chroot: $(uname -srmo)"
    fi
fi

if _have git; then
    _git_branch() {
        local path=$(git symbolic-ref HEAD 2> /dev/null)
        [ -n "$path" ] && echo -e "\033[00m:\033[0;33m${path#refs/heads/}"
    }
    
    git config --global color.ui auto
    git config --global push.default tracking
    
    ps1_user_host="$ps1_user_host\$(_git_branch)"
fi

export PS1="\[\033[4;30;32m\]$ps1_user_host\[\033[00m\]:\[\033[01;34m\]\w\n\\$\[\033[00m\] "
unset ps1_user_host

nano_rc=~/.nanorc

if [ -n "$HAVE_NANO" -a -n "$INTERACTIVE" -a ! -e "$nano_rc" ]; then
    ls -1 /usr/share/nano/*.nanorc | sed -e 's/(.+)/include "\1"/' > $nano_rc
    cat << 'TEXT' >> $nano_rc
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

unset nano_rc
kde_start_ssh_add=~/.kde/Autostart/ssh-add.sh

if [ -z "$KDE_FULL_SESSION" -o ! -e "$kde_start_ssh_add" ]; then
    ssh-add < /dev/null 2> /dev/null
    
    if  [ -n "$KDE_FULL_SESSION" ]; then
        cat << 'TEXT' > $kde_start_ssh_add && chmod +x $kde_start_ssh_add
#!/bin/sh
ssh-add
TEXT
    fi
fi

unset kde_start_ssh_add
show_py="$(dirname $(readlink $BASH_SOURCE) 2> /dev/null)/show.py"

if [ -e "$show_py" ]; then
    alias s=$show_py
    alias diff='s'
    export ACK_PAGER=$show_py
    export GIT_EXTERNAL_DIFF=$show_py
    export GIT_PAGER=$show_py
else
    alias s='less'
    _have colordiff && alias diff=$NAME
fi

unset show_py

for bashrc_child in $(ls -1 $BASH_SOURCE.* 2> /dev/null); do
    source $bashrc_child
    [ -n "$INTERACTIVE" ] && echo "* Loaded: $bashrc_child"
done

unset bashrc_child

_in_git() {
    git symbolic-ref HEAD > /dev/null 2>&1
}

_in_scm() {
    echo "* SCM? $(pwd)" >&2
}

_in_svn() {
    svn info > /dev/null 2>&1
}

cleanup() {
    _have apt-get && (sudo $NAME -qq autoremove; sudo $NAME -qq clean)
    perl -i -ne 'print unless $seen{$_}++' $HISTFILE
    rm -rf ~/.cpan/{build,sources}
}

ff() {
    find $@ -a ! -name '*.svn-base'
}

reload() {
    exec $SHELL
}

sci() {
    if _in_git; then
        if  [ -z "$@" ]; then
            git commit -a
        else
            git commit $@
        fi
    elif _in_svn; then
        svn commit $@
    else
        _in_scm
    fi
}

sdi() {
    if _in_git; then
        git diff $@
    elif _in_svn; then
        svn diff $@
    else
        _in_scm
    fi
}

sre() {
    if _in_git; then
        git checkout $@
    elif _in_svn; then
        svn revert $@
    else
        _in_scm
    fi
}

sst() {
    if _in_git; then
        git status $@
    elif _in_svn; then
        svn status $@
    else
        _in_scm
    fi
}

sup() {
    if _in_git; then
        git pull $@
    elif _in_svn; then
        svn update $@
    else
        _in_scm
    fi
}
