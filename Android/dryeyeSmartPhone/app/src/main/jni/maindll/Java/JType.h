#ifndef __JType__
#define __JType__
enum JTYPE {
    TJEMPTY, TJOBJECT, TJSTRING, TJINT, TJFLOAT, TJSHORT, TJBYTE, TJCHAR, TJBOOL, TJDOUBLE, TJLONG
};
#pragma pack(push)
#pragma pack(4)

struct JType {
    JType()
    { type = TJEMPTY; }

    JType(const JType &c)
    { type = c.type; }

    JTYPE type;
};

#pragma pack(pop)
#endif
