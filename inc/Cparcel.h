/* brief: a simple data structure serializer for network transfering */

#ifndef __CPARCEL_H__
#define __CPARCEL_H__

#define PARCEL_BASE 64

typedef enum {
    PARCEL_ONE = PARCEL_BASE,
    PARCEL_TWO = PARCEL_BASE << 1,
    PARCEL_THREE = PARCEL_BASE << 2,
} parcel_t;

class Cparcel
{
public:
    Cparcel();
    Cparcel(int t);
    Cparcel(const Cparcel &other);
    Cparcel(Cparcel &&other);
    ~Cparcel();

    Cparcel &operator=(const Cparcel &other);
    Cparcel &operator==(const Cparcel &other) = delete;

    int init();
    int release();

    int getRestSpace();
    const char *getData() const;
    void resize();
    void clear();

    
    Cparcel &write(void *pos, int size);
    // overload for basic built in type
    template <typename T>
    Cparcel &write(T t) {
        T param = t;
        int param_size = sizeof(t);
        return write(&param, param_size);
    }

    int read(void *target, int size);
    /*
    int readInt();
    long readLong();
    float readFloat();
    short readShort();
    char readChar();*/

    void dumpCparcel();
private:
    void indexExpand();

    char *mData;
    int mReadPtr;
    int mWritePtr;
    int mSize;
};

#endif /* __CPARCEL_H__ */
