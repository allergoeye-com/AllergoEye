#include "stdafx.h"
#include "variable.h"
#include "stdio.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


void AddHex(DWORD d, int sz, TString &hex)
{
    const char *sz_8 = "%08x ";
    const char *sz_2 = "%02x ";
    char str[16];

    if (sz == 2)
        sprintf(str, sz_2, (BYTE) d);
    else
        sprintf(str, sz_8, d);
    hex += str;
    hex.Upper();
}

void MakeHex(BYTE *arr, int len, TString &hex, DWORD *address)
{
    if (address)
    {
        hex = "         0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F\r\n";
        AddHex(*address, 8, hex);
        *address += 16;
    }
    for (int j = 0; j < len; ++j)
    {
        if (j && !(j % 16))
        {
            hex += "\r\n";
            if (address)
            {
                AddHex(*address, 8, hex);
                *address += 16;
            }
        }
        AddHex(arr[j], 2, hex);
    }
}

//---------------------------------------------
//
//---------------------------------------------
void Variable::Clear()
{
    switch (m_Type)
    {
        case VAR_STRING:
            delete m_Data.str;
            break;
        case VAR_WSTRING:
            delete m_Data.wstr;
            break;
        case VAR_BUFFER:
            delete m_Data.buff;
            break;
        case VAR_VECTOR:
            delete m_Data.vect;
            break;
        case VAR_GUID:
            delete m_Data.guid;
            break;
        default:;
    }
    m_Type = VAR_EMPTY;
    m_Data.db = 0.0;
}

void Variable::SetDWORD()
{
    switch (m_Type)
    {
        case VAR_STRING:
            delete m_Data.str;
            break;
        case VAR_WSTRING:
            delete m_Data.wstr;
            break;
        case VAR_BUFFER:
            delete m_Data.buff;
            break;
        case VAR_VECTOR:
            delete m_Data.vect;
            break;
        case VAR_GUID:
            delete m_Data.guid;
            break;
        default:
            m_Type = VAR_ULONG;
            m_Data.ulg = operator DWORD();
            return;
    }
    m_Type = VAR_ULONG;
    m_Data.db = 0.0;
}

//---------------------------------------------
//
//---------------------------------------------
Variable::operator GUID()
{
    if (m_Type == VAR_GUID) return *m_Data.guid;

    GUID guid;
    memset(&guid, 0, sizeof(guid));
    return guid;
}

//---------------------------------------------
//
//---------------------------------------------
Variable::operator TString()
{
    if (m_Type == VAR_STRING) return *m_Data.str;
    if (m_Type == VAR_WSTRING) return TString(*m_Data.wstr);
    return TString();
}

//---------------------------------------------
//
//---------------------------------------------
Variable::operator WString()
{
    if (m_Type == VAR_WSTRING) return *m_Data.wstr;
    if (m_Type == VAR_STRING) return WString(*m_Data.str);
    return WString();
}

//---------------------------------------------
//
//---------------------------------------------
Variable::operator const char *()
{
    if (m_Type == VAR_STRING) return *m_Data.str;
    if (m_Type == VAR_WSTRING) return TString(*m_Data.wstr);
    return TString();
}

//---------------------------------------------
//
//---------------------------------------------
Variable::operator const wchar_t *()
{
    if (m_Type == VAR_WSTRING) return *m_Data.wstr;
    if (m_Type == VAR_STRING) return WString(*m_Data.str);
    return WString();
}

//---------------------------------------------
//
//---------------------------------------------
Variable::operator MArray<BYTE>()
{

    if (m_Type == VAR_BUFFER)
        return *m_Data.buff;
    return MArray<BYTE>();
}

//---------------------------------------------
//
//---------------------------------------------
Variable::operator DWORD()
{
    switch (m_Type)
    {
        case VAR_ULONG:
            return m_Data.ulg;

        case VAR_INT:
        case VAR_LONG:
            return (DWORD) m_Data.lg;
        case VAR_UINT:
        case VAR_ALIGNMENT:
            return (DWORD) m_Data.uin;
        case VAR_CHAR :
            return (DWORD) m_Data.ch;
        case VAR_UCHAR :
            return (DWORD) m_Data.uch;
        case VAR_DOUBLE:
            return (DWORD) m_Data.db;
        case VAR_SHORT:
            return (DWORD) m_Data.sh;
        case VAR_USHORT:
            return (DWORD) m_Data.ush;


    }
    return 0;
}

