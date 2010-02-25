#include "Ril.h"


namespace WM {
    size_t Ril::_references = 0;
    std::map<HRIL, Ril*> Ril::_instances;
}
