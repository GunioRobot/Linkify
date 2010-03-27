#!/usr/bin/perl

# To Do:
# - Print CSV line-by-line instead of everything at once.
# - Use as CGI script.
# - Get the human readable name for shows, seasons and episodes.
# - Implement import of backups.
#
# See:
#   http://blog.mytvshows.org/kind-of-an-api/


# External modules:
use autodie;
use LWP::UserAgent ();
use Text::CSV::Slurp ();

# Internal modules:
use Pearl;


sub alphanumeric {
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
    return download("api/get_status/$api_key/$show/$season/$episode");
}


sub list_episodes {
    my ($show, $season) = @ARG;
    my @l = download("show/$show/$season") =~ m{<span class="nr">([^<]+)}g;
    
    return sort alphanumeric @l;
}


sub list_seasons {
    my ($show) = @ARG;
    my @l = download("show/$show") =~ m{<li><a href="/show/\Q$show\E/([^/"]+)}g;
    
    return sort alphanumeric @l;
}


sub list_shows {
    my ($user_name) = @ARG;
    my @l = download("user/$user_name") =~ m{<a href="/show/([^/"]+)}g;
    
    return sort alphanumeric @l;
}


sub main {
    unless ((@ARGV > 0) && (@ARGV < 3)) {
        print <<'USAGE' and return;
Usage: <user name> [API key]
USAGE
    }
    
    my ($user_name, $api_key) = @ARGV;
    my @shows = list_shows($user_name);
    my @csv_fields = qw(Name);
    my @csv;
    
    if (defined $api_key) {
        push @csv_fields, qw(Season Episode Status);
        
        foreach my $show (@shows) {
            print STDERR "Listing seasons for show: $show\n";
            
            foreach my $season (list_seasons($show)) {
                print STDERR "  Listing episodes for season: $season\n";
                
                foreach my $ep (list_episodes($show, $season)) {
                    push @csv, {
                        Name => $show,
                        Season => $season,
                        Episode => $ep,
                        Status => get_status($api_key, $show, $season, $ep),
                    };
                }
            }
        }
    }
    else {
        @csv = map {{Name => $ARG}} @shows;
    }
    
    print Text::CSV::Slurp->create(input => \@csv, field_order => \@csv_fields);
    print "\n";
}


main();