//---------------------------------------------
//
//---------------------------------------------
Variable::operator int()
{
    switch (m_Type)
    {
        case VAR_INT:
        case VAR_LONG:
            return m_Data.lg;
        case VAR_ULONG:
        case VAR_UINT:
        case VAR_ALIGNMENT:
            return (int) m_Data.uin;
        case VAR_CHAR :
            return (int) m_Data.ch;
        case VAR_UCHAR :
            return (int) m_Data.uch;
        case VAR_DOUBLE:
            return (int) m_Data.db;
        case VAR_SHORT:
            return (int) m_Data.sh;
        case VAR_USHORT:
            return (int) m_Data.ush;


    }
    return 0;
}

//---------------------------------------------
//
//---------------------------------------------
Variable::operator double()
{
    switch (m_Type)
    {
        case VAR_DOUBLE:
            return m_Data.db;
        case VAR_CHAR :
            return (double) m_Data.ch;
        case VAR_UCHAR :
            return (double) m_Data.uch;
        case VAR_SHORT:
            return (double) m_Data.sh;
        case VAR_USHORT:
            return (double) m_Data.ush;
        case VAR_LONG:
            return (double) m_Data.lg;
        case VAR_ULONG:
            return (double) m_Data.ulg;
        case VAR_INT:
            return (double) m_Data.in;
        case VAR_UINT:
        case VAR_ALIGNMENT:
            return (double) m_Data.uin;
    }
    return 0;
}


//---------------------------------------------
//
//---------------------------------------------
Variable &Variable::operator=(const Variable &var)
{

    if (m_Type == var.m_Type)
    {
        switch (var.m_Type)
        {
            case VAR_GUID:
                *m_Data.guid = *var.m_Data.guid;
                break;
            case VAR_STRING:
                *m_Data.str = *var.m_Data.str;
                break;
            case VAR_WSTRING:
                *m_Data.wstr = *var.m_Data.wstr;
                break;

            case VAR_BUFFER:
                *m_Data.buff = *var.m_Data.buff;
                break;
            case VAR_VECTOR:
                *m_Data.vect = *(MVector<Variable> *) m_Data.vect;
                break;

            default :
                m_Data.db = var.m_Data.db;
        }
    } else
    {
        Clear();
        m_Type = var.m_Type;
        switch (var.m_Type)
        {
            case VAR_GUID:
                m_Data.guid = new GUID;
                *m_Data.guid = *var.m_Data.guid;
                break;
            case VAR_STRING:
                m_Data.str = new TString(*var.m_Data.str);
                break;
            case VAR_WSTRING:
                m_Data.wstr = new WString(*var.m_Data.wstr);
                break;

            case VAR_BUFFER:
                m_Data.buff = new MArray<BYTE>();
                m_Data.buff->SetAddedRealSize(sizeof(DWORD));
                *m_Data.buff = *var.m_Data.buff;
                break;
            case VAR_VECTOR:
                m_Data.vect = new MVector<Variable>();
                m_Data.vect->SetAddedRealSize(sizeof(Variable));
                *m_Data.vect = *(MVector<Variable> *) m_Data.vect;
                break;

            default :
                m_Data.db = var.m_Data.db;
        }
    }

    return *this;
}

//------------------------------------------------------------------
//
//------------------------------------------------------------------
void Variable::DeleteInVector(int pos, int num)
{
    ASSERT(m_Type == VAR_VECTOR);
    if (m_Type != VAR_VECTOR)
        return;
    m_Data.vect->Remove(pos, num);


}


//------------------------------------------------------------------
//
//------------------------------------------------------------------
int Variable::Dim(int n /*= -1*/)
{
    if (n < 0)
        return m_Type == VAR_VECTOR ? m_Data.vect->GetLen() : 0;

    if (m_Type != VAR_VECTOR)
    {
        Clear();
        m_Data.vect = new MVector<Variable>();
        m_Data.vect->SetAddedRealSize(sizeof(Variable));
        m_Type = VAR_VECTOR;
    }
    m_Data.vect->SetLen(n);
    return n;

}

