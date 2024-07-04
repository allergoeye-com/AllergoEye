/*
 * mfile.cpp
 *
 *  Created on: Mar 24, 2012
 *      Author: alex
 */
#include "stdafx.h"
#include "mfile.h"

#ifdef WIN32
inline  void StdError(...) {}
BOOL _RemoveDir(LPCTSTR _dir, MPROGRESS_ROUTINE func, void *lpData)
{
	DWORD attr;
	if ((attr = GetFileAttributes(_dir)) == 0xffffffff) return 0;
     if ((attr & FILE_ATTRIBUTE_ARCHIVE) == FILE_ATTRIBUTE_ARCHIVE)
         return DeleteFile(_dir);
   _ClearDir(_dir, func, lpData);
	return RemoveDirectory(_dir) != -1;
}


//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
BOOL _ClearDir(LPCTSTR _dir, MPROGRESS_ROUTINE func, void *lpData)
{
	DWORD attr;
	if ((attr = GetFileAttributes(_dir)) == 0xffffffff) return 0;
	if ((attr & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY)  return 0;

	WIN32_FIND_DATA *dir = 0, *file = 0;
	int num_dir = 0, num_file = 0;
	MString path = _dir;
	if (path.Right(1) != SFD)
	path += SFD;
	GetDirList(path, _TEXT("*.*"), dir, num_dir, file, num_file);
	int i;
	for (i = 0; i < num_file; ++i)
    {
        if (func)
            if (func(path  + file[i].cFileName, 0, 0, lpData) == PROGRESS_CANCEL)
                return FALSE;

		DeleteFile(path  + file[i].cFileName);
	}
	for (i = 0; i < num_dir; ++i)
		if (mstrcmp(dir[i].cFileName, _TEXT(".")) && mstrcmp(dir[i].cFileName, _TEXT("..")))
			_RemoveDir(path  + dir[i].cFileName, func, lpData);

	if (file) HFREE(file);
	if (dir) HFREE(dir);
	return true;
}
bool GetDirList(LPCTSTR path, LPCTSTR mask, WIN32_FIND_DATA *&dir, int &num_dir, WIN32_FIND_DATA *&file, int &num_file)
{
	MString Path = path;
	if (Path[Path.StrLen() - 1] != FD)
		Path += SFD;
    else
       while(Path.StrLen() > 2 && Path[Path.StrLen() - 2] == FD)
             Path[Path.StrLen() -1] = 0;
	
	 MString _path = Path;
	 MString msk = mask;
	 if (!msk.StrLen())
		msk = "*.*";
	 _path +=  msk;

	WIN32_FIND_DATA FindFileData;

	memset(&FindFileData, 0, sizeof(WIN32_FIND_DATA));



	HANDLE hFile = FindFirstFile(_path.GetPtr(), &FindFileData);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		DWORD dw = GetLastError();
		SetLastError(0);
		return 0;
	}
  	MArray<WIN32_FIND_DATA> vDir, vFile;
	do
	{
		if (mstrcmp(FindFileData.cFileName, _TEXT(".")) && mstrcmp(FindFileData.cFileName, _TEXT("..")))
		{
			if ((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
			{
				WIN32_FIND_DATA &ret = vDir.Add();
				memcpy(&ret, &FindFileData, sizeof(FindFileData));
			}
			else
			{
				WIN32_FIND_DATA &ret = vFile.Add();
				memcpy(&ret, &FindFileData, sizeof(FindFileData));
			}
		}
		memset(&FindFileData, 0, sizeof(WIN32_FIND_DATA));

	}
	while (FindNextFile(hFile, &FindFileData));

	FindClose(hFile);
   	num_dir = vDir.GetLen();
	num_file = vFile.GetLen();
	file = vFile.GetPtr();
	dir = vDir.GetPtr();
	vFile.Detach();
	vDir.Detach();

	return true;

}
#endif
#ifdef ANDROID_NDK
EXPORTNIX bool UpdatePathFromWIN32(LPCSTR _dir, bool fIsDir, TString &res);
EXPORTNIX char *TmpNam(char *tmp);
extern bool CreateTmpFile(TString &assets);
CMemFile *spyfile = 0;
#endif
bool MFILE::Open(bool fCreate)
{
	Close();
#ifdef ANDROID_NDK

	if (spyfile && spyfile->GetFileName() == name) { hFile = -12345678; return true; }
#endif
	if (!name)
	{
		char _name[MAX_PATH * 2];
#ifdef __PLUK_LINUX__
		TmpNam(_name);
#else
		char path[_MAX_PATH];
		GetTempPathA(_MAX_PATH, path);
		GetTempFileNameA(path, "t", 0, _name);
#endif	 
		name = (char *)HMALLOC(strlen(_name) + 1); 
#ifdef WIN32
		strcpy_s(name, strlen(_name) + 1, _name);
#else
		strcpy(name, _name);	
#endif
	}
#ifdef ANDROID_NDK
	TString assets= name;
	if (CreateTmpFile(assets))
	{
		if (name)
			HFREE(name);
		name = assets.GetPtr();
		assets.Detach();
		fDel = true;
	}
#else
int fDel = false;
#endif
        
#ifdef __PLUK_LINUX__
        TString res;
        
        if (!fDel && name[0] == FD && UpdatePathFromWIN32(name, false, res))
            strcpy(name, res.GetPtr());
        else
        	res = name;
        if (fCreate && GetFileAttributes(res.GetPtr()) != 0xffffffff)
            unlink(res.GetPtr());
    DWORD test = GetFileAttributes(res.GetPtr());
	if (res.Find(":") != -1 || (hFile = open(res.GetPtr(), fOpenMode | (fCreate ? O_CREAT : 0) , fCreate ? S_IRWXU | S_IRWXG |  S_IRWXO : 0)) <= 0)
        {
            TRACE ("\r\nError open file %s\r\n", name);
	StdError(hFile);
        }
#else
UString n = name;
WString name_w = n;
	hFile = CreateFileW(name_w.GetPtr(),
		fOpenMode,
				fShare ? FILE_SHARE_READ | FILE_SHARE_WRITE : 0,
				0,
				fCreate ? OPEN_ALWAYS : OPEN_EXISTING,
				FILE_ATTRIBUTE_NORMAL  | FILE_FLAG_WRITE_THROUGH, NULL);
#endif
	return hFile != INVALID_HANDLE_VALUE;
}
bool MFILE::Write(off_t offset, const void *s, off_t size)
{
#ifdef ANDROID_NDK
	if (spyfile && spyfile->GetFileName() == name)
	{
		if ((int)offset != -1) spyfile->Seek(offset, CFile::begin);
		return spyfile->Write(s, (UINT)size);
	}
	if (hFile == -12345678) return 0;
#endif
	off_t pos = 0;
	if (hFile != INVALID_HANDLE_VALUE)
	{
#ifdef __PLUK_LINUX__
		off_t n;
		if (offset == -1)
		{
			do
			{
				if ((n = write(hFile, (const char *)s + pos, size - pos)) == -1)
				{
					if (errno == EINTR)
						continue;
					else
					{
						StdError(hFile);
						break;
					}
				}
				pos += n;
			}while(size > pos);
		}
		else
		{
			do
			{
				if ((n = pwrite(hFile, (const char *)s + pos, size - pos, offset + pos)) == -1)
				{
					if (errno == EINTR)
						continue;
					else
					{
						StdError(-1);
						break;
					}
				}
				pos += n;
			}while(size > pos);

		}
#else
		DWORD n = 0;
		if (offset != (DWORD)-1)
		{
			if ( SetFilePointer(hFile, (LONG)offset, 0, FILE_BEGIN ) != (DWORD)offset)
			{
				n = GetLastError();
				return false;
			}
		}
		if(!WriteFile(hFile, s, (DWORD)size, &n, 0 ) || n != (DWORD)size)
		{
			n = GetLastError();
			return false;
		}
		pos = size;
	#endif
	}
	return size == pos;
}
off_t MFILE::Read(off_t offset, void *s, off_t size)
{
#ifdef ANDROID_NDK

	if (spyfile && spyfile->GetFileName() == name)
	{
		if ((int)offset != -1) spyfile->Seek(offset, CFile::begin);
		return spyfile->Read(s, (UINT)size);
	}
	if (hFile == -12345678) return 0;
#endif

	off_t pos = 0;
	off_t b_read = 0;
	if (hFile != INVALID_HANDLE_VALUE)
	{
#ifdef __PLUK_LINUX__
		off_t n;


		if (offset == (off_t)-1)
		{
			do
			{
				if ((n = read(hFile, (char *)s + pos, size - pos)) == -1)
				{
					if (errno == EINTR)
						continue;
					else
					{
						StdError(-1);
						break;
					}
				}
				pos += n;
                b_read += n;
			}while(n && size > pos);
		}
		else
		{
			do
			{
				if ((n = pread(hFile, (char *)s + pos, size - pos, offset + pos)) == -1)
				{
					if (errno == EINTR)
						continue;
					else
					{
						StdError(-1);
						break;
					}
				}
				pos += n;
				b_read += n;
			}while(n && size > pos);

		}
	#else
		if (offset != (DWORD)-1)
			if ( SetFilePointer(hFile, (LONG)offset, 0, FILE_BEGIN ) != (DWORD)offset)
				return false;
		BOOL rw;
		LONG kn, sz = (LONG)size, dz = 0x80000;
		DWORD n;
		if (!(rw=ReadFile(hFile, (void *)s, (DWORD)size, &n, 0 )) || n != (DWORD)size)
		{

			if (rw && n) // && (dw = GetLastError()) == ERROR_HANDLE_EOF)
				return 1;
			else
			{
				int count = 0;
	_last_rrror:
				++count;
				SetLastError(0);
				SetFilePointer(hFile, (LONG)offset, 0, FILE_BEGIN );
				dz = 0x80000;
				BYTE *b = (BYTE *)s;

				while(sz > 0)
				{
			
					kn = sz < dz ? sz : dz;
					if (!(rw=ReadFile(hFile, (void *)b, kn, &n, 0 )))
					{
						;;
					}
					if (!rw || n != (DWORD)kn)
					{
						if (count > 10)
							return false;
						dz /= 2;
						goto _last_rrror;

					}
					b += n;
					sz -= n;
					b_read += n;

				}
			}
		}
        else
            b_read = size;
	#endif
	}
return b_read;

}
bool MFILE::Close()
{
#ifdef ANDROID_NDK
	if (this == (MFILE *)spyfile || (spyfile && spyfile->GetFileName() == name) || hFile == 
	-12345678)
	{
		hFile = INVALID_HANDLE_VALUE;
		return true;
	}
#endif
	if (hFile != INVALID_HANDLE_VALUE)
	#ifdef __PLUK_LINUX__
		close(hFile);
#ifdef ANDROID_NDK
	if (fDel)
		DeleteFile(name);
#endif
#else
		CloseHandle(hFile);
	#endif
	hFile = INVALID_HANDLE_VALUE;
	
	return true;
}
off_t MFILE::GetSeek()
{
#ifdef ANDROID_NDK

	if (spyfile && spyfile->GetFileName() == name)
	{
		return spyfile->GetPosition();
	}
	if (hFile == -12345678) return 0;
#endif
	off_t n = 0;
	if (hFile != INVALID_HANDLE_VALUE)
#ifdef __PLUK_LINUX__
		StdError(n = lseek(hFile, 0, SEEK_CUR));
#else
	if((n = SetFilePointer(hFile, 0, 0, FILE_CURRENT)) == INVALID_SET_FILE_POINTER)
		StdError(-1);
#endif
	return n;
}
off_t MFILE::SetSeekEnd()
{
#ifdef WIN32
	return SetFilePointer(hFile,
		0,
		0,
		FILE_END);
#else
	return lseek(hFile, 0, SEEK_END);
#endif
}
off_t MFILE::SetSeek(off_t offset)
{
#ifdef ANDROID_NDK
	if (spyfile && spyfile->GetFileName() == name)
	{
		return spyfile->Seek(offset, CFile::begin);
	}
	if (hFile == -12345678) return 0;
#endif
	off_t n = 0;
	if (hFile != INVALID_HANDLE_VALUE)
#ifdef __PLUK_LINUX__
            if (GetSize() < offset)
                SetSize(offset);
	n = lseek(hFile, offset, SEEK_SET);
#else
	if((n = SetFilePointer(hFile, (LONG)offset, 0, FILE_BEGIN)) == INVALID_SET_FILE_POINTER)
		StdError(-1);
#endif
	return n;
}
off_t MFILE::GetSize()
{
#ifdef ANDROID_NDK
	if (spyfile && spyfile->GetFileName() == name)
		{
			return spyfile->GetSize();
		}

	if (hFile == -12345678) return 0;
#endif
	off_t ret = -1;
	if (hFile != INVALID_HANDLE_VALUE)
	{
#ifdef __PLUK_LINUX__
    struct stat st;
    fstat(hFile, &st);
    return st.st_size;
#else
	ret = GetFileSize(hFile, 0);
#endif
	}
	return ret;
}
bool MFILE::SetSize(off_t size)
{
#ifdef ANDROID_NDK
	if (spyfile && spyfile->GetFileName() == name)
	{
		spyfile->SetLength(size);
		return spyfile->GetLength();
	}
	if (hFile == -12345678) return 0;
#endif
#ifdef __PLUK_LINUX__
return hFile != INVALID_HANDLE_VALUE ? !StdError(ftruncate(hFile, size)) : 0;
#else
	DWORD offset = SetFilePointer(hFile, 0, 0, FILE_CURRENT);
	DWORD Status = SetFilePointer(hFile,
							(LONG)size,
							0,
							FILE_BEGIN);
	if ((Status == (DWORD)-1) && ((Status = GetLastError()) != NO_ERROR))
	{
		Status = GetLastError();
		return 0;
	}
	Status = SetEndOfFile(hFile);
	if (!Status)
	{
		Status = GetLastError();
		return 0;
	}
	if ((off_t)offset < size)
		SetFilePointer(hFile,
						offset,
						0,
						FILE_BEGIN);
	return true;
	#endif
}
bool MFILE::Flush()
{
#ifdef ANDROID_NDK
	if (spyfile && spyfile->GetFileName() == name)
	{
		return true;
	}
	if (hFile == -12345678) return 0;
#endif
#ifdef __PLUK_LINUX__
            if (hFile != INVALID_HANDLE_VALUE)
            {
                fdatasync(hFile);
                return true;
            }
        return false;
#else
	return hFile != INVALID_HANDLE_VALUE ?
	FlushFileBuffers(hFile) == 1
	: false;
#endif
}

