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
    
    return $response->content;
}


sub _parse_guide {
    my ($content) = @ARG;
    my @guide;
    
    my ($debuts) = ($content =~ m/<!-- ESTREIAS -->(.+)<!-- \/ESTREIAS -->/s);
    my @info = ($debuts =~ m#([^"]+)">([^>]+)\s+</a><br\s*/>\s*([^<]+)\s+#gs);
    
    while (@info > 0) {
        my ($link, $title, $date) = (shift @info, shift @info, shift @info);
        
        push @guide, {
            date => join('-', reverse split m/-/, $date),
            title => $title,
            link => "http://cultura.sapo.pt$link",
        }
    }
    
    return sort {$a->{title} cmp $b->{title}} @guide;
}


sub _to_html {
    my ($program) = @ARG;
    my ($link, $title) = @$program{qw(link title)};
    
    return "<a href=\"$link\">$title</a><br/>";
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
