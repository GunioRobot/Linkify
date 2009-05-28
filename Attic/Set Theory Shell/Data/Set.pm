package Data::Set;

use base 'Data::Collection';
use Pearl;


sub new {
    my ($invocant) = @ARG;
    my $class = ref($invocant) || $invocant;
    my $self = {elements => {}};
    
    return bless $self, $class;
}


sub add {
    my ($self, @elements) = @ARG;
    my $elements = $self->{elements};
    
    foreach my $element (@elements) {
        my $hash = $element->to_hash();
        next if exists $elements->{$hash};
        
        $elements->{$hash} = $element;
    }
    
    return $self;
}


sub elements {
    my ($self) = @ARG;
    return values %{$self->{elements}};
}


sub has {
    my ($self, $element) = @ARG;
    return exists $self->{elements}->{$element->to_hash()} ? $true : $false;
}


sub to_string {
    my ($self) = @ARG;
    my $elements = $self->{elements};
    my @strings;
    
    foreach my $hash (keys %$elements) {
        push @strings, $elements->{$hash}->to_string();
    }
    
    return '{'.join(' ', sort @strings).'}';
}


1
