#!/usr/bin/perl

use defaults;
use threads;
use threads::shared;

use Getopt::Long ();
use Module::Load ();
use Regexp::Common qw(number);
use Text::xSV ();
use Thread::Queue ();
use TV::Tracker ();


sub alphanumerically {
    my $real = qr/^$Regexp::Common::RE{num}{real}$/;
    return (($a =~ $real) && ($b =~ $real)) ? $a <=> $b : $a cmp $b;
}


sub export {
    my ($tracker, $parallel) = @ARG;
    my %shows :shared = $tracker->list_shows();
    my $header = 'Name ID/Name/Season ID/Season/Episode ID/Episode/Status';
    my $csv = Text::xSV->new();
    my $work = Thread::Queue->new();
    my $remaining :shared = 0;
    
    $csv->set_header(split '/', $header);
    $csv->print_header();
    
    async {
        while (my $item = $work->dequeue()) {
            my ($show_id, $season_id, $season, $ep_id, $ep) = @$item;
            my $status = $tracker->get_status($show_id, $season_id, $ep_id);
            
            print $csv->format_row(
                $show_id, $shows{$show_id},
                $season_id, $season,
                $ep_id, $ep,
                $status);
            
            --$remaining;
        }
    }->detach() for 1..$parallel;
    
    foreach my $show (sort alphanumerically keys %shows) {
        my %seasons = $tracker->list_seasons($show);
        
        foreach my $season (sort alphanumerically keys %seasons) {
            my %episodes = $tracker->list_episodes($show, $season);
            
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


sub main {
    my %options = (
        'help' => \(my $help = $false),
        'parallel=i' => \(my $parallel = 5),
    );
    
    binmode STDOUT, ':utf8';
    return unless Getopt::Long::GetOptionsFromArray(\@ARG, %options);
    
    if ((@ARG == 0) || $help) {
        print <<'USAGE' and return;
Usage: [options] <tracker> <arguments>
Options:
  --help        Displays this information.
  --parallel    Number of concurrent connections.
USAGE
    }
    
    my $name = shift @ARG;
    my $module = sprintf '%s::%s', TV::Tracker::, $name;
    
    Module::Load::load($module);
    export($module->new(@ARG), $parallel);
}


main(@ARGV);
