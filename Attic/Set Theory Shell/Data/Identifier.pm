package Data::Identifier;

use base qw(Data::Value);

# Internal modules:
use Pearl;


sub new {
    my ($class, $identifier) = @ARG;
    return instantiate($class, identifier => $identifier);
}


sub to_string {
    my ($self) = @ARG;
    return $self->{identifier};
}


1;