//---------------------------------------------
//
//---------------------------------------------
Variable &Variable::operator=(const char *str)
{
    if (m_Type == VAR_STRING)
        *m_Data.str = str;
    else
    {
        Clear();
        m_Data.str = new TString(str);
        m_Type = VAR_STRING;
    }
    return *this;
}

//---------------------------------------------
//
//---------------------------------------------
Variable &Variable::operator=(const wchar_t *str)
{
    if (m_Type == VAR_WSTRING)
        *m_Data.wstr = str;
    else
    {
        Clear();
        m_Data.wstr = new WString(str);
        m_Type = VAR_WSTRING;
    }
    return *this;
}

//---------------------------------------------
//
//---------------------------------------------
Variable &Variable::operator=(const GUID &guid)
{
    if (m_Type == VAR_GUID)
        *m_Data.guid = guid;
    else
    {
        Clear();
        m_Data.guid = new GUID;
        *m_Data.guid = guid;
        m_Type = VAR_GUID;
    }
    return *this;
}

//---------------------------------------------
//
//---------------------------------------------
Variable &Variable::operator=(double db)
{
    if (m_Type != VAR_DOUBLE)
    {
        Clear();
        m_Type = VAR_DOUBLE;
    }
    m_Data.db = db;
    return *this;
}

//---------------------------------------------
//
//---------------------------------------------
Variable &Variable::operator=(short sh)
{
    if (m_Type != VAR_SHORT)
    {
        Clear();
        m_Type = VAR_SHORT;
    }
    m_Data.sh = sh;
    return *this;
}

//---------------------------------------------
//
//---------------------------------------------
Variable &Variable::operator=(USHORT ush)
{
    if (m_Type != VAR_USHORT)
    {
        Clear();
        m_Type = VAR_USHORT;
    }
    m_Data.ush = ush;
    return *this;
}
/*
//---------------------------------------------
//
//---------------------------------------------
Variable &Variable::operator = (int lg)
{
	if (m_Type != VAR_LONG)
	{
		Clear();
		m_Type = VAR_LONG;
	}
	m_Data.lg = lg;
	return *this;
}
 */
//---------------------------------------------
//
//---------------------------------------------
Variable &Variable::operator=(DWORD ulg)
{
    if (m_Type != VAR_ULONG)
    {
        Clear();
        m_Type = VAR_ULONG;
    }
    m_Data.ulg = ulg;
    return *this;
}

//---------------------------------------------
//
//---------------------------------------------
Variable &Variable::operator=(int in)
{
    if (m_Type != VAR_INT)
    {
        Clear();
        m_Type = VAR_INT;
    }
    m_Data.in = in;
    return *this;
}
//---------------------------------------------
//
//---------------------------------------------
#ifdef WIN32
Variable &Variable::operator = (UINT uin)
{
    if (m_Type != VAR_UINT)
    {
        Clear();
        m_Type = VAR_UINT;
    }
    m_Data.uin = uin;
    return *this;
}
#endif

//---------------------------------------------
//
//---------------------------------------------
Variable &Variable::operator=(char ch)
{
    if (m_Type != VAR_CHAR)
    {
        Clear();
        m_Type = VAR_CHAR;
    }
    m_Data.ch = ch;
    return *this;
}

//---------------------------------------------
//
//---------------------------------------------
Variable &Variable::operator=(BYTE uch)
{
    if (m_Type != VAR_UCHAR)
    {
        Clear();
        m_Type = VAR_UCHAR;
    }
    m_Data.uch = uch;
    return *this;
}

//---------------------------------------------
//
//---------------------------------------------
BOOL Variable::operator==(const Variable &var)
{
    BOOL is = 0;
    switch (var.m_Type)
    {
        case VAR_CHAR :
        case VAR_UCHAR :
        case VAR_DOUBLE:
        case VAR_SHORT:
        case VAR_USHORT:
        case VAR_LONG:
        case VAR_ULONG:
        case VAR_INT:
        case VAR_UINT:
        case VAR_ALIGNMENT:
            is = true;
    }

    if (IsNumber() && is)
    {
        double f, f1;
        if (m_Type == VAR_DOUBLE)
            f = operator double();
        else
            f = operator DWORD();

        if (var.m_Type == VAR_DOUBLE)
            f1 = (double) (Variable) var;
        else
            f1 = (DWORD) (Variable) var;
        return f1 == f;

    }
    if (m_Type == var.m_Type)
    {
        switch (m_Type)
        {
            case VAR_WSTRING:
                return *m_Data.wstr == *var.m_Data.wstr;
                break;
            case VAR_STRING:
                return *m_Data.str == *var.m_Data.str;
                break;
            case VAR_BUFFER:
                return *m_Data.buff == *var.m_Data.buff;
            case VAR_VECTOR:
            {
                if (m_Data.vect->GetLen() == var.m_Data.vect->GetLen())
                {
                    int dim = m_Data.vect->GetLen();
                    for (int i = 0; i < dim; ++i)
                        if ((*m_Data.vect)[i] != (*var.m_Data.vect)[i])
                            return FALSE;
                    return TRUE;
                }
                break;

            }
            case VAR_GUID :
                return IsEqualGUID(*m_Data.guid, *var.m_Data.guid);

        }
    }
    return FALSE;
}

