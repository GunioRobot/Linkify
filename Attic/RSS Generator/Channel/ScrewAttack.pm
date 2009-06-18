package Channel::ScrewAttack;

use LWP;
use Pearl;


my @MONTHS = qw(
    January February March April May June
    July August September October November December
);


sub details {
    return (
        description => 'Newest ScrewAttack episodes from GameTrailers.',
        title => 'ScrewAttack Episodes',
        url => 'http://www.gametrailers.com/screwattack',
    );
}


sub generate {
    my ($rss) = @ARG;
    
    foreach my $episode (_parse(_download())) {
        $rss->add_item(
            title => $episode->{title},
            link => $episode->{url},
            description => $episode->{description},
            dc => {
                date => $episode->{date},
                identifier => $episode->{date},
            },
        );
    }
}


sub _download {
    my $browser = LWP::UserAgent->new();
    my $response = $browser->get({details()}->{url}, 'User-Agent' => 'Mozilla');
    
    $response->is_success()
        or die 'Download failed: '.$response->status_line()."\n";
    
    $response->content_type() eq 'text/html'
        or die 'Invalid content type: '.$response->content_type()."\n";
    
    return $response->decoded_content();
}


sub _parse {
    my ($content) = @ARG;
    
    my @episodes;
    my ($episodes) = ($content =~ m/screw_tab_top(.+)<!-- END OF NERD/s);
    
    my @info = ($episodes =~ m`
        ([^"]+)     # URL.
        "\s+class="gamepage_content_row_title">\s+
        ([^<]+?)    # Title.
        \s{2,}.+?class="gamepage_content_row_date">\s+
        ([^<]+?)    # Date.
        \s{2,}.+?class="gamepage_content_row_text">\s+
        ([^<]+)     # Description.
        `gsx
    );
    
    while (@info > 0) {
        my $url = shift @info;
        my $title = shift @info;
        my $date = shift @info;
        my $description = shift @info;
        
        push @episodes, {
            date => _to_standard_date($date),
            description => $description,
            url => "http://www.gametrailers.com$url",
            title => $title,
        }
    }
    
    return sort {$a->{date} cmp $b->{date}} @episodes;
}


sub _to_standard_date {
    my ($date) = @ARG;
    my ($month_name, $day, $year) = ($date =~ m/([^\d\s]+)\s+(\d+),\s+(\d+)/);
    my %month_number = map {($MONTHS[$ARG] => $ARG + 1)} 0..$#MONTHS;
    
    return sprintf '%d-%02d-%02d', $year, $month_number{$month_name}, $day;
}


1;
