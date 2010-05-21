package TV::Tracker;

use defaults;
use LWP::UserAgent ();


sub _download {
    my ($self, $url, $cookies) = @ARG;
    my $agent = LWP::UserAgent->new();
    
    $agent->cookie_jar($cookies) if defined $cookies;
    return $agent->get($url, 'User-Agent' => 'Mozilla')->decoded_content();
}


sub get_status {
    my ($self, $show, $season, $episode) = @ARG;
    abstract
}


sub list_episodes {
    my ($self, $show, $season) = @ARG;
    abstract
}


sub list_seasons {
    my ($self, $show) = @ARG;
    abstract
}


sub list_shows {
    my ($self) = @ARG;
    abstract
}


sub new {
    my ($class, @args) = @ARG;
    abstract
}


1;