//---------------------------------------------
//
//---------------------------------------------
Variable &Variable::operator=(const MArray<BYTE> &var)
{
    Clear();
    m_Data.buff = new MArray<BYTE>();
    m_Data.buff->SetAddedRealSize(sizeof(DWORD));
    *m_Data.buff = var;
    m_Type = VAR_BUFFER;
    return *this;
}

//---------------------------------------------
//
//---------------------------------------------
void Variable::Serialize1(MArray<BYTE> &b, UINT &algnBuffer)
{
    b.Append((BYTE *) &m_Type, sizeof(m_Type));
    switch (m_Type)
    {
        case VAR_CHAR :
            b.Append((BYTE *) &m_Data.ch, 1);
            break;
        case VAR_UCHAR :
            b.Append((BYTE *) &m_Data.uch, 1);
            break;
        case VAR_DOUBLE:
            b.Append((BYTE *) &m_Data.db, sizeof(m_Data.db));
            break;
        case VAR_SHORT:
            b.Append((BYTE *) &m_Data.sh, sizeof(m_Data.sh));
            break;
        case VAR_USHORT:
            b.Append((BYTE *) &m_Data.ush, sizeof(m_Data.ush));
            break;
        case VAR_LONG:
            b.Append((BYTE *) &m_Data.lg, sizeof(m_Data.lg));
            break;
        case VAR_ULONG:
            b.Append((BYTE *) &m_Data.ulg, sizeof(m_Data.ulg));
            break;
        case VAR_INT:
            b.Append((BYTE *) &m_Data.in, sizeof(m_Data.in));
            break;
        case VAR_UINT:
        case VAR_ALIGNMENT:
            b.Append((BYTE *) &m_Data.uin, sizeof(m_Data.uin));
            break;
        case VAR_STRING:
        {
            char *str = m_Data.str->GetPtr();
            int n = strlen(str);
            if (!str || !n)
            {
                BYTE c = 0;
                b.Append(&c, 1);
            } else
                b.Append((BYTE *) m_Data.str->GetPtr(), n + 1);
        }
            break;
        case VAR_WSTRING:
        {
            wchar_t *wstr = m_Data.wstr->GetPtr();
            int n = wstr ? wcslen(wstr) : 0;
            if (!wstr || !n)
            {
                wchar_t c = 0;
                b.Append((BYTE *) &c, sizeof(wchar_t));
            } else
                b.Append((BYTE *) m_Data.wstr->GetPtr(), (n + 1) * sizeof(wchar_t));
        }
            break;
        case VAR_GUID:
            b.Append((BYTE *) m_Data.guid, sizeof(GUID));
            break;

        case VAR_BUFFER:
        {
            int len = m_Data.buff->GetLen();
            b.Append((BYTE *) &len, sizeof(int));
            if ((b.GetLen() % algnBuffer))
                b.SetLen(b.GetLen() + algnBuffer - (b.GetLen() % algnBuffer));

            b += *m_Data.buff;
            if ((b.GetLen() % algnBuffer))
                b.SetLen(b.GetLen() + algnBuffer - (b.GetLen() % algnBuffer));
            break;
        }
        case VAR_VECTOR:
        {
            int len = m_Data.vect->GetLen();
            b.Append((BYTE *) &len, sizeof(int));
            Variable *ptr = m_Data.vect->GetPtr();
            while (len--)
                (ptr++)->Serialize1(b, algnBuffer);
        }
    }
}

