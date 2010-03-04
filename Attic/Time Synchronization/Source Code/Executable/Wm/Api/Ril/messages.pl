#!/usr/bin/perl

use strict;
use warnings;

open my $file, '<', 'ril.h' or die $!;

print <<EOT;
static const TCHAR* _messages[] = {
    NULL,
EOT

while (my $line = <$file>) {
    if ($line =~ m/(RIL_E_\w+)[^\@]+\@constdefine\s+(.+)$/) {
        print "    TEXT(\"$2\"), // $1\n";
    }
}

print <<EOT;
    NULL
};
EOT

close $file;
