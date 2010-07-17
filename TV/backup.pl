#!/usr/bin/perl

use defaults;
use threads;
use threads::shared;

# External modules:
use DateTime ();
use Getopt::Long ();
use IO::Compress::Bzip2 ();
use Regexp::Common qw(number);
use Text::xSV ();
use Thread::Queue ();

# Internal modules:
use TV::Tracker ();


sub alphanumerically {
    my $real = qr/^$Regexp::Common::RE{num}{real}$/;
    return (($a =~ $real) && ($b =~ $real)) ? $a <=> $b : $a cmp $b;
}


sub backup {
    my ($tracker, $parallel, $output) = @ARG;
    my $episodes = Thread::Queue->new();
    
    my $backup = async {
        my $stream = do {
            if ($output) {
                \*STDOUT;
            }
            else {
                my $date_time = DateTime->now();
                
                IO::Compress::Bzip2->new(sprintf '%s (%sT%sZ).csv.bz2',
                    $tracker->name(),
                    $date_time->ymd(''),
                    $date_time->hms(''));
            }
        };
        
        print $stream $ARG while $ARG = $episodes->dequeue();
        close $stream unless $output;
    };
    
    export($tracker, $parallel, $episodes);
    $episodes->enqueue(undef);
    $backup->join();
}


sub export {
    my ($tracker, $parallel, $episodes) = @ARG;
    my %shows :shared = $tracker->list_shows();
    my $header = 'Name ID/Name/Season ID/Season/Episode ID/Episode/Status';
    my $csv = Text::xSV->new();
    my $work = Thread::Queue->new();
    my $remaining :shared = 0;
    
    $csv->set_header(split '/', $header);
    $episodes->enqueue($csv->format_header());
    
    async {
        for (; my $item = $work->dequeue(); --$remaining) {
            my ($show_id, $season_id, $season, $ep_id, $ep) = @$item;
            my $status = $tracker->get_status($show_id, $season_id, $ep_id);
            
            $episodes->enqueue($csv->format_row(
                $show_id, $shows{$show_id},
                $season_id, $season,
                $ep_id, $ep,
                $status));
        }
    }->detach() for 1 .. $parallel;
    
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
        'output' => \(my $output = $false),
        'parallel=i' => \(my $parallel = 5),
    );
    
    binmode STDOUT, ':utf8';
    return unless Getopt::Long::GetOptionsFromArray(\@ARG, %options);
    
    if ((@ARG == 0) || $help || ($parallel < 1)) {
        my @trackers = TV::Tracker->list_trackers();
        print <<"USAGE" and return;
Usage: [options] <tracker> <arguments>

Trackers: @trackers

Options:
  --help        Display this information.
  --output      Print to standard output instead of saving to a file.
  --parallel    Number of concurrent connections.
USAGE
    }
    
    backup(TV::Tracker->load_tracker(@ARG), $parallel, $output);
}


main(@ARGV);
