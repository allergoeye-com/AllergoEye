#ifndef __JSPEC_H__
#define  __JSPEC_H__

#include "JType.h"
#pragma pack (push)
#pragma pack (4)
EXPORTNIX class JSpec : public JType {
public:
	JSpec() { name = 0; nVect = 0; type = TJEMPTY; }
	JSpec(LPCSTR _name, int _nVect, JTYPE t);
	JSpec(const JSpec &j);
	virtual ~JSpec() { if (name) HFREE(name); }
	void Init(LPCSTR _name, int _nVect, JTYPE t);
	void Copy(const JSpec &s);
	//BOOL Init(TString &t);
	void Clear() {if (name) HFREE(name); name = 0; nVect = 0; type = TJEMPTY; }
	void FromFunc(TString &, int iStack = 0);
	BOOL Init2(TString &param);
	static JSpec *CheckSpec(jobject obj, JSpec *spec, TString &cls_name);
	static void toFormGetTypeName(TString &in, TString &out);
	char *name;
	int nVect;
};
#pragma pack (pop)
#endif