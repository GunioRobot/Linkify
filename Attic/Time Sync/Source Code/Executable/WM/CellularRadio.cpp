#include "CellularRadio.h"


namespace WM {
    size_t CellularRadio::_references = 0;
    std::map<HRIL, CellularRadio*> CellularRadio::_instances;
}