//------------------------------------------
//
//------------------------------------------
void Variable::UnSerialize(BYTE *b, int blen, int &pos, UINT &algnBuffer, BOOL fNoUnserializeBuffer)
{
    //Clear();
    BYTE *ptr = b;
    if (pos + sizeof(TYPE_VAR) > (UINT) blen)
        goto error;
    if (m_Type != VAR_EMPTY && m_Type != (TYPE_VAR) Mem2Long(ptr + pos))
        Clear();
    m_Type = (TYPE_VAR) Mem2Long(ptr + pos);
    pos += sizeof(TYPE_VAR);

    switch (m_Type)
    {
        case VAR_WSTRING:
        {
            //	wchar_t *wstr = (wchar_t *)(ptr + pos);
            //	int n;
            if (m_Data.wstr)
            {
                //	*m_Data.wstr = wstr;
                //	pos += (wcslen(wstr) + 1) * sizeof(wchar_t);
            } else
                //if (wstr && (n = wcslen(wstr)))
            {
                m_Data.wstr = new WString(); //wstr);
                //	pos += (n + 1) * sizeof(wchar_t);
            }
//		else
//		{
//			m_Data.wstr = new WString;
//			pos += sizeof(wchar_t);
            //
            //	}
            while (Mem2UShort(ptr + pos))
            {
                m_Data.wstr->Add() = (wchar_t) Mem2UShort(ptr + pos);
                pos += 2;

            }
            m_Data.wstr->Add() = 0;
            pos += sizeof(wchar_t);

        }
            break;
        case VAR_STRING:
        {
            char *str = (char *) (ptr + pos);
            int n = 0;
            if (m_Data.str)
            {
                *m_Data.str = str;
                pos += strlen(str) + 1;
            } else if (str && (n = strlen(str)))
            {
                m_Data.str = new TString(str);
                pos += n + 1;
            } else
            {
                m_Data.str = new TString;
                ++pos;

            }
            break;
        }
        case VAR_BUFFER:
        {
            if (pos + sizeof(int) > (UINT) blen)
                goto error;
            int len = Mem2Int(ptr + pos);
            pos += sizeof(int);
            if (pos + len > blen)
                goto error;
            if (!m_Data.buff)
            {
                m_Data.buff = new MArray<BYTE>();
                m_Data.buff->SetAddedRealSize(sizeof(DWORD));
            }
            if ((pos % algnBuffer))
                pos += (algnBuffer - (pos % algnBuffer));
            if (!fNoUnserializeBuffer)
                m_Data.buff->Copy(ptr + pos, len);
            else
            {
                m_Data.buff->SetLen(0);
                m_Data.buff->Append((BYTE *) &len, sizeof(UINT));
                m_Data.buff->Append((BYTE *) &pos, sizeof(UINT));
            }
            pos += len;
            if ((pos % algnBuffer))
                pos += algnBuffer - (pos % algnBuffer);
            break;
        }
        case VAR_VECTOR:
        {
            if (pos + sizeof(int) > (UINT) blen)
                goto error;
            int len = Mem2Int(ptr + pos);
            pos += sizeof(int);
            if (!m_Data.vect)
            {
                m_Data.vect = new MVector<Variable>();
                m_Data.vect->SetAddedRealSize(sizeof(Variable));
            }
            m_Data.vect->SetLen(len);
            Variable *bptr = m_Data.vect->GetPtr();
            while (len--)
                (bptr++)->UnSerialize(b, blen, pos, algnBuffer, fNoUnserializeBuffer);
            break;
        }
        case VAR_GUID :
            if (pos + sizeof(GUID) > (UINT) blen)
                goto error;
            if (!m_Data.guid)
                m_Data.guid = new GUID;
            memcpy(m_Data.guid, ptr + pos, sizeof(GUID));
            pos += sizeof(GUID);
            break;

        case VAR_CHAR :
            if (pos + sizeof(char) > (UINT) blen)
                goto error;
            m_Data.ch = *(char *) (ptr + pos);
            ++pos;
            break;
        case VAR_UCHAR :
            if (pos + sizeof(char) > (UINT) blen)
                goto error;
            m_Data.ch = *(ptr + pos);
            ++pos;
            break;
        case VAR_DOUBLE:
            if (pos + sizeof(double) > (UINT) blen)
                goto error;
            memcpy(&m_Data.db, ptr + pos, sizeof(double));
            pos += sizeof(double);
            break;
        case VAR_USHORT:
        case VAR_SHORT:
            if (pos + sizeof(short) > (UINT) blen)
                goto error;
            m_Data.sh = Mem2Short(ptr + pos);
            pos += sizeof(m_Data.sh);
            break;
        case VAR_LONG:
        case VAR_ULONG:
        case VAR_INT:
        case VAR_UINT:
        case VAR_ALIGNMENT:
            if (pos + sizeof(int) > (UINT) blen)
                goto error;
            m_Data.lg = Mem2Long(ptr + pos);
            pos += sizeof(int);
            break;
        default:
        error:
            Clear();
    }
}

