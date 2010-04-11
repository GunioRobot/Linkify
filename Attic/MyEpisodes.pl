#!/usr/bin/perl

# To Do:
# - Check for invalid user name and password.
# - Check for invalid HTTP responses.
# - Have an option to save cookies locally?
# - Have an option to save and compress the exported list to a file?
# - Print episode number in format "S-E-"?
# - Merge with the MyTVShows exporter script?
# - Use WWW::Mechanize?


# External modules:
use HTTP::Cookies ();
use HTTP::Request::Common ();
use List::MoreUtils ();
use LWP::UserAgent ();
use Text::xSV ();

# Internal modules:
use Pearl;


my $base_url = 'http://www.myepisodes.com/';


sub download {
    my ($cookies, $url) = @ARG;
    my $agent = LWP::UserAgent->new();
    
    $agent->cookie_jar($cookies);
    return $agent->get($base_url.$url)->decoded_content();
}


sub export_episode_list {
    my ($csv, $cookies, $show_id, $show) = @ARG;
    my $list = download($cookies, "views.php?type=epsbyshow&showid=$show_id");
    
    my @season_names = ($list =~ m/class="season">([^<]+)/gi);
    my @season_ids = ($list =~ m/id="epscnt([^"]+)/gi);
    
    my @episode_names = ($list =~ m|target="_blank">([^<]+)</a></td>|gi);
    my @episode_ids = ($list =~ m/id="A([^"]+)/gi);
    
    my %acquired = ($list =~ m|name="A([^"]+)"((?:\s+checked)?)></td>|gi);
    my %viewed = ($list =~ m|name="V([^"]+)"((?:\s+checked)?)></td>|gi);
    
    my %seasons = List::MoreUtils::mesh(@season_ids, @season_names);
    my %episodes = List::MoreUtils::mesh(@episode_ids, @episode_names);
    
    foreach my $season (@season_ids) {
        foreach my $episode (grep m/^\Q$season\E/, @episode_ids) {
            my $status = 'unseen';
            
            if ($viewed{$episode} =~ m/checked/i) {
                $status = 'seen';
            }
            elsif ($acquired{$episode} =~ m/checked/i) {
                $status = 'acquired';
            }
            
            $csv->print_row(
                $show_id, $show,
                $season, $seasons{$season},
                $episode, $episodes{$episode},
                $status);
        }
    }
}


sub list_shows {
    my ($cookies) = @ARG;
    my $shows = download($cookies, 'shows.php?type=manage');
    
    return $shows =~ m/<option\s+value="([^"]+)">([^<]+)/gi;
}


sub log_in {
    my ($name, $password) = @ARG;
    my $agent = LWP::UserAgent->new();
    my $cookies = HTTP::Cookies->new();
    
    my $log_in = HTTP::Request::Common::POST($base_url.'login.php', [
        username => $name,
        password => $password,
        action => 'Login',
        u => '',
    ]);
    
    $agent->cookie_jar($cookies);
    $agent->request($log_in);
    
    return $cookies;
}


sub main {
    binmode STDOUT, ':utf8';
    
    if (@ARGV != 2) {
        print "Usage: <name> <password>\n";
        return;
    }
    
    my ($name, $password) = @ARGV;
    my $cookies = log_in($name, $password);
    my %shows = list_shows($cookies);
    my $header = 'Name ID/Name/Season ID/Season/Episode ID/Episode/Status';
    my $csv = Text::xSV->new();
    
    $csv->set_header(split '/', $header);
    $csv->print_header();
    
    foreach my $show (keys %shows) {
        export_episode_list($csv, $cookies, $show, $shows{$show});
    }
}


main();
