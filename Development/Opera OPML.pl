#!/usr/bin/perl

# External modules:
use File::Spec ();
use XML::OPML::SimpleGen ();

# Internal modules:
use Pearl;


if (@ARGV != 1) {
    print "Exports the Opera feed list to OPML.\n";
    print "Usage: <path to index.ini>\n";
    exit 1;
}

my ($file) = @ARGV;
open my $index, '<', $file or die $ERRNO;

my @sections = split "\n\n", join '', <$index>;
close $index;

my $opml = XML::OPML::SimpleGen->new(version => '1.0');
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

print $opml->as_string();
