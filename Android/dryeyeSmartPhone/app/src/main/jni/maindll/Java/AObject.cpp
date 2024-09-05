/*
 * AObject.cpp
 *
 *  Created on: Jun 22, 2017
 *      Author: alex
 */

#include "stdafx.h"
#include "AObject.h"
#include "JVariable.h"
#include "AClass.h"
#include "minstance.h"

//-----------------------------------
//
//----------------------------------
AObject::AObject()
{
	spec = 0;
	obj = 0;
	fLocal = TJLOCAL;

}
//-----------------------------------
//
//----------------------------------
AObject::~AObject()
{
	Clear();
}
//-----------------------------------
//
//----------------------------------
void AObject::Clear()
{
	if (obj)
	{
		JNIEnv *env = MInstance::GetLocalJNI();
		switch(fLocal)
		{
		case TJLOCAL:
		case TJDEFAULT:
			env->DeleteLocalRef(obj);
			break;
		case TJGLOBAL:
			env->DeleteGlobalRef(obj);
			break;
		default:
			break;
		}


	}
	spec = 0;
	obj = 0;
	fLocal = TJLOCAL;

}
//-----------------------------------
//
//----------------------------------
jobject AObject::Detach()
{
	jobject ret = obj;
	spec = 0;
	obj = 0;
	fLocal = TJDEFAULT;
	return ret;

}
BOOL AObject::Attach(jobject o, LPCSTR stype, AObject::_TYPE_ _fLocal, JNIEnv *_env)
{
	Clear();
	if (!o) return false;
	JNIEnv *env = _env ? _env : MInstance::GetLocalJNI();
	TString s = stype ? stype : MInstance::GetNameObject(env,o).GetPtr();
	if (MInstance::InitSpec(s.GetPtr()))
	{
		fLocal = _fLocal;
		switch(fLocal)
		{
			case TJLOCAL:
				obj = env->NewLocalRef(o);
				break;
			case TJGLOBAL:
				obj = env->NewGlobalRef(o);
				break;
			case TJSTATIC:
			default:
				obj = o;
		}
		spec = MInstance::GetAClass(s.GetPtr());
	}
	return spec != 0;

}

//-----------------------------------
//
//----------------------------------
BOOL AObject::Attach(jobject o, AObject::_TYPE_ _fLocal, JNIEnv *_env)
{
	Clear();
	if (!o) return false;
	JNIEnv *env = _env ? _env : MInstance::GetLocalJNI();
    TString s = MInstance::GetNameObject(env,o);
	if (MInstance::InitSpec(s.GetPtr()))
	{
		fLocal = _fLocal;
		switch(fLocal)
		{
		case TJLOCAL:
			obj = env->NewLocalRef(o);
			break;
		case TJGLOBAL:
			obj = env->NewGlobalRef(o);
			break;
		case TJSTATIC:
		default:
			obj = o;
		}
		spec = MInstance::GetAClass(s.GetPtr());
	}
	return spec != 0;

}
//-----------------------------------
//
//----------------------------------
BOOL AObject::Attach(const char *name)
{
	Clear();
	if (MInstance::InitSpec(name))
	{
		obj = MInstance::FindClass(name);
		fLocal = TJSTATIC;
		spec = MInstance::GetAClass(name);
	}
	return obj != 0 && spec != 0;


}
//-----------------------------------
//
//----------------------------------
BOOL AObject::GetMembers(TStruct &s, JNIEnv *_env)
{
	if (!spec || !obj) return 0;
    JNIEnv *env = _env ? _env : MInstance::GetLocalJNI();
    ArHashMap<AMember *, TString> &members = spec->GetMembers();
    ArHashMap<AMember *, TString>::iterator it(members);
    for (RBData<AMember *, TString> *t = it.begin(); t; t = it.next())
    {
        if (t->value->fField)
        {
            JVariable res;
            if (!t->value->fFieldIn && InvokeField(t->value, 0, 0, res, env))
			{
                res.AddTo(t->value->name.GetPtr(), s);
			}
        }
    }
    return true;

}

BOOL AObject::Run(const char *name, JNIEnv *env)
{
	JVariable res;
	return Run(name, 0, 0, res, env);
}
BOOL AObject::Run(const char *name, JVariable *v, int n, JNIEnv *env)
{
	JVariable res;
	return Run(name, v, n, res, env);

}
BOOL AObject::Run(const char *name, JVariable &res, JNIEnv *env)
{
	return Run(name, 0, 0, res, env);
}

