#!/bin/sh

user=$1
backup=$2

if [ -z "$user" -o -z "$backup" ]; then
    echo "Restore a MySQL database."
    echo "Usage: <user> <backup>"
    exit 1
fi

database=`basename "$backup" .sql.bz2`
bzip2 -d -c "$backup" | mysql -u "$user" -p "$database"