bool Variable::IsNumber()
{
    switch (m_Type)
    {
        case VAR_CHAR :
        case VAR_UCHAR :
        case VAR_DOUBLE:
        case VAR_SHORT:
        case VAR_USHORT:
        case VAR_LONG:
        case VAR_ULONG:
        case VAR_INT:
        case VAR_UINT:
        case VAR_ALIGNMENT:
            return true;

    }
    return false;
}

TString Variable::GetHexString()
{
    TString ret;
    switch (m_Type)
    {
        case VAR_CHAR :
            ret.Format("0x%02x", m_Data.ch);
            break;
        case VAR_UCHAR :
            ret.Format("0x%02x", m_Data.uch);
            break;
        case VAR_DOUBLE:
            ret.Format("0x%.16x", m_Data.db);
            break;
        case VAR_SHORT:
            ret.Format("0x%04x", m_Data.sh);
            break;
        case VAR_USHORT:
            ret.Format("0x%04x", m_Data.ush);
            break;
        case VAR_LONG:
            ret.Format("0x%08x", m_Data.lg);
            break;
        case VAR_ULONG:
            ret.Format("0x%08x", m_Data.ulg);
            break;
        case VAR_INT:
            ret.Format("0x%08x", m_Data.in);
            break;
        case VAR_UINT:
        case VAR_ALIGNMENT:
            ret.Format("0x%08x", m_Data.uin);
            break;
        case VAR_BUFFER:
        {
            DWORD addr = 0;
            MakeHex(m_Data.buff->GetPtr(), m_Data.buff->GetLen(), ret, &addr);
        }
            break;
        case VAR_STRING:
        {
            DWORD addr = 0;
            MakeHex((BYTE *) m_Data.str->GetPtr(), m_Data.str->GetLen(), ret, &addr);
        }
            break;
        case VAR_WSTRING:
        {
            DWORD addr = 0;
            MakeHex((BYTE *) m_Data.wstr->GetPtr(), m_Data.wstr->GetLen() * sizeof(wchar_t), ret,
                    &addr);
        }
            break;
    }
    return ret;
}

//---------------------------------------------
//
//---------------------------------------------
void Variable::UnSerialize(MArray<BYTE> &b, BOOL fNoUnserializeBuffer, UINT *algn)
{
    Clear();
    if (b.GetLen() < sizeof(int)) return;
    int pos = 0;
    UINT algnBuffer = 1;
    TYPE_VAR tp = (TYPE_VAR) Mem2Long(b.GetPtr());
    if (tp == VAR_ALIGNMENT)
    {
        pos += sizeof(TYPE_VAR);
        algnBuffer = Mem2Long(b.GetPtr() + pos);
        pos += sizeof(UINT);
    }
    if (algn) *algn = algnBuffer;
    UnSerialize(b.GetPtr(), b.GetLen(), pos, algnBuffer, fNoUnserializeBuffer);
}

DWORD Variable::ReadNext(MFILE *hFile, int offset, const void *s, DWORD size)
{
    DWORD n, rw;
    if ((DWORD) offset != hFile->GetSeek())
        if (hFile->SetSeek(offset) != (DWORD) offset)
        {
            n = GetLastError();
            return false;
        }

    if ((n = hFile->Read(-1, (void *) s, size)) != (DWORD) size)
    {

        DWORD dw = GetLastError();
        if (!rw || !n)
            return 0;
    }
    return n;
}

