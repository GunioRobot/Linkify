#!/usr/bin/perl

# See: http://blog.mytvshows.org/kind-of-an-api/


use strict;
use threads;
use threads::shared;
use utf8;
use warnings;

use CGI ();
use English qw(-no_match_vars);
use Getopt::Long ();
use LWP::UserAgent ();
use Text::xSV ();
use Thread::Queue ();


sub alphanumerically {
    return (($a =~ m/^\d+$/) && ($b =~ m/^\d+$/)) ? $a <=> $b : $a cmp $b;
}


sub download {
    my ($url) = @ARG;
    my $response = LWP::UserAgent->new()->get("http://www.mytvshows.org/$url",
        'User-Agent' => 'Mozilla');
    
    $response->is_success()
        or die 'Download failed: '.$response->status_line()."\n";
    
    $response->content_type() eq 'text/html'
        or die 'Invalid content type: '.$response->content_type()."\n";
    
    return $response->decoded_content();
}


sub export_list {
    my ($jobs, $user_name, $api_key) = @ARG;
    my %shows :shared = list_shows($user_name);
    my $csv = Text::xSV->new();
    my @header = ('Name ID', 'Name');
    
    if (defined $api_key) {
        push @header, split '/', 'Season ID/Season/Episode ID/Episode/Status';
    }
    
    $csv->set_header(@header);
    $csv->print_header();
    
    if (defined $api_key) {
        export_episode_list($jobs, $user_name, $api_key, $csv, \%shows);
    }
    else {
        foreach my $show (sort alphanumerically keys %shows) {
            $csv->print_row($show, $shows{$show});
        }
    }
}


sub export_episode_list {
    my ($jobs, $user_name, $api_key, $csv, $shows) = @ARG;
    my $work = Thread::Queue->new();
    my $remaining :shared = 0;
    
    async {
        while (my $item = $work->dequeue()) {
            my ($show, $season_id, $season, $ep_id, $ep) = @$item;
            my $status = eval {get_status($api_key, $show, $season_id, $ep_id)};
            exit if $EVAL_ERROR;
            
            print $csv->format_row(
                $show, $shows->{$show},
                $season_id, $season,
                $ep_id, $ep,
                $status) if defined $status;
            
            --$remaining;
        }
    }->detach() for (1 .. $jobs);
    
    foreach my $show (sort alphanumerically keys %$shows) {
        my %seasons = list_seasons($show);
        
        foreach my $season (sort alphanumerically keys %seasons) {
            my %episodes = list_episodes($show, $season);
            
            foreach my $episode (sort alphanumerically keys %episodes) {
                ++$remaining;
                
                $work->enqueue([$show,
                    $season, $seasons{$season},
                    $episode, $episodes{$episode}]);
            }
        }
    }
    
    sleep 1 while $remaining > 0;
}


sub get_status {
    my ($api_key, $show, $season, $episode) = @ARG;
    my $status = download("api/get_status/$api_key/$show/$season/$episode");
    
    if ($status =~ m/wrong.+?key/i) {
        die "Invalid API key.\n";
    }
    elsif ($status =~ m/not\s+found/i) {
        return undef;
    }
    else {
        return $status;
    }
}


sub list_episodes {
    my ($show, $season) = @ARG;
    my $episodes = download("show/$show/$season");
    my @numbers = ($episodes =~ m{span\s+class="nr">([^<]+)}gi);
    my @names = ($episodes =~ m{span\s+class="title">([^<]*)}gi);
    
    return map {($numbers[$ARG], $names[$ARG])} 0..$#numbers;
}


sub list_seasons {
    my ($show) = @ARG;
    my ($seasons) = (download("show/$show") =~ m{"seasons_list">.+?</ul}gis);
    
    return $seasons =~ m{href="/show/\Q$show\E/([^/"]+)[^>]+>([^<]+)}gi;
}


sub list_shows {
    my ($name) = @ARG;
    my $shows = download("user/$name");
    
    die "Invalid user name.\n" if $shows =~ m/page\s+not\s+found/i;
    return $shows =~ m{href="/show/([^/"]+)[^>]+>([^<]+)}gi;
}


sub main {
    my ($user_name, $api_key) = (CGI::url_param('user'), CGI::url_param('key'));
    my $jobs = 5;
    
    $OUTPUT_AUTOFLUSH = 1;
    binmode STDOUT, ':utf8';
    
    if (defined $user_name) {
        print "Content-type: text/plain; charset=UTF-8\n\n";
    }
    else {
        return unless Getopt::Long::GetOptions('jobs=i' => \$jobs);
    
        unless ((@ARGV > 0) && (@ARGV < 3) && ($jobs > 0)) {
            print <<'USAGE' and return;
Usage: [options] <user name> [API key]
Options:
  --jobs #
USAGE
        }
        
        ($user_name, $api_key) = @ARGV;
    }
    
    export_list($jobs, $user_name, $api_key);
}


main();
