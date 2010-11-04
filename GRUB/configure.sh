#!/bin/sh

if [ "$(id -u)" != "0" ]; then
    sudo "$0" $*
    exit
fi

(cd /etc/default/ && [ ! -e grub.bak ] && mv -i grub grub.bak)

cat << CONFIG > /etc/default/grub
GRUB_TIMEOUT=3
GRUB_TERMINAL=console
GRUB_DISABLE_LINUX_RECOVERY=true
CONFIG

cat << CONFIG > /etc/grub.d/01_custom
#!/bin/sh
cat << MENU
set menu_color_normal=white/black
set menu_color_highlight=white/blue
menuentry "Windows" {
    insmod ntfs
    set root=(hd0,3)
    chainloader +1
}
MENU
CONFIG

(cd /etc/grub.d/ && chmod -x * && chmod +x [01][01]*)
update-grub
