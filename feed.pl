#!/usr/bin/env perl

use defaults;
use HTTP::Status ();
use List::MoreUtils qw(indexes);
use Mojolicious::Lite;
use XML::FeedPP ();


app->helper(filter_feed_items => sub {
    my ($self, $url, $keep_item) = @ARG;
    
    my $feed = try {
        XML::FeedPP->new($url);
    }
    catch {
        die $ARG unless $ARG =~ m/^\QLoading failed: $url\E/;
        
        $self->render(
            status => HTTP::Status::HTTP_GATEWAY_TIMEOUT,
            text => "Feed error: $ARG");
        
        undef;
    };
    
    if (defined $feed) {
        my @items_to_remove = indexes {!$keep_item->($ARG)} $feed->get_item;
        $feed->remove_item($ARG) for reverse @items_to_remove;
        
        $self->render(
            format => 'rss',
            text => $feed->to_string);
    }
});


# TODO: Keep favicon.ico from original feed?

app->hook(before_dispatch => sub {
    my ($self) = @ARG;
    
    if ($self->req->url->path eq '/favicon.ico') {
        $self->render_not_found;
    }
});


# TODO: Include posters as inline attachments.

get '/hd-trailers' => sub {
    shift->filter_feed_items(
        'http://feeds.hd-trailers.net/hd-trailers/blog',
        sub {$ARG->title =~ m/\( [^(]* (teaser | trailer) [^)]* \)/ix});
};


get '/ign-daily-fix' => sub {
    shift->filter_feed_items(
        'http://feeds.ign.com/ignfeeds/podcasts/games/',
        sub {$ARG->title =~ m/\b IGN \s+ Daily \s+ Fix \b/ix});
};


app->start;
