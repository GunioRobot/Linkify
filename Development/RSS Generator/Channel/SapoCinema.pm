package Channel::SapoCinema;

use LWP;
use Pearl;


sub details {
    return (
        description => 'Weekly movie guide from the Sapo Cultura web site.',
        url => 'http://cultura.sapo.pt/cinema.aspx',
        title => 'Sapo Cultura Cinema',
    );
}


sub generate {
    my ($rss) = @ARG;
    my @guide = _parse(_download());
    
    $rss->add_item(
        title => 'Weekly Guide',
        link => {details()}->{url},
        description => join('', map {_to_html($ARG)} @guide),
        dc => {
            date => $guide[0]{date},
            identifier => $guide[0]{date},
        },
    );
}


sub _download {
    my $browser = new LWP::UserAgent;
    my $response = $browser->get({details()}->{url}, 'User-Agent' => 'Mozilla');
    
    $response->is_success
        or die 'Download failed: '.$response->status_line."\n";
    
    $response->content_type eq 'text/html'
        or die 'Invalid content type: '.$response->content_type."\n";
    
    return $response->decoded_content;
}


sub _parse {
    my ($content) = @ARG;
    
    my @guide;
    my ($debuts) = ($content =~ m/<!-- ESTREIAS -->(.+)<!-- \/ESTREIAS -->/s);
    
    my @info = ($debuts =~ m`
        >\s+
        ([^>]+)    # Genre.
        \s+<h1><[^<]+>
        ([^>]+)    # Portuguese title.
        </a></h1><a\s*href="
        ([^"]+)    # URL.
        ">
        ([^>]+)    # Title.
        \s+</a><br\s*/>\s*
        ([^<]+)    # Date.
        \s+`gsx
    );
    
    while (@info > 0) {
        my $genre = shift @info;
        my $title_pt = shift @info;
        my $url = shift @info;
        my $title = shift @info;
        my $date = shift @info;
        
        push @guide, {
            date => join('-', reverse split m/-/, $date),
            genre => $genre,
            url => "http://cultura.sapo.pt$url",
            title => $title,
            title_pt => $title_pt,
        }
    }
    
    return sort {$a->{title} cmp $b->{title}} @guide;
}


sub _to_html {
    my ($program) = @ARG;
    my ($url, $title, $pt, $genre) = @$program{qw(url title title_pt genre)};
    
    return qq{<p><b><a href="$url">$title</a></b><br/>"$pt", <i>$genre</i></p>};
}


1;
