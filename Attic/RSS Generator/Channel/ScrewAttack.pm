package Channel::ScrewAttack;

use LWP;
use Pearl;


our $description = 'Newest ScrewAttack episodes from GameTrailers.';
our $link = 'http://www.gametrailers.com/screwattack';
our $title = 'ScrewAttack Episodes';

my @MONTHS = qw(
    January February March April May June 
    July August September October November December
);


sub _download {
    my $browser = new LWP::UserAgent;
    my $response = $browser->get($link, 'User-Agent' => 'Mozilla');
    
    $response->is_success
        or die 'Download failed: '.$response->status_line."\n";
    
    $response->content_type eq 'text/html'
        or die 'Invalid content type: '.$response->content_type."\n";
    
    return $response->decoded_content;
}


sub _parse {
    my ($content) = @ARG;
    
    my @episodes;
    my ($episodes) = ($content =~ m/screw_tab_top(.+)<!-- END OF NERD/s);
    
    my @info = ($episodes =~ m`
        ([^"]+)     # Link.
        "\s+class="gamepage_content_row_title">\s+
        ([^<]+?)    # Title.
        \s{2,}.+?class="gamepage_content_row_date">\s+
        ([^<]+?)    # Date.
        \s{2,}.+?class="gamepage_content_row_text">\s+
        ([^<]+)     # Description.
        `gsx
    );
    
    while (@info > 0) {
        my $link = shift @info;
        my $title = shift @info;
        my $date = shift @info;
        my $description = shift @info;
        
        push @episodes, {
            date => _to_standard_date($date),
            description => $description,
            link => "http://www.gametrailers.com$link",
            title => $title,
        }
    }
    
    return sort {$a->{date} cmp $b->{date}} @episodes;
}


sub _to_standard_date {
    my ($date) = @ARG;
    my ($month_name, $day, $year) = ($date =~ m/([^\d\s]+)\s+(\d+),\s+(\d+)/);
    my %month_number = map {($MONTHS[$ARG], $ARG + 1)} 0..$#MONTHS;
    
    return sprintf '%d-%02d-%02d', $year, $month_number{$month_name}, $day;
}


sub generate {
    my ($rss) = @ARG;
    
    foreach my $episode (_parse(_download())) {
        $rss->add_item(
            title => $episode->{title},
            link => $episode->{link},
            description => $episode->{description},
            dc => {
                date => $episode->{date},
                identifier => $episode->{date},
            },
        );
    }
}