//---------------------------------------------
//
//---------------------------------------------
void Variable::UnSerialize(MFILE *hFile, BOOL fNoUnserializeBuffer, UINT *algn, int __offset)
{
    int size;
    if (hFile->File() > 0 && (size = hFile->GetSize()) > 0)
    {
        hFile->SetSeek(0);
        int pos = __offset;
        UINT algnBuffer = 1;
        TYPE_VAR tp = (TYPE_VAR) File2Long(hFile, pos);
        if (tp == VAR_ALIGNMENT)
        {
            pos += sizeof(TYPE_VAR);
            algnBuffer = File2Long(hFile, pos);
            pos += sizeof(UINT);
        }
        if (algn) *algn = algnBuffer;
        UnSerialize(hFile, size, pos, algnBuffer, fNoUnserializeBuffer);

    }
}

//------------------------------------------
//
//------------------------------------------
void
Variable::UnSerialize(MFILE *hFile, int blen, int &pos, UINT &algnBuffer, BOOL fNoUnserializeBuffer)
{
    //Clear();

    if (pos + sizeof(TYPE_VAR) > (UINT) blen)
        goto error;
    if (m_Type != VAR_EMPTY && m_Type != (TYPE_VAR) File2Long(hFile, pos))
        Clear();
    m_Type = (TYPE_VAR) File2Long(hFile, pos);
    pos += sizeof(TYPE_VAR);

    switch (m_Type)
    {
        case VAR_WSTRING:
        {
            if (!m_Data.wstr)
                m_Data.wstr = new WString();
            wchar_t t;
            while ((t = File2UShort(hFile, pos)))
            {
                m_Data.wstr->Add(t);
                pos += sizeof(wchar_t);

            }
            m_Data.wstr->Add() = 0;
            pos += sizeof(wchar_t);

        }
            break;
        case VAR_STRING:
        {
            if (!m_Data.str)
                m_Data.str = new TString();

            char t;
            while ((t = File2Byte(hFile, pos)))
            {
                m_Data.str->Add(t);
                ++pos;

            }
            m_Data.str->Add() = 0;
            ++pos;
            break;
        }
        case VAR_BUFFER:
        {
            if (pos + sizeof(int) > (UINT) blen)
                goto error;
            int len = File2Long(hFile, pos);
            pos += sizeof(int);
            if (pos + len > blen)
                goto error;
            if (!m_Data.buff)
            {
                m_Data.buff = new MArray<BYTE>();
                m_Data.buff->SetAddedRealSize(sizeof(DWORD));
            }
            if ((pos % algnBuffer))
                pos += (algnBuffer - (pos % algnBuffer));
            if (!fNoUnserializeBuffer)
            {
                if (len)
                {
                    m_Data.buff->SetLen(len);
                    if (!ReadNext(hFile, pos, m_Data.buff->GetPtr(), len))
                        goto error;
                }
            } else
            {
                m_Data.buff->SetLen(0);
                m_Data.buff->Append((BYTE *) &len, sizeof(UINT));
                m_Data.buff->Append((BYTE *) &pos, sizeof(UINT));
            }
            pos += len;
            if ((pos % algnBuffer))
                pos += algnBuffer - (pos % algnBuffer);
            break;
        }
        case VAR_VECTOR:
        {
            if (pos + sizeof(int) > (UINT) blen)
                goto error;
            int len = File2Long(hFile, pos);
            pos += sizeof(int);
            if (!m_Data.vect)
            {
                m_Data.vect = new MVector<Variable>();
                m_Data.vect->SetAddedRealSize(sizeof(Variable));
            }
            m_Data.vect->SetLen(len);
            Variable *bptr = m_Data.vect->GetPtr();
            while (len--)
                (bptr++)->UnSerialize(hFile, blen, pos, algnBuffer, fNoUnserializeBuffer);
            break;
        }
        case VAR_GUID :
            if (pos + sizeof(GUID) > (UINT) blen)
                goto error;
            if (!m_Data.guid)
                m_Data.guid = new GUID;
            if (!ReadNext(hFile, pos, m_Data.guid, sizeof(GUID)))
                goto error;
            pos += sizeof(GUID);
            break;

        case VAR_CHAR :
            if (pos + sizeof(char) > (UINT) blen)
                goto error;
            m_Data.ch = File2Byte(hFile, pos);
            ++pos;
            break;
        case VAR_UCHAR :
            if (pos + sizeof(char) > (UINT) blen)
                goto error;
            m_Data.ch = File2Byte(hFile, pos);
            break;
        case VAR_DOUBLE:
            if (pos + sizeof(double) > (UINT) blen)
                goto error;
            m_Data.db = File2Double(hFile, pos);
            pos += sizeof(double);
            break;
        case VAR_USHORT:
        case VAR_SHORT:
            if (pos + sizeof(short) > (UINT) blen)
                goto error;
            m_Data.sh = File2UShort(hFile, pos);
            pos += sizeof(short);
            break;
        case VAR_LONG:
        case VAR_ULONG:
        case VAR_INT:
        case VAR_UINT:
        case VAR_ALIGNMENT:
            if (pos + sizeof(int) > (UINT) blen)
                goto error;
            m_Data.lg = File2Long(hFile, pos);
            pos += sizeof(int);
            break;
        default:
        error:
            Clear();
    }
}

