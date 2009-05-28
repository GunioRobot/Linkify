package Data::List;

use base 'Data::Collection';
use Pearl;


sub new {
    my ($invocant) = @ARG;
    my $class = ref($invocant) || $invocant;
    my $self = {elements => []};
    
    return bless $self, $class;
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


1
