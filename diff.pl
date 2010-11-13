#!/usr/bin/perl

use strict;
use warnings;
use IPC::Open2 ();


IPC::Open2::open2(my ($output, $input), 'colordiff', @ARGV);
my @lines = <$output>;

close $output;
close $input;
wait;

if (@lines < 15) {
    print @lines;
}
else {
    IPC::Open2::open2(my ($output, $input), qw(kompare -o -));
    
    foreach my $line (@lines) {
        # Remove ANSI "color" escape sequences.
        $line =~ s/\e\[\d+(?>(;\d+)*)m//g;
        print $input $line;
    }
    
    close $output;
    close $input;
    wait;
}

exit $?;
