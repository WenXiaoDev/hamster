/**
 * 仿Android序列化对象 
 */
#include <cstdlib>

#define __CPARCEL_DATA_SIZE__ 256

typedef enum {
    MSG_TYPE = 1,
    GEN_TYPE,
    LONG_TYPE,
    TYPE_MAX,
} parcel_t;

class Cparcel
{
public:
    Cparcel();
    Cparcel(const parcel_t type);
    Cparcel(Cparcel &other);
    ~Cparcel();

    int serialize(const void *param, int size);
    int deserialize(const void *buff, int size);
    char *getData();
    parcel_t getType();
    int getRestSpace();
    void clean();
    static const unsigned int BASE;
private:
    char *_data;
    char *_readPtr;
    char *_writePtr;
    parcel_t _type;
};

const unsigned int Cparcel::BASE = 128;