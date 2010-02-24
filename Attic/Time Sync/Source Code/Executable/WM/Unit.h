#pragma once


namespace WM {
    typedef size_t (*Unit)(size_t value, size_t total);
    
    size_t Pixel(size_t value, size_t total);
    size_t Percent(size_t value, size_t total);
}
