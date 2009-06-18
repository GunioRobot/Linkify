package Channel::RTP2;

use LWP;
use Pearl;


sub details {
    return (
        description => 'Daily TV guide for the RTP2 Portuguese channel.',
        url => 'http://www.rtp.pt/tv/rtp2/includes/guia.php',
        title => 'RTP2 Guia TV',
    );
}


sub generate {
    my ($rss) = @ARG;
    my $today = _today();
    my @guide = _parse(_download($today));
    
    $rss->add_item(
        title => 'Daily Guide',
        link => _url($today),
        description => join('', map {_to_html($ARG)} @guide),
        dc => {
            date => $today,
            identifier => $today,
        },
    );
}


sub _download {
    my ($date) = @ARG;
    my $browser = LWP::UserAgent->new();
    my $response = $browser->get(_url($date), 'User-Agent' => 'Mozilla');
    
    $response->is_success()
        or die 'Download failed: '.$response->status_line()."\n";
    
    $response->content_type() eq 'text/html'
        or die 'Invalid content type: '.$response->content_type()."\n";
    
    return $response->decoded_content();
}


sub _url {
    my ($date) = @ARG;
    
    $date = join '-', reverse split m/-/, $date;
    return "http://www.rtp.pt/tv/rtp2/includes/epg_output.php?dia=$date";
}


sub _parse {
    my ($content) = @ARG;
    my @guide;
    
    my @hours = ($content =~ m/<span class="guiaHora">([^<]+)/gs);
    my @titles = ($content =~ m/<span class="guiaTitulo"><[^>]+>([^<]+)/gs);
    my @urls = ($content =~ m/<span class="guiaTitulo"><a href='([^']+)'/gs);
    
    while (@titles > 0) {
        push @guide, {
            hour => shift @hours,
            url => shift @urls,
            title => uncapitalize(shift @titles),
        }
    }
    
    return @guide;
}


sub _to_html {
    my ($program) = @ARG;
    my ($hour, $url, $title) = @$program{qw(hour url title)};
    
    return "<b>$hour</b> - <a href=\"$url\">$title</a><br/>";
}


sub _today {
    my ($seconds, $minutes, $hours, $day, $month, $year) = gmtime;
    return sprintf '%d-%02d-%02d', ($year + 1900), ($month + 1), $day;
}


1;
