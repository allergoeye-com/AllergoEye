#include "stdafx.h"
#include "MDNSStruct.h"
#include "lzwcomp.h"

namespace MDNSStruct
{
    void RespHeader::Init(int nRec)
    {
        ID = 0;
        Flags = htons(0x84);
        QuestionCount = 0;
        AnswerCount = 0;
        NameServerRecords = 0;
        AdditionalRecords = htons(nRec);
    }

    void RespHeader::Read(char *d)
    {
        memcpy(&ID, d, sizeof(USHORT));
        d += sizeof(USHORT);
        memcpy(&Flags, d, sizeof(USHORT));
        d += sizeof(USHORT);
        memcpy(&QuestionCount, d, sizeof(USHORT));
        d += sizeof(USHORT);
        memcpy(&AnswerCount, d, sizeof(USHORT));
        d += sizeof(USHORT);
        memcpy(&NameServerRecords, d, sizeof(USHORT));
        d += sizeof(USHORT);
        memcpy(&AdditionalRecords, d, sizeof(USHORT));
        d += sizeof(USHORT);
        Flags = htons(Flags);
        QuestionCount = htons(QuestionCount);
        AnswerCount = htons(AnswerCount);
        NameServerRecords = htons(NameServerRecords);
        AdditionalRecords = htons(AdditionalRecords);
    }


    void MdnsTextRecord::Init(LPCSTR txt)
    {
        Type = htons(0x10);
        Class = 0x0180;
        len = htons((USHORT) strlen(txt) + 1);
        memcpy(data, txt, strlen(txt));
    }

    void MdnsTextRecord::Init(BYTE *txt, int _len)
    {
        Type = htons(0x11);
        Class = 0x0180;
        len = htons((USHORT) _len);
        memcpy(data, txt, _len);
    }

    void MdnsRecord::Read(char *d)
    {
        memcpy(&Type, d, sizeof(USHORT));
        d += sizeof(USHORT);
        memcpy(&Class, d, sizeof(int));
        d += sizeof(int);
        memcpy(&TTL, d, sizeof(int));
        d += sizeof(int);
        memcpy(&len, d, sizeof(USHORT));
        d += sizeof(USHORT);
        len = htons(len);
    }

    void AddRecord(MArray<char> &buff, BYTE *data, int len)
    {
        int n = int(sizeof(MdnsRecord) + len);
        int N = buff.GetLen();
        buff.SetLen(N + n);
        ((MdnsTextRecord *) (buff.GetPtr() + N))->Init(data, len);


    }

    void AddRecord(MArray<char> &buff, LPCSTR data)
    {
        int n = int(sizeof(MdnsTextRecord) + strlen(data));
        int N = buff.GetLen();
        buff.SetLen(N + n);
        ((MdnsTextRecord *) (buff.GetPtr() + N))->Init(data);


    }
    void CreateMdnsRecords2W(TString &us1, TString &us2, MArray<jchar> &ws2, TString &us3, MArray<char> &buff)
    {
        TString local = "local";
        buff.SetLen(sizeof(RespHeader) + 1);
        ((RespHeader *) (buff.GetPtr()))->Init(3);
        buff[sizeof(RespHeader)] = us1.StrLen();
        buff.Append(us1.GetPtr(), us1.StrLen());
        buff.Add() = local.StrLen();
        buff.Append(local.GetPtr(), local.StrLen());
        AddRecord(buff, us2);
        AddRecord(buff, (BYTE *)ws2.GetPtr(), ws2.GetLen() * 2);
        AddRecord(buff, us3);
    }
    void CreatePacket(TString &domain, MVector<TString> &arr,  MArray<char> &buff)
    {
        TString local = "local";
        buff.SetLen(sizeof(RespHeader) + 1);
        int i = 0, l = arr.GetLen();;
        ((RespHeader *) (buff.GetPtr()))->Init(arr.GetLen());
        buff[sizeof(RespHeader)] = domain.StrLen();
        buff.Append(domain.GetPtr(), domain.StrLen());
        buff.Add() = local.StrLen();
        buff.Append(local.GetPtr(), local.StrLen());
        for (; i < l; ++i)
        {
            AddRecord(buff, arr[i]);
        }
        /*    MArray<BYTE> dst;
            if (l > 1)
            {
                PackLZW pack((BYTE *)arr[i - 1].GetPtr(),arr[i - 1].StrLen(), dst);
                pack.Pack();
                StringUtils s = "PLZ2";
                dst.Insert(0, (BYTE *)s.GetPtr(), 4);
                AddRecord(buff, dst.GetPtr(), dst.GetLen());
            }
            */

    }
    void CreatePacket(TString &domain, MVector<MArray<jchar> > &arr, MArray<char> &buff)
    {
        TString local = "local";
        buff.SetLen(sizeof(RespHeader) + 1);
        int i = 0, l = arr.GetLen();;
        ((RespHeader *) (buff.GetPtr()))->Init(arr.GetLen());
        buff[sizeof(RespHeader)] = domain.StrLen();
        buff.Append(domain.GetPtr(), domain.StrLen());
        buff.Add() = local.StrLen();
        buff.Append(local.GetPtr(), local.StrLen());
        for (; i < l; ++i)
        {
            AddRecord(buff, (BYTE *)arr[i].GetPtr(), arr[i].GetLen() * 2);
        }
        /*    MArray<BYTE> dst;
            if (l > 1)
            {
                PackLZW pack((BYTE *)arr[i - 1].GetPtr(),arr[i - 1].StrLen(), dst);
                pack.Pack();
                StringUtils s = "PLZ2";
                dst.Insert(0, (BYTE *)s.GetPtr(), 4);
                AddRecord(buff, dst.GetPtr(), dst.GetLen());
            }
            */

    }

    bool ReadMDNS(char *data, int len, MVector<TString> &sarr, MVector<MArray<jchar> > &sarr1)
    {
        if (data)
        {
            char *rec = data;
            RespHeader head;
            head.Read(data);
            //if (head.Flags != 0x84)
            //	return false;
            int nRec = head.AdditionalRecords;
            size_t pos = 0;
            TString str;
            head.Read(rec);
            rec += sizeof(RespHeader);
            while (*rec)
            {

                char l = *rec;
                if (l > 0)
                {
                    ++rec;
                    str.Copy(rec, l);
                    str.Add() = 0;
                    if (str.StrLen())
                        sarr.Add(str);
                    rec = rec + l;

                } else
                    break;


            }
            while (nRec--)
            {
                pos = (rec - data);
                MdnsRecord record;
                char *res = 0;
                record.Read(rec);

                if (pos + sizeof(MdnsRecord) + record.len > len)
                    break;
                rec += sizeof(MdnsRecord);

                if (record.Type == 0x1000)
                {
                    res = (char *) _alloca(record.len + 1);
                    memcpy(res, rec, record.len);
                    res[record.len] = 0;
                    sarr.Add() = res;
                } else
                if (record.Type == 0x1100)
                    sarr1.Add().Copy((jchar *)rec, record.len/2);
                else
                    return 0;
                rec += record.len;
            }
        }
        return true;
    }

};
