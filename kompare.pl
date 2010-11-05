#!/usr/bin/perl

# Helper script to use Kompare as the diff command e.g. for SVN, Git, etc.

use strict;
use warnings;
use File::stat qw(stat);

exit system qw(kompare -c), do {
    my %seen;
    grep {!$seen{$_}++}                             # Remove duplicates.
    sort {stat($a)->mtime() <=> stat($b)->mtime()}  # Order by modified time.
    grep -e,                                        # Files only.
    @ARGV
};
