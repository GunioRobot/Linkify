package Data::Value;

use Carp;
use Digest::MD5 'md5_hex';
use Pearl;


sub equals {
    my ($self, $another) = @ARG;
    return ($self->to_hash() eq $another->to_hash()) ? $true : $false;
}


sub to_hash {
    my ($self) = @ARG;
    return md5_hex($self->to_string());
}


sub to_string {
    croak;
}


1
