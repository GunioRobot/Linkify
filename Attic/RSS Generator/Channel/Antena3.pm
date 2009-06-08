package Channel::Antena3;

use Digest::MD5 'md5_hex';
use LWP;
use Pearl;


our $description = 'Weekly podcasts from the Antena 3 Portuguese radio station.';
our $link = 'http://ww1.rtp.pt/multimedia/index.php?rcanal=3';
our $title = 'Antena 3 Podcasts';

my @links = (
    'http://ww1.rtp.pt/multimedia/programa.php?prog=1683&from_iframe=',
    'http://ww1.rtp.pt/multimedia/programa.php?prog=1078&from_iframe=',
    'http://ww1.rtp.pt/multimedia/programa.php?prog=1680&from_iframe=',
);

sub _download {
    my ($link) = @ARG;
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
    my ($list) = ($content =~ m/podcasts<\/a>(.+)<!-- IFRAME/s);
    
    my @info = ($list =~ m`
        ["\s]>
        ([^<]*)                # Subtitle.
        </td>\s+</tr>\s+</table></td>.+?
        (\d{4}-\d{2}-\d{2})    # Date.
        .+?
        ((?:\d+h)?\d+m\d+s)    # Length.
        .+?
        (\d+\.\d+)             # Size in MiB.
        \s+Mb.+?"
        (mms://[^"]+)          # Link.
        `gsx
    );
    
    my $program = uncapitalize($content =~ m/"nomeprograma">([^<]+?)\s*</);
    my @episodes;
    
    while (@info > 0) {
        my $subtitle = shift @info;
        my $date = shift @info;
        my $length = shift @info;
        my $size = shift @info;
        my $link = shift @info;
        
        my $title = $program;
        my @identifier = ($program, $date);
        
        if ($subtitle) {
            my ($part) = ($subtitle =~ m/^(\d+)\s*ª\s*(?:hora|parte)$/i);
            
            if ($part) {
                $title .= " (${part}ª parte)";
                push @identifier, $part;
            }
            else {
                $title .= " - $subtitle";
            }
        }
        
        push @episodes, {
            date => $date,
            identifier => md5_hex(join '#', @identifier),
            length => $length,
            link => $link,
            size => $size,
            title => $title,
        }
    }
    
    return sort {
        $a->{date} cmp $b->{date} || $a->{identifier} cmp $b->{identifier}
    } @episodes;
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
    
    foreach my $link (@links) {
        foreach my $episode (_parse(_download($link))) {
            $rss->add_item(
                title => $episode->{title},
                link => $episode->{link},
                description => _to_html($episode),
                dc => {
                    date => $episode->{date},
                    identifier => $episode->{identifier},
                },
            );
        }
    }
}
