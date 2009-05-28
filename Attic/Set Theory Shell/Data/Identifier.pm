package Data::Identifier;

use base 'Data::Value';
use Pearl;


sub new {
    my ($class, $identifier) = @ARG;
    return instantiate($class, identifier => $identifier);
}


sub to_string {
    my ($self) = @ARG;
    return $self->{identifier};
}


1
