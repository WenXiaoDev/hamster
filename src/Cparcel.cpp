#include <cstdlib>
#include <cstdio>
#include <new>
#include <cstring>

#include "cparcel.h"

/*
	usage:
	1.	Cparcel parcel;
	   	long b = 123456789;
		parcel.write(&b, 4);

	2.	Cparcel parcel2(PARCEL_BASE << 2);
	3.	Cparcel parcel3(parcel);
	4.	Cparcel parcel4(std::move(parcel));
*/

inline
int regularizeSize(int t)
{
    int ret = PARCEL_BASE;
    for(int i = 6; i <= 20; i++) {
        if((ret = (2 << i)) >= t) {
            break;
        }
    }

    return ret;
}

Cparcel::Cparcel() : mData(nullptr), mReadPtr(0), mWritePtr(0), mSize(PARCEL_BASE)
{
    try {
        mData = new char[PARCEL_BASE];
        memset(mData, 0, mSize);
    } catch(std::bad_alloc) {
        printf("Cparcel construct failed on bad allocation\n");
    }
}

Cparcel::Cparcel(int t) : mData(nullptr), mReadPtr(0), mWritePtr(0)
{
    // mSize will be 
    mSize = regularizeSize(t);
    try {
        mData = new char[mSize];
        memset(mData, 0, mSize);
    } catch(std::bad_alloc) {
        printf("Cparcel construct failed on bad allocation\n");
    }
}

// mSize is made for sure not a invalid value in basic constructor
Cparcel::Cparcel(const Cparcel &other) : mReadPtr(0), mWritePtr(other.mWritePtr), mSize(other.mSize)
{
    try {
        mData = new char[mSize];
        if(nullptr != other.mData) {
            memcpy(mData, other.mData, mSize);
        } else {
            printf("WARNNING: Cparcel copy constructor on abnormal parcel object.\n");
        }
    } catch(std::bad_alloc) {
        printf("Cparcel construct failed on bad allocation\n");
    }

}

// std::static_cast<T &&>(T &);
Cparcel::Cparcel(Cparcel &&other) : mReadPtr(0), mWritePtr(other.mWritePtr), mSize(other.mSize), mData(other.mData)
{
    other.mData = nullptr;
}

Cparcel::~Cparcel()
{
    release();
}

Cparcel &Cparcel::operator=(const Cparcel &other)
{
    mReadPtr = 0;
    mWritePtr = other.mWritePtr;
    mSize = other.mSize;

    try {
        if(mData != nullptr) {
            delete mData;
            mData = nullptr;
        }

        mData = new char[mSize];
        //memset(mData, 0, mSize);
        if(nullptr != other.mData) {
            memcpy(mData, other.mData, mSize);
        } else {
            printf("WARNNING: Cparcel copy constructor on abnormal parcel object.\n");
        }
    } catch(std::bad_alloc) {
        printf("Cparcel copy failed on bad allocation\n");
    }

    return *this;
}

// deprecated don't need to use
int Cparcel::init()
{
    /*
    mWritePtr = 0;
    mReadPtr = 0;

    if(mData != nullptr) {
        return 0;
    }

    mData = (char *)malloc(mSize * sizeof(char));
    if(mData == nullptr) {
        printf("Cparcel::init() memory allocation failed.\n");
        return 1;
    }
    */
    return 0;
}

int Cparcel::release()
{
    if(nullptr == mData) {
        delete mData;
        mData = nullptr;
    }

    mReadPtr = 0;
    mWritePtr = 0;
    mSize = 0;

    return 0;
}

inline
int Cparcel::getRestSpace()
{
    return mSize - mWritePtr;
}

void Cparcel::resize()
{

}

void Cparcel::clear()
{
    mWritePtr = 0;
    mReadPtr = 0;
}

Cparcel &Cparcel::write(void *pos, int size)
{
    char *p = (char *)pos;

    if(size > getRestSpace()) {
        indexExpand();
    }

    if(p != nullptr && size > 0 && size <= getRestSpace()) {
        memcpy(mData + mWritePtr, p, size * sizeof(char));
        mWritePtr += size;
        printf("Cparcel::write() .here\n");
    }

    return *this;
}

int Cparcel::read(void *target, int size)
{
    char *p = (char *)target;
    if(p != nullptr && size > 0 && size <= (mWritePtr - mReadPtr)) {
        memcpy(p, mData + mReadPtr, size * sizeof(char));
        mReadPtr += size;
        return size;
    }

    printf("Cparcel::read().target = %p, size = %d\n", target, size);
    return 0;
}

const char *Cparcel::getData() const
{
    return mData;
}

void Cparcel::indexExpand()
{
    char *tmp = mData;

    try {
        mData = new char[mSize*2];
    } catch(std::bad_alloc) {
        mData = tmp;
    }

    memcpy(mData, tmp, mSize);
    delete tmp;
    mSize *= 2;
}

void Cparcel::dumpCparcel()
{
    for(int i = 0; i < mSize; i += 16) {
        printf("%04X ", i);
        for(int j = 0; j < 16; j++) {
            if(i + j >= mSize) {
                break;
            }

            printf("%02X ", (unsigned char)mData[i + j]);
        }

        printf("\n");
    }
}