#include "stdafx.h"
#include "AMember.h"


//----------------------------------------------
//
//----------------------------------------------
AMember::AMember (const char *_name, const char *_ret, TString *param, int _nParam, int &fError)
{
	fConstructor = 0;
	fField = 0;
	fStatic = 0;
	fError = true;
	TString p;
	fid = 0;
	data = 0;
	params = 0;
	if (_ret)
	{
		p = _ret;
		if (!InitSpec(p, ret))
			return;
	}
	if ((nParam = (UINT)_nParam))
	{
		params = new JSpec[nParam];
		for (int i = 0; i < nParam; ++i)
			if (!InitSpec(param[i], params[i]))
				return;
	}
	fError = false;
	name = _name;
}
AMember::AMember (const AMember &par)
{
	fid = 0;
	fConstructor = 0;
	fField = 0;
	fStatic = 0;
	data = 0;
	params = 0;
	if (par.ret.type != TJEMPTY)
		ret.Copy(par.ret);
	if ((nParam = par.nParam))
	{
		params = new JSpec[nParam];
		for (int i = 0; i < nParam; ++i)
			params[i].Copy(par.params[i]);
	}
	name = par.name;
	data = par.data;
	fid = par.fid;

}

//----------------------------------------------
//
//----------------------------------------------
void AMember::GetFuncSpec(TString &sparams)
{
	if (!fField)
	{
		sparams = "(";
		for (int i = 0; i < nParam; ++i)
			params[i].FromFunc(sparams);
		sparams += ")";
		if (ret.type == TJEMPTY)
		{
			sparams += "V";
		}
		else
			ret.FromFunc(sparams);
	}
	else
		ret.FromFunc(sparams);

}

//----------------------------------------------
//
//----------------------------------------------
AMember::~AMember()
{
	if (params)
		delete [] params;
}
BOOL AMember::InitSpec(TString &param, JSpec &p)
{
	return p.Init2(param);

}
