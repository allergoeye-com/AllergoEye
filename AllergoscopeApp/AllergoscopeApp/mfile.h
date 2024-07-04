/*
 * mfile.h
 *
 *  Created on: Mar 24, 2012
 *      Author: alex
 */

#ifndef __MFILE_H_
#define __MFILE_H_
#include "mio.h"
#ifndef FILE_BEGIN
#define FILE_BEGIN           0
#define FILE_CURRENT         1
#define FILE_END             2
#endif
#ifdef WIN32
#define FD		'\\'
#define SFD		"\\"
#define CD		".\\"
#define FILEHANDLE HANDLE 
#else
#define FD		'/'
#define SFD		"/"
#define CD		"./"
#endif
#ifndef _EOS_LINUX_
#ifdef NOPLUK
#ifndef CR
#define CR "/r/n"
#endif
#endif
#endif
#pragma pack(push) //NADA
#pragma pack(4)
EXPORTNIX class _PEXPORT MFILE:public MIO {

public:
#ifdef __PLUK_LINUX__
	enum MFILE_FLG {F_READONLY=O_RDONLY, F_WRITEONLY=O_WRONLY, F_READWRITE=O_RDWR, F_OPEN=0, F_CREATE=O_CREAT};
#else
	enum MFILE_FLG {F_READONLY=GENERIC_READ, F_WRITEONLY=GENERIC_WRITE, F_READWRITE=GENERIC_WRITE | GENERIC_READ, F_OPEN=OPEN_EXISTING, F_CREATE=OPEN_ALWAYS, O_PERM = 0 };
#endif
	MFILE(bool _fShare = false, MIO::MIO_Type fType = T_FILE):MIO(fType) {
#ifdef ANDROID_NDK
	fDel = false;
#endif
		fOpenMode = F_READWRITE; fShare = _fShare; hFile = INVALID_HANDLE_VALUE; }
	MFILE(const char *_name,bool _fShare = false, MIO_Type fType = T_FILE):MIO(fType) {
#ifdef ANDROID_NDK
	fDel = false;
#endif
		fOpenMode = F_READWRITE; fShare = _fShare; hFile = INVALID_HANDLE_VALUE;
		if (_name) 
		{
			name = (char *)HMALLOC(strlen(_name) + 1); 
#ifndef WIN32
			strcpy(name, _name); 
#else
			strcpy_s(name, strlen(_name) + 1, _name);
#endif
		} 
		else 
			name = 0; 
	}

	virtual ~MFILE() { Close(); }
        void SetOpenMode(int mode) { fOpenMode = mode; }
	virtual bool Open(bool fCreate = false);
	virtual bool Write(off_t offset, const void *s, off_t size);
	virtual off_t Read(off_t offset, void *s, off_t size);
        virtual off_t Read(void *s, off_t size)    { return Read(-1, s, size); }
        virtual bool Write(const void *s, off_t size)    { return Write(-1, s, size); }
	virtual bool Close(); 
	virtual off_t GetSeek();
	virtual off_t SetSeek(off_t offset);
	virtual off_t SetSeekEnd();
	virtual off_t GetSize();
	virtual bool SetSize(off_t size);
	virtual bool Flush();
	FILEHANDLE File() { return hFile; }
	virtual BYTE *Detach() { hFile = INVALID_HANDLE_VALUE; return 0; }
protected:
	FILEHANDLE hFile;
	int fShare;
        int fOpenMode;
#ifdef ANDROID_NDK
	bool fDel;
#endif


};
#pragma pack(pop) //NADA
#ifdef WIN32
bool GetDirList(LPCTSTR path, LPCTSTR mask, WIN32_FIND_DATA *&dir, int &num_dir, WIN32_FIND_DATA *&file, int &num_file);
typedef DWORD (WINAPI *MPROGRESS_ROUTINE)(LPCTSTR filename, UINT_PTR size, UINT_PTR pos,  LPVOID lpUserData);
BOOL _ClearDir(LPCTSTR _dir, MPROGRESS_ROUTINE func = 0, void *lpData = 0);

#endif
#endif /* MFILE_H_ */