//-----------------------------------
//
//----------------------------------
BOOL AObject::Run(const char *name, JVariable *v, int n, JVariable &res, JNIEnv *_env)
{
	if (!spec || !obj) return 0;
	try
	{
		JNIEnv *env = _env ? _env : MInstance::GetLocalJNI();

		TString param;
		for (int i = 0; i < n; ++i)
			v[i].FromFunc(param);
		AMember *member = spec->GetMember(name, param.GetPtr(), env);
		if (!member)
			return 0;
		if (member->fField)
			return InvokeField(member, v, n, res, env);

		return Invoke(member, v, n, res, env);
	}
	catch(...)
	{
		return false;
	}
}
//-----------------------------------
//
//----------------------------------
BOOL AObject::Invoke(AMember *member, JVariable *jv, int n, JVariable &ret, JNIEnv *env)
{
	if (!spec || !obj) return 0;
	if (member->fField)
		return InvokeField(member, jv, n, ret, env);
	jvalue *args = 0;
	if (n)
		args = new jvalue[n];
	for (int i = 0; i < n; ++i)
		args[i] = jv[i].value;
	if (member->fStatic)
	{
		JPtr clazz;
		if (fLocal == TJSTATIC)
			clazz.obj = obj;
		else
			clazz = MInstance::FindClass(spec->name);
		if (MInstance::ClearException(env)) goto __err_exit;
		if (!member->ret.nVect)
		{
			switch(member->ret.type)
			{
                case TJLONG:
                    ret = env->CallStaticLongMethodA(clazz, member->id, args);
                    break;
                case TJINT:
					ret = env->CallStaticIntMethodA(clazz, member->id, args);
				break;
				case TJDOUBLE:
					ret = env->CallStaticDoubleMethodA(clazz, member->id, args);
				break;
				case TJFLOAT:
					ret = env->CallStaticFloatMethodA(clazz, member->id, args);
				break;
				case TJBOOL:
					ret = env->CallStaticBooleanMethodA(clazz, member->id, args);
				break;
				case TJBYTE:
					ret = env->CallStaticByteMethodA(clazz, member->id, args);
				break;
				case TJCHAR:
					ret = env->CallStaticCharMethodA(clazz, member->id, args);
				break;
				case TJSTRING:
				case TJOBJECT:
				{
					jobject _obj = env->CallStaticObjectMethodA(clazz, member->id, args);
					if (MInstance::ClearException(env))  goto __err_exit;
					ret.Attach(_obj, &member->ret);


				}
				break;
				default:
					env->CallStaticVoidMethodA(clazz, member->id, args);
			}
		}
		else
		{
				jobject _obj = env->CallStaticObjectMethodA(clazz, member->id, args);
				if (MInstance::ClearException(env))  goto __err_exit;
				ret.Attach(_obj, &member->ret);

		}
		if (fLocal == TJSTATIC)
			clazz.Detach();

	}
	else
	if (!member->ret.nVect)
	{
		if (!obj) goto __err_exit;
		switch(member->ret.type)
		{
            case TJLONG:
                ret = env->CallLongMethodA(obj, member->id, args);
                break;

            case TJINT:
				ret = env->CallIntMethodA(obj, member->id, args);
			break;
			case TJDOUBLE:
				ret = env->CallDoubleMethodA(obj, member->id, args);
			break;
			case TJFLOAT:
				ret = env->CallFloatMethodA(obj, member->id, args);
			break;
			case TJBOOL:
				ret = env->CallBooleanMethodA(obj, member->id, args);
			break;
			case TJBYTE:
				ret = env->CallByteMethodA(obj, member->id, args);
			break;
			case TJCHAR:
				ret = env->CallCharMethodA(obj, member->id, args);
			break;
			case TJSTRING:
			case TJOBJECT:
			{
				jobject _obj = env->CallObjectMethodA(obj, member->id, args);
				if (MInstance::ClearException(env))  goto __err_exit;
				ret.Attach(_obj, &member->ret);
			}
			break;
			default:
				env->CallVoidMethodA(obj, member->id, args);
			if (MInstance::ClearException(env)) goto __err_exit;
		}
	}
	else
	{
			jobject _obj = env->CallObjectMethodA(obj, member->id, args);
			if (MInstance::ClearException(env))  goto __err_exit;
			ret.Attach(_obj, &member->ret);

	}
	if (args) delete [] args;
	return 1;
__err_exit:
	if (args) delete [] args;
	return 0;


}
//-----------------------------------
//
//----------------------------------
BOOL AObject::New(const char *p, JVariable *v, int n, _TYPE_ _type, JNIEnv *_env)
{
	JNIEnv *env = _env ? _env : MInstance::GetLocalJNI();
	if (MInstance::InitSpec(p))
	{
		JPtr load = MInstance::FindClass(p);
		if (!load.obj) return 0;
		spec = MInstance::GetAClass(p);
		fLocal = _type;
		TString param;
		jvalue *args = 0;
		jobject tst;
		if (n)
		{
			for (int i = 0; i < n; ++i)
				v[i].FromFunc(param);
			args = new jvalue[n];
			for (int i = 0; i < n; ++i)
				args[i] = v[i].value;
		}
		AMember *member = spec->GetMember("<init>", param.GetPtr());
		if (_type != TJSTATIC && member)
		{
			obj = env->NewObjectA(load.cls, member->id, args);
			if (MInstance::ClearException(env, 0)) return false;

		} else
		{
            obj = load.obj;
            load.Detach();
		}
        if (obj)
        if (fLocal == TJGLOBAL)
        {
            jobject o = obj;
            obj = env->NewGlobalRef(o);
            env->DeleteLocalRef(o);

        }
		if (args) delete [] args;
	}
	return  obj != 0;
}
//-----------------------------------
//
//----------------------------------
BOOL AObject::InvokeField(AMember *member, JVariable *v, int n, JVariable &ret, JNIEnv *env)
{
	if (!spec || !obj) return 0;
	if (!obj) return 0;
	if (!n)
	{

		if (!member->ret.nVect)
		{
			if (member->fStatic)
			{
				JPtr clazz = MInstance::FindClass(spec->name);
				if (MInstance::ClearException(env)) goto __err_exit;
				switch(member->ret.type)
				{
                    case TJLONG:
                        ret = env->GetStaticLongField(clazz, member->fid);
                        break;

                    case TJINT:
						ret = env->GetStaticIntField(clazz, member->fid);
					break;
					case TJDOUBLE:
						ret = env->GetStaticDoubleField(clazz, member->fid);
					break;
					case TJFLOAT:
						ret = env->GetStaticFloatField(clazz, member->fid);
					break;
					case TJBOOL:
						ret = env->GetStaticBooleanField(clazz, member->fid);
					break;
					case TJBYTE:
						ret = env->GetStaticByteField(clazz, member->fid);
					break;
					case TJCHAR:
						ret = env->GetStaticCharField(clazz, member->fid);
					break;
					case TJSTRING:
					case TJOBJECT:
					{
						jobject _obj = env->GetStaticObjectField(clazz, member->fid);
						if (MInstance::ClearException(env))  goto __err_exit;
						ret.Attach(_obj, &member->ret);
					}
					break;
					default:
						break;
				}
			}
			else
			switch(member->ret.type)
			{
                case TJLONG:
                    ret = env->GetLongField(obj, member->fid);
                    break;
                case TJINT:
					ret = env->GetIntField(obj, member->fid);
				break;
				case TJDOUBLE:
					ret = env->GetDoubleField(obj, member->fid);
				break;
				case TJFLOAT:
					ret = env->GetFloatField(obj, member->fid);
				break;

				case TJBOOL:
					ret = env->GetBooleanField(obj, member->fid);
				break;
				case TJBYTE:
					ret = env->GetByteField(obj, member->fid);
				break;
				case TJCHAR:
					ret = env->GetCharField(obj, member->fid);
				break;
				case TJSTRING:
				case TJOBJECT:
				{
					jobject _obj = env->GetObjectField(obj, member->fid);
					if (MInstance::ClearException(env))  goto __err_exit;
					ret.Attach(_obj, &member->ret);
				}
				break;
				default:
					break;
			}
		}
		else
		{
			ret.obj = env->GetObjectField(obj, member->fid);
			if (MInstance::ClearException(env))  goto __err_exit;
			ret.Attach(ret.obj, &member->ret);

		}

	}
	else
	{
		if (!member->ret.nVect)
		{
			switch(member->ret.type)
			{
                case TJLONG:
                    env->SetLongField(obj, member->fid, (int)v[0]);
                    break;
				case TJINT:
					env->SetIntField(obj, member->fid, (int)v[0]);
				break;
				case TJDOUBLE:
					env->SetDoubleField(obj, member->fid, (double)v[0]);
				break;
				case TJFLOAT:
					env->SetFloatField(obj, member->fid, (float)(double)v[0]);
				break;
				case TJBOOL:
					env->SetBooleanField(obj, member->fid, (int)v[0] != 0);
				break;
				case TJBYTE:
					env->SetByteField(obj, member->fid, (char)v[0]);
				break;
				case TJCHAR:
					env->SetCharField(obj, member->fid, (jchar)(int)v[0]);
				break;
				case TJSTRING:
				{
					env->SetObjectField(obj, member->fid, v[0].str);
				}
				break;
				case TJOBJECT:
				{

					env->SetObjectField(obj, member->fid, v[0].obj);
				}
				break;
				default:
					break;
			}
		}
		else
		{
			env->SetObjectField(obj, member->fid, v[0].obj);
		}

	}

	if (MInstance::ClearException(env)) goto __err_exit;
	return 1;
__err_exit:
	return 0;

}


