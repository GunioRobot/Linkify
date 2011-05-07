#!/usr/bin/env perl

use defaults;
use List::MoreUtils qw(indexes);
use Mojolicious::Lite;
use XML::FeedPP ();


# TODO: Keep favicon.ico from original feed.

sub filter_feed_items(&$) {
    my ($keep_item, $url) = @ARG;
    my $feed = XML::FeedPP->new($url);
    my @items_to_remove = indexes {!$keep_item->($ARG)} $feed->get_item;
    
    $feed->remove_item($ARG) for reverse @items_to_remove;
    return $feed->to_string;
}


# TODO: Include posters as inline attachments.

get '/hd-trailers' => sub {
    shift->render(
        format => 'rss',
        text => filter_feed_items
            {$ARG->title =~ m/\( [^(]* (teaser | trailer) [^)]* \)/ix}
            'http://feeds.hd-trailers.net/hd-trailers/blog',
    );
};


get '/ign-daily-fix' => sub {
    shift->render(
        format => 'rss',
        text => filter_feed_items
            {$ARG->title =~ m/\b IGN \s+ Daily \s+ Fix \b/ix}
            'http://feeds.ign.com/ignfeeds/podcasts/games/',
    );
};


app->start;