//---------------------------------------------
//
//---------------------------------------------
void Variable::Serialize(MArray<BYTE> &b, UINT *_algn)
{
    UINT algn = _algn ? *_algn : 1;
    if (_algn)
    {
        TYPE_VAR tp = VAR_ALIGNMENT;
        b.Append((BYTE *) &tp, sizeof(tp));
        b.Append((BYTE *) _algn, sizeof(UINT));
    }
    Serialize1(b, algn);

}

int Variable::SizeOf()
{
    int size;
    SizeOf(size);
    return size;
}

//---------------------------------------------
//
//---------------------------------------------
void Variable::SizeOf(int &size)
{
    size += sizeof(m_Type);
    switch (m_Type)
    {
        case VAR_CHAR :
        case VAR_UCHAR :
            size += 1;
            break;
        case VAR_DOUBLE:
            size += sizeof(m_Data.db);
            break;
        case VAR_SHORT:
            size += sizeof(m_Data.sh);
            break;
        case VAR_USHORT:
            size += sizeof(m_Data.ush);
            break;
        case VAR_LONG:
            size += sizeof(m_Data.lg);
            break;
        case VAR_ULONG:
            size += sizeof(m_Data.ulg);
            break;
        case VAR_INT:
            size += sizeof(m_Data.in);
            break;
        case VAR_UINT:
        case VAR_ALIGNMENT:
            size += sizeof(m_Data.uin);
            break;
        case VAR_STRING:
        {
            char *str = m_Data.str->GetPtr();
            if (!str || !strlen(str))
                size += 1;
            else
                size += strlen(str) + 1;
        }
            break;
        case VAR_WSTRING:
        {
            wchar_t *wstr = m_Data.wstr->GetPtr();
            if (!wstr || !wcslen(wstr))
                size += sizeof(wchar_t);
            else
                size += sizeof(wchar_t) * (wcslen(wstr) + 1);
        }
            break;
        case VAR_GUID:
            size += sizeof(GUID);
            break;

        case VAR_BUFFER:
        {
            size += m_Data.buff->GetLen();
            break;
        }
        case VAR_VECTOR:
        {
            int len = m_Data.vect->GetLen();
            size += sizeof(int);
            for (int i = 0; i < len; ++i)
                (*m_Data.vect)[i].SizeOf(size);
        }
    }
}

int Variable::SizeOfVariant()
{
    int size;
    SizeOfVariant(size);
    return size;
}

//---------------------------------------------
//
//---------------------------------------------
void Variable::SizeOfVariant(int &size)
{
    size += sizeof(Variable);
    switch (m_Type)
    {
        case VAR_STRING:
        {
            size += sizeof(TString);
            char *str = m_Data.str->GetPtr();
            if (str)
                size += strlen(str) + 1;
        }
            break;
        case VAR_WSTRING:
        {
            wchar_t *wstr = m_Data.wstr->GetPtr();
            size += sizeof(WString);
            if (wstr)
                size += sizeof(wchar_t) * (wcslen(wstr) + 1);
        }
            break;
        case VAR_GUID:
            size += sizeof(GUID);
            break;
        case VAR_BUFFER:
        {
            size += m_Data.buff->GetLen();
            break;
        }
        case VAR_VECTOR:
        {
            int len = m_Data.vect->GetLen();
            size += sizeof(MVector<Variable>);
            for (int i = 0; i < len; ++i)
                (*m_Data.vect)[i].SizeOf(size);
        }
    }
}
