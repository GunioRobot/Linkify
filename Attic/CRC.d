module CRC;


public alias CRC_16_CCITT CRC_CCITT;
public alias CRC_16_IBM CRC_16;
public alias CRC_32_IEEE_802_3 CRC_32;


public ushort CRC_16_CCITT(ubyte[] data, ushort initialValue = 0xFFFF) {
    return CRC!(ushort)(data, initialValue, 0, 0x1021, false, false);
}


public ushort CRC_16_IBM(ubyte[] data, ushort initialValue = 0) {
    return CRC!(ushort)(data, initialValue, 0, 0x8005, true, true);
}


public uint CRC_32_IEEE_802_3(ubyte[] data, uint initialValue = 0xFFFFFFFF) {
    return CRC!(uint)(data, initialValue, 0xFFFFFFFF, 0x04C11DB7, true, true);
}


public T CRC(T)(
    ubyte[] data,
    T initialValue, T finalXorValue, T polynomial,
    bool reflectData, bool reflectRemainder)
{
    T remainder = initialValue;
    size_t widthBits = T.sizeof * 8;
    
    foreach (b; data) {
        remainder ^= (reflectData ? reflect(b) : b) << (widthBits - 8);
        
        for (int bit = 0; bit < 8; ++bit) {
            if ((remainder >> (widthBits - 1)) == 1) {
                remainder = (remainder << 1) ^ polynomial;
            }
            else {
                remainder <<= 1;
            }
        }
    }
    
    return (reflectRemainder ? reflect(remainder) : remainder) ^ finalXorValue;
}


private T reflect(T)(T data) {
    T reflectedData = 0;
    size_t widthBits = T.sizeof * 8;
    
    for (ubyte bit = 0; bit < widthBits; ++bit, data >>= 1) {
        if ((data & 1) == 1) {
            reflectedData |= (1 << ((widthBits - 1) - bit));
        }
    }
    
    return reflectedData;
}


unittest {
    ubyte[] data = ['1', '2', '3', '4', '5', '6', '7', '8', '9'];
    
    assert(CRC_16_CCITT(data) == 0x29B1);
    assert(CRC_16_IBM(data) == 0xBB3D);
    assert(CRC_32_IEEE_802_3(data) == 0xCBF43926);
}
