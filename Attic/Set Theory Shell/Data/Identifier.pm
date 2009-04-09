package Data::Identifier;

use base 'Data::Value';
use strict;
use utf8;

use Pearl;


sub new {
    my ($invocant, $identifier) = @ARG;
    my $class = ref($invocant) || $invocant;
    my $self = {identifier => $identifier};
    
    return bless $self, $class;
}


sub to_string {
    my ($self) = @ARG;
    return $self->{identifier};
}


1
