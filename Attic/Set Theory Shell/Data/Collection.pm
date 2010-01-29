package Data::Collection;

use base qw(Data::Value);

# Internal modules:
use Pearl;


sub add {
    abstract();
}


sub elements {
    abstract();
}


sub has {
    abstract();
}


1;
