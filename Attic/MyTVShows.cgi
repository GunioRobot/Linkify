#!/usr/bin/perl

use strict;
use utf8;
use warnings;

use CGI ();
use English qw(-no_match_vars);
use LWP::UserAgent ();
use Text::xSV ();


sub alphanumerically {
    return (($a =~ m/^\d+$/) && ($b =~ m/^\d+$/)) ? $a <=> $b : $a cmp $b;
}


sub generate {
    my ($user_name, $api_key) = @ARG;
    my %shows = list_shows($user_name);
    my $csv = Text::xSV->new();
    my @header = ('Name ID', 'Name');
    
    if (defined $api_key) {
        push @header, split '/', 'Season ID/Season/Episode ID/Episode/Status';
    }
    
    $csv->set_header(@header);
    $csv->print_header();
    
    if (defined $api_key) {
        foreach my $show (sort keys %shows) {
            my %seasons = list_seasons($show);
            
            foreach my $season (sort alphanumerically keys %seasons) {
                my %episodes = list_episodes($show, $season);
                
                foreach my $episode (sort alphanumerically keys %episodes) {
                    my $status = get_status($api_key, $show, $season, $episode);
                    
                    if (defined $status) {
                        $csv->print_row(
                            $show => $shows{$show},
                            $season => $seasons{$season},
                            $episode => $episodes{$episode},
                            $status);
                    }
                }
            }
        }
    }
    else {
        $csv->print_row($ARG => $shows{$ARG}) foreach sort keys %shows;
    }
}


sub get {
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


# See: http://blog.mytvshows.org/kind-of-an-api/
sub get_status {
    my ($api_key, $show, $season, $episode) = @ARG;
    my $status = get("api/get_status/$api_key/$show/$season/$episode");
    
    return ($status =~ m/not\s+found/i) ? undef : $status;
}


sub list_episodes {
    my ($show, $season) = @ARG;
    my $episodes = get("show/$show/$season");
    my @numbers = ($episodes =~ m{span\s+class="nr">([^<]+)}gi);
    my @names = ($episodes =~ m{span\s+class="title">([^<]+)}gi);
    
    return map {($numbers[$ARG], $names[$ARG])} 0..$#numbers;
}


sub list_seasons {
    my ($show) = @ARG;
    my ($seasons) = (get("show/$show") =~ m{"seasons_list">.+?</ul}gis);
    
    return $seasons =~ m{href="/show/\Q$show\E/([^/"]+)[^>]+>([^<]+)}gi;
}


sub list_shows {
    my ($user_name) = @ARG;
    return get("user/$user_name") =~ m{href="/show/([^/"]+)[^>]+>([^<]+)}gi;
}


sub main {
    my ($user_name, $api_key) = (CGI::url_param('user'), CGI::url_param('key'));
    
    if (defined $user_name) {
        $OUTPUT_AUTOFLUSH = 1;
        binmode STDOUT, ':utf8';
        print "Content-type: text/plain; charset=UTF-8\n\n";
    }
    else {
        unless ((@ARGV > 0) && (@ARGV < 3)) {
            print "Usage: <user name> [API key]\n";
            return;
        }
        
        ($user_name, $api_key) = @ARGV;
    }
    
    generate($user_name, $api_key);
}


main();
