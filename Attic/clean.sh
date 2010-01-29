#!/bin/sh

if [ `whoami` != "root" ]; then
    sudo "$0" $*
    exit 1
fi

apt-get -qq autoremove
apt-get -qq clean
rm -r -f ~/.cpan/build
rm -r -f ~/.cpan/sources
echo "Done!"
