package Channel::SapoCinema;

use strict;
use utf8;
use warnings;

use English;
use LWP;


our $description = 'Weekly movie guide from the Sapo Cultura web site.';
our $link = 'http://cultura.sapo.pt/cinema.aspx';
our $title = 'Sapo Cultura Cinema';


sub _download_guide {
    my $browser = new LWP::UserAgent;
    my $response = $browser->get($link, 'User-Agent' => 'Mozilla');
    
    $response->is_success
      or die 'Download failed: '.$response->status_line."\n";
    
    $response->content_type eq 'text/html'
      or die 'Invalid content type: '.$response->content_type."\n";
    
    return $response->decoded_content;
}


sub _parse_guide {
    my ($content) = @ARG;
    
    my @guide;
    my ($debuts) = ($content =~ m/<!-- ESTREIAS -->(.+)<!-- \/ESTREIAS -->/s);
    
    my @info = ($debuts =~ m`
        >\s+
        ([^>]+)    # Genre.
        \s+<h1><[^<]+>
        ([^>]+)    # Portuguese title.
        </a></h1><a\s*href="
        ([^"]+)    # Link.
        ">
        ([^>]+)    # Title.
        \s+</a><br\s*/>\s*
        ([^<]+)    # Date.
        \s+`gsx
    );
    
    while (@info > 0) {
        my $genre = shift @info;
        my $title_pt = shift @info;
        my $link = shift @info;
        my $title = shift @info;
        my $date = shift @info;
        
        push @guide, {
            date => join('-', reverse split m/-/, $date),
            genre => $genre,
            link => "http://cultura.sapo.pt$link",
            title => $title,
            title_pt => $title_pt,
        }
    }
    
    return sort {$a->{title} cmp $b->{title}} @guide;
}


sub _to_html {
    my ($program) = @ARG;
    my ($link, $title, $pt, $genre) = @$program{qw(link title title_pt genre)};
    
    return "<p><b><a href=\"$link\">$title</a></b><br/>\"$pt\", <i>$genre</i></p>";
}


sub generate {
    my ($rss) = @ARG;
    my @guide = _parse_guide(_download_guide());
    
    $rss->add_item(
        title => 'Weekly Guide',
        link => $link,
        description => join('', map {_to_html($ARG)} @guide),
        dc => {
            date => $guide[0]{date},
            identifier => $guide[0]{date},
        },
    );
}
