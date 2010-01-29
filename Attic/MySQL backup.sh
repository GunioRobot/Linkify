#!/bin/sh

user=$1
database=$2

if [ -z "$user" -o -z "$database" ]; then
    echo "Backup a MySQL database."
    echo "Usage: <user> <database>"
    exit 1
fi

mysqldump --opt -u "$user" -p "$database" | bzip2 -c > "$database.sql.bz2"
