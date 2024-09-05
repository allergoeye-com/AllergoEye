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

    bool ReadMDNS(char *data, int len, MVector<UString> &sarr, MVector<MArray<wchar_t> > &sarr2);

    void CreatePacket(UString &domain, MVector<UString> &arr, MArray<char> &buff);
    void CreatePacket(UString &domain, MVector<MArray<wchar_t> > &sarr2, MArray<char> &buff);
    void AddRecord(MArray<char>& buff, BYTE* data, int len);
    void AddRecord(MArray<char>& buff, LPCSTR data);



#pragma pack(pop)
};

