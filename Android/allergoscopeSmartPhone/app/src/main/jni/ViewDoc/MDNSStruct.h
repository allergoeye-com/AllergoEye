#pragma once
namespace MDNSStruct
{
#pragma pack(push)
#pragma pack(1)

    struct RespHeader {
        USHORT ID;
        USHORT Flags;
        USHORT QuestionCount;
        USHORT AnswerCount;
        USHORT NameServerRecords;
        USHORT AdditionalRecords;

        void Init(int nRec);

        void Read(char *d);
    };

    struct MdnsRecord {

        USHORT Type;
        int Class;
        int TTL;
        USHORT len;

        void Read(char *d);
    };

    struct MdnsTextRecord : public MdnsRecord {

        BYTE data[1];

        void Init(LPCSTR txt);

        void Init(BYTE *txt, int len);

    };

    bool ReadMDNS(char *data, int len, MVector<TString> &sarr, MVector<MArray<jchar> > &sarr2);
    void CreateMdnsRecords2W(TString &us1, TString &us2, MArray<jchar> &ws2, TString &us3, MArray<char> &buff);
    void CreatePacket(TString &domain, MVector<TString> &arr, MArray<char> &buff);
    void CreatePacket(TString &domain, MVector<MArray<jchar> > &sarr2, MArray<char> &buff);


#pragma pack(pop)
};

