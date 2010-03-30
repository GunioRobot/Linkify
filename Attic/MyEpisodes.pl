#!/usr/bin/perl

# To Do:
# - Check for invalid user name and password.
# - Check for invalid HTTP responses.
# - Merge with the MyTVShows exporter script.
# - Have option to save cookies locally.
# - Print episode number in format "S-E-".


# External modules:
use Getopt::Long ();
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
    my $content = download($cookies, "views.php?type=epsbyshow&showid=$show_id");
    
    my @season_names = ($content =~ m/class="season">([^<]+)/gi);
    my @season_ids = ($content =~ m/id="epscnt([^"]+)/gi);
    
    my @episode_names = ($content =~ m|target="_blank">([^<]+)</a></td>|gi);
    my @episode_ids = ($content =~ m/id="A([^"]+)/gi);
    
    my %acquired = ($content =~ m|name="A([^"]+)"((?:\s+checked)?)></td>|gi);
    my %viewed = ($content =~ m|name="V([^"]+)"((?:\s+checked)?)></td>|gi);
    
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
    my $jobs = 5;
    return unless Getopt::Long::GetOptions('jobs=i' => \$jobs);
    
    if ((@ARGV != 2) || ($jobs <= 0)) {
        print <<'USAGE' and return;
Usage: [options] <name> <password>
Options:
  --jobs #
USAGE
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
