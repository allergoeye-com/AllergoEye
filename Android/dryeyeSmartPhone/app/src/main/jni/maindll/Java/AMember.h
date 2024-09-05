/*
 * AMember.h
 *
 *  Created on: Jun 16, 2017
 *      Author: alex
 */

#ifndef MAINDLL_AMEMBER_H_
#define MAINDLL_AMEMBER_H_

#include "JType.h"
#include "JSpec.h"
#pragma pack (push)
#pragma pack (4)

EXPORTNIX struct AMember {
	AMember (const char *name, const char *_ret, TString *param, int _nParam, int &fError);
	AMember (const AMember &m);
	~AMember();
	void GetFuncSpec(TString &param);
	BOOL InitSpec(TString &param, JSpec &p);

	TString name;
	JSpec ret;
	JSpec *params;
	union {
	jmethodID id;
	jfieldID  fid;
	};
	union {
			struct {
			UINT nParam : 28;
			UINT fStatic : 1;
			UINT fField : 1;
			UINT fFieldIn : 1;
			UINT fConstructor : 1;
	};
		UINT data;
	};


};
#pragma pack (pop)

#endif /* MAINDLL_AMEMBER_H_ */
