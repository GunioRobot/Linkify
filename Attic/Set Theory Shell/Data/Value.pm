package Data::Value;

# External modules:
use Digest::MD5 ();

# Internal modules:
use Pearl;


sub equals {
    my ($self, $another) = @ARG;
    return ($self->to_hash() eq $another->to_hash()) ? $true : $false;
}


sub to_hash {
    my ($self) = @ARG;
    return Digest::MD5::md5_hex($self->to_string());
}


sub to_string {
    abstract();
}


1;
