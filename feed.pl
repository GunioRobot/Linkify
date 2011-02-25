#!/usr/bin/env perl

use defaults;
use List::MoreUtils qw(indexes);
use XML::FeedPP ();


# my $url = 'http://feeds.ign.com/ignfeeds/podcasts/games/';
my $url = 'http://feeds.hd-trailers.net/hd-trailers/blog';

my $feed = XML::FeedPP->new($url);
my @items_to_remove = indexes {!is_hd_trailer($ARG)} $feed->get_item();

$feed->remove_item($ARG) for reverse @items_to_remove;

print "Content-Type: application/rss+xml; charset=UTF-8\n\n";
print $feed->to_string();


sub is_hd_trailer {
    my ($item) = @ARG;
    return $item->title() =~ m/\( [^(]* (teaser | trailer) [^)]* \)/xi;
}


sub is_ign_daily_fix_item {
    my ($item) = @ARG;
    return $item->title() =~ m/\b IGN \s+ Daily \s+ Fix \b/xi;
}
