#!/usr/bin/perl

use strict;
use warnings;

print <<EOT;
static const TCHAR* _messages[] = {
    NULL,
EOT

while (my $line = <STDIN>) {
    if ($line =~ m/(RIL_E_\w+)[^\@]+\@constdefine\s+(.+)$/) {
        print "    TEXT(\"$2\"), // $1\n";
    }
}

print <<EOT;
    NULL
};
EOT
