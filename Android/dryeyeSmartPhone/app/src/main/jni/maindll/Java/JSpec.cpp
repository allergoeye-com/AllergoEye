#include "stdafx.h"
#include "JSpec.h"
#include "mstring.h"

//----------------------------------------------
//
//----------------------------------------------
JSpec::JSpec(const JSpec &j)
{
    nVect = j.nVect;
    type = j.type;
    name = 0;
    if (j.name)
    {
        size_t n = strlen(j.name) + 1;
        name = (char *) HMALLOC(n);
        strcpy(name, j.name);
    }
}

JSpec::JSpec(LPCSTR _name, int _nVect, JTYPE _type)
{
    nVect = _nVect;
    type = _type;
    name = 0;
    if (_name)
    {
        size_t n = strlen(_name) + 1;
        name = (char *) HMALLOC(n);
        strcpy(name, _name);
    }
}

void JSpec::Init(LPCSTR _name, int _nVect, JTYPE _type)
{
    Clear();
    nVect = _nVect;
    type = _type;
    name = 0;
    if (_name)
    {
        size_t n = strlen(_name) + 1;
        name = (char *) HMALLOC(n);
        strcpy(name, _name);
    }
}

//----------------------------------------------
//
//----------------------------------------------
void JSpec::Copy(const JSpec &j)
{
    if (name)
        HFREE(name);
    nVect = j.nVect;
    type = j.type;
    name = 0;
    if (j.name)
    {
        size_t n = strlen(j.name) + 1;
        name = (char *) HMALLOC(n);
        strcpy(name, j.name);
    }


}

void JSpec::toFormGetTypeName(TString &in, TString &out)
{
    MVector<TString> t;
    in.Tokenize(" ", t);
    TString &s = t[t.GetLen() - 1];
    char *sp = s.GetPtr();
    int i = 0, nV = 0;
    while (sp[i] == '[')
    {
        ++nV;
        ++i;
    }
    switch (sp[i])
    {
        case 'I':
            out = "int";
            break;
        case 'F':
            out = "float";
            break;
        case 'S':
            out = "short";
            break;
        case 'B':
            out = "byte";
            break;
        case 'C':
            out = "char";
            break;
        case 'Z':
            out = "bool";
            break;
        case 'D':
            out = "double";
            break;
        case 'J':
            out = "long";
            break;
        case 'L':
            ++i;
            out = sp + i;
            out[(int) strlen(sp + i) - 1] = 0;
            break;
        default:
            out = sp + i;

    }
    for (int j = 0; j < nV; ++j)
        out += "[]";

}/*
BOOL JSpec::Init(TString &in)
{
	Clear();
	MVector<TString> t;
	in.Tokenize(" ", t);
	TString &s = t[t.GetLen() - 1];
	char *sp = s.GetPtr();
	int i = 0;
	nVect = 0;
	while(sp[i] == '[')
	{
		++nVect;
		++i;
	}
	switch(sp[i])
	{
	case 'I':
		type = TJINT;
		break;
	case 'F':
		type = TJFLOAT;
		break;
	case 'S':
		type = TJSHORT;
		break;
	case 'B':
		type = TJBYTE;
		break;
	case 'C':
		type = TJCHAR;
		break;
	case 'Z':
		type = TJBOOL;
		break;
	case 'D':
		type = TJDOUBLE;
		break;
	case 'J':
		type = TJLONG;
		break;
	case 'L':
		++i;
		name = (char *)HMALLOC(strlen(sp + i) + 1);
		strcpy(name, sp + i);
		name[(int)strlen(sp + i) - 1] = 0;
		break;
	default:
		name = (char *)HMALLOC(strlen(sp + i) + 1);
		strcpy(name, sp + i);
	}
	if (name)
	{
		TString decl_class = name;
		if (!(*MInstance::clazz).Lookup(decl_class))
		{
			MHashSet<TString> addTypes;
			if (!MInstance::InitSpec(name, &addTypes))
			{
				Clear();
				return 0;
			}
			JSpec *spec = (*MInstance::clazz)[decl_class];
			if (spec)
			{
				Copy(*spec);
				return 1;
			}
		}
		else 
		{
			JSpec *spec = (*MInstance::clazz)[decl_class];
			Copy(*spec);
			return 1;
		}
		Clear();Init
		return 0;
	}
	return 1;
}
*/
//----------------------------------------------
//
//----------------------------------------------
void JSpec::FromFunc(TString &sparams, int iStack)
{
    for (int j = 0; j < nVect - iStack; ++j)
        sparams += "[";

    switch (type)
    {
        case TJINT:
            sparams += "I";
            break;
        case TJFLOAT:
            sparams += "F";
            break;
        case TJSHORT:
            sparams += "S";
            break;
        case TJBYTE:
            sparams += "B";
            break;
        case TJCHAR:
            sparams += "C";
            break;
        case TJBOOL:
            sparams += "Z";
            break;
        case TJDOUBLE:
            sparams += "D";
            break;
        case TJLONG:
            sparams += "J";
            break;
        case TJSTRING:
        case TJOBJECT:
        {
            TString n = name;
            n.ReplaceAll(".", "/");
            sparams += "L";
            sparams += n;
            sparams += ";";
            break;
        }
        default:
            break;
    }
}

//---------------------------------
//
//---------------------------------
JSpec *JSpec::CheckSpec(jobject obj, JSpec *spec, TString &cls_name)
{
    JNIEnv *env = MInstance::GetLocalJNI();

    if (!strcmp(spec->name, "java.lang.Object"))
    {
        cls_name = MInstance::GetNameObject(env, obj);
        if (cls_name[0] == '[')
            return 0;
        if (cls_name != spec->name)
        {
            TString decl_class = cls_name;
            if (!MInstance::LookupAClass(decl_class))
            {
                MHashSet<TString> addTypes;
                if (!MInstance::InitSpec(cls_name, &addTypes))
                    return 0;
                spec = MInstance::GetAClass(decl_class);
            }
        }
    }
    return spec;
}

BOOL JSpec::Init2(TString &param)
{
    if (!param.StrLen())
        return true;
    if (param.Find("<") != -1)
        return false;
    bool fVector = param.Find("[") != -1;
    if (fVector)
    {
        while (param.Replace("[", ""))
            ++nVect;
    }
    if (param == "void")
        return true;
    if (param == "V")
        return true;
    if (param == "S")
        type = TJSHORT;
    else if (param == "I")
        type = TJINT;
    else if (param == "J")
        type = TJLONG;
    else if (param == "C")
        type = TJCHAR;
    else if (param == "B")
        type = TJBYTE;
    else if (param == "Z")
        type = TJBOOL;
    else if (param == "F")
        type = TJFLOAT;
    else if (param == "D")
        type = TJDOUBLE;
    else if (param == "short")
        type = TJSHORT;
    else if (param == "int")
        type = TJINT;
    else if (param == "long")
        type = TJLONG;
    else if (param == "char")
        type = TJCHAR;
    else if (param == "byte")
        type = TJBYTE;
    else if (param == "boolean")
        type = TJBOOL;
    else if (param == "float")
        type = TJFLOAT;
    else if (param == "double")
        type = TJDOUBLE;
    else
    {
        param.Replace(";", "");
        if (param[0] == 'L') param.Delete(0, 1);
        int l = param.StrLen();
        if (param[l - 1] == ';')
            param[l - 1] = 0;
        int n = (int) param.StrLen() + 1;
        name = (char *) HMALLOCZ(n);
        strcpy(name, param.GetPtr());
        if (param == "java.lang.String")
            type = TJSTRING;
        else
            type = TJOBJECT;

    }
    return true;
}
