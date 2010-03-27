#!/usr/bin/perl

# To Do:
# - Print human readable names for shows, seasons and episodes.
# - Use as CGI script.

# See:
#   http://blog.mytvshows.org/kind-of-an-api/


# External modules:
use LWP::UserAgent ();
use Text::xSV ();

# Internal modules:
use Pearl;


sub alphanumerically {
    return (($a =~ m/^\d+$/) && ($b =~ m/^\d+$/)) ? $a <=> $b : $a cmp $b;
}


sub download {
    my ($url) = @ARG;
    my $browser = LWP::UserAgent->new();
    my $response = $browser->get("http://www.mytvshows.org/$url",
        'User-Agent' => 'Mozilla');
    
    $response->is_success()
        or die 'Download failed: '.$response->status_line()."\n";
    
    $response->content_type() eq 'text/html'
        or die 'Invalid content type: '.$response->content_type()."\n";
    
    return $response->decoded_content();
}


sub get_status {
    my ($api_key, $show, $season, $episode) = @ARG;
    my $status = download("api/get_status/$api_key/$show/$season/$episode");
    
    return ($status =~ m/not\s+found/i) ? undef : $status;
}


sub list_episodes {
    my ($show, $season) = @ARG;
    
    return sort alphanumerically
        download("show/$show/$season") =~ m{<span class="nr">([^<]+)}g;
}


sub list_seasons {
    my ($show) = @ARG;
    
    return sort alphanumerically
        download("show/$show") =~ m{<li><a href="/show/\Q$show\E/([^/"]+)}g;
}


sub list_shows {
    my ($user_name) = @ARG;
    
    return sort alphanumerically
        download("user/$user_name") =~ m{<a href="/show/([^/"]+)}g;
}


sub main {
    unless ((@ARGV > 0) && (@ARGV < 3)) {
        print <<'USAGE' and return;
Usage: <user name> [API key]
USAGE
    }
    
    my ($user_name, $api_key) = @ARGV;
    my $csv = Text::xSV->new();
    my @header = qw(Name);
    
    if (defined $api_key) {
        push @header, qw(Season Episode Status);
    }
    
    $csv->set_header(@header);
    $csv->print_header();
    
    if (defined $api_key) {
        foreach my $show (list_shows($user_name)) {
            foreach my $season (list_seasons($show)) {
                foreach my $episode (list_episodes($show, $season)) {
                    my $status = get_status($api_key, $show, $season, $episode);
                    
                    if (defined $status) {
                        $csv->print_row($show, $season, $episode, $status);
                    }
                }
            }
        }
    }
    else {
        $csv->print_row($ARG) foreach list_shows($user_name);
    }
}


main();
