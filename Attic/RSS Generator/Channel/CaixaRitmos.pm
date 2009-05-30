package Channel::CaixaRitmos;

use LWP;
use Pearl;


our $description = 'Weekly podcasts from the Antena 3 Portuguese radio station.';
our $link = 'http://ww1.rtp.pt/multimedia/programa.php?prog=1680&from_iframe=';
our $title = 'Caixa de Ritmos';


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
    
    my @info = ($content =~ m`
        CAIXA\s+DE\s+RITMOS</td>.+?
        (\d+)                  # Part number.
        \s*ª\s*(?:hora|parte).+?
        (\d{4}-\d{2}-\d{2})    # Date.
        .+?
        (\d+m\d+s)             # Length.
        .+?
        (\d+\.\d+)             # Size in MiB.
        \s+Mb.+?"
        (mms://[^"]+)          # Link.
        `gsx
    );
    
    my @episodes;
    
    while (@info > 0) {
        my $part = shift @info;
        my $date = shift @info;
        my $length = shift @info;
        my $size = shift @info;
        my $link = shift @info;
        
        push @episodes, {
            date => $date,
            length => $length,
            link => $link,
            part => $part,
            size => $size,
        }
    }
    
    return sort {$a->{date} cmp $b->{date} || $a->{part} cmp $b->{part}} @episodes;
}


sub _to_html {
    my ($episode) = @ARG;
    
    return <<"HTML";
<p>
  <b>Duração:</b> $episode->{length}
  <br/>
  <b>Tamanho:</b> $episode->{size} MiB
</p>
HTML
}


sub generate {
    my ($rss) = @ARG;
    
    foreach my $episode (_parse(_download())) {
        $rss->add_item(
            title => "$episode->{part}ª parte",
            link => $episode->{link},
            description => _to_html($episode),
            dc => {
                date => $episode->{date},
                identifier => "$episode->{date}#$episode->{part}",
            },
        );
    }
}
