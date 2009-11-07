package Data::List;

use base qw(Data::Collection);

# Internal modules:
use Pearl;


sub new {
    my ($class) = @ARG;
    return instantiate($class, elements => []);
}


sub add {
    my ($self, @elements) = @ARG;
    push @{$self->{elements}}, @elements;
    return $self;
}


sub elements {
    my ($self) = @ARG;
    return @{$self->{elements}};    
}


sub has {
    my ($self, $element) = @ARG;
    
    foreach my $e ($self->elements()) {
        return $true if $e->equals($element);
    }
    
    return $false;
}


sub to_string {
    my ($self) = @ARG;
    my @strings;
    
    foreach my $element (@{$self->{elements}}) {
        push @strings, $element->to_string();
    }
    
    return '('.join(' ', @strings).')';
}


1;
