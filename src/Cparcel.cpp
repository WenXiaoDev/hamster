#include "Cparcel.h"

Cparcel::Cparcel()
{
    _data = (char *)malloc(GEN_TYPE);
    _readPtr = _data;
    _writePtr = _data;
    _type = GEN_TYPE;
}

Cparcel::Cparcel(const parcel_t type)
{
    if(type < 0 || type > TYPE_MAX) {
        return ;
    }

    _data = (char *)malloc(BASE << type);
    _readPtr = _data;
    _writePtr = _data;
    _type = type;
}

Cparcel::Cparcel(Cparcel &other)
{
    _type = other.getType();
    
}

Cparcel::~Cparcel()
{
    if(_data != nullptr) {
        free(_data);
        _data = nullptr;
    }

    _readPtr = 0;
}

int Cparcel::serialize(const void* param, int size)
{
    const char *pos = (char *)param;

    if(nullptr == param) {
        return -1;
    }

    if(getRestSpace() < size) {
        return -2;
    }

    for(int i = 0; i < size; i++) {
        *(_writePtr++) = *(pos++);
    }

    return size;
}

char *Cparcel::getData()
{
    return _data;
}

int Cparcel::getType()
{
    return _type;
}

int Cparcel::getRestSpace()
{
    return (BASE << _type) - (_writePtr - _data);
}

void Cparcel::clean()
{
    _readPtr = _data;
    _writePtr = _data;
}