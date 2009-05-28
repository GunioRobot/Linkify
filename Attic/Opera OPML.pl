#!/usr/bin/perl

use File::Spec::Functions;
use Pearl;
use XML::OPML::SimpleGen;


if (@ARGV != 1) {
    print "Exports the Opera feed list to OPML.\n";
    print "Usage: <mail directory>\n";
    exit 1;
}

open my $index, '<', catfile(shift @ARGV, 'index.ini') or die $ERRNO;
my @sections = split "\n\n", join '', <$index>;
close $index;

my $opml = new XML::OPML::SimpleGen(version => '1.0');
$opml->head(title => 'Opera feed list');

foreach my $section (grep m/^Type=5$/m, @sections) {
    my ($name) = ($section =~ m/^Name=(.+)$/m);
    my ($url) = ($section =~ m/^Search Text=(.+)$/m);
    
    $opml->insert_outline(
        text => $name,
        type => 'rss',
        xmlUrl => $url,
        title => $name,
    );
}

print $opml->as_string;
