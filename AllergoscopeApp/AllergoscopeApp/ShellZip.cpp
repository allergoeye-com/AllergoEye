#include "stdafx.h"
#include "ShellZip.h"
#include "mstring.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


CShellZip::CShellZip()
{
	pZippedFile = 0;
	pShell = 0;
}

CShellZip::~CShellZip()
{
	if (pZippedFile)
		pZippedFile->Release();
}

BOOL CShellZip::ListThreads(RBSet<DWORD>& set, RBSet<DWORD>* exclude)
{
	HANDLE tool;
	int N = 0;
	THREADENTRY32 entry;
	memset(&entry, 0, sizeof(THREADENTRY32));
	DWORD proc = GetCurrentProcessId();
	if ((tool = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0)) != INVALID_HANDLE_VALUE)
	{
		entry.dwSize = sizeof(THREADENTRY32);
		if (Thread32First(tool, &entry))
		{

			do
			{
				if (entry.th32OwnerProcessID == proc)
				{
					if (!exclude || !exclude->Find(entry.th32ThreadID))
						set.Add(entry.th32ThreadID);
					++N;
				}
			} while (Thread32Next(tool, &entry));

		}
		CloseHandle(tool);
	}

	return N;

}
BOOL CShellZip::Open(CString cZipFile)
{
	WString wZipFile = cZipFile.GetBuffer(0);
	BSTR lpZipFile = wZipFile.AllocSysString();
	pZippedFile = 0L;
	long FilesCount = 0;
	IDispatch* pItem = 0L;
	FolderItems* pFilesInside = 0L;
	VARIANT InZipFile;

	if (CoCreateInstance(CLSID_Shell, NULL, CLSCTX_INPROC_SERVER, IID_IShellDispatch, (void**)&pShell) == S_OK)
	{

		VariantInit(&InZipFile);
		InZipFile.vt = VT_BSTR;
		InZipFile.bstrVal = lpZipFile;
		pShell->NameSpace(InZipFile, &pZippedFile);

	}
	SysFreeString(lpZipFile);
	return pZippedFile != 0;
}
BOOL CShellZip::Create(CString strDest)
{

	RBSet<DWORD> cur;
	RBSet<DWORD> dw;
	BOOL retval = false;

	BYTE startBuffer[] = { 80, 75, 5, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	FILE* f = _wfopen(strDest.GetBuffer(strDest.GetLength()), _T("wb"));
	//FILE* f = fopen(strDest.GetBuffer(strDest.GetLength()), _T("wb"));
	fwrite(startBuffer, sizeof(startBuffer), 1, f);
	fclose(f);
	return Open(strDest);
}
BOOL CShellZip::Add(CString strSrc, CString cZipFile)
{
	BSTR bstrSource = strSrc.AllocSysString();
	HRESULT hResult = S_FALSE;
	RBSet<DWORD> cur;
	RBSet<DWORD> dw;
	BOOL retval = 0;

	if (pZippedFile)
	{
		MVector<WString> ph;
		WString wZipFile = cZipFile.GetBuffer(0);
		wZipFile.Tokenize(L"\\/", ph);
		VARIANT variantFile, variantOpt;


		Folder* pToFolder = 0;
		if (wZipFile != "/")
		{
			FolderItem* item = Find(ph);
			if (item)
			{
				item->get_GetFolder((IDispatch**)&pToFolder);
				item->Release();
			}
		}
		else
		{
			pZippedFile->AddRef();
			pToFolder = pZippedFile;
		}

		if (pToFolder)
		{
			VariantInit(&variantFile);
			variantFile.vt = VT_BSTR;
			variantFile.bstrVal = bstrSource;

			VariantInit(&variantOpt);
			variantOpt.vt = VT_I4;
			variantOpt.lVal = FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_NOCONFIRMMKDIR;

			int N = ListThreads(cur, 0);
			hResult = pToFolder->CopyHere(variantFile, variantOpt);
			retval = true;
			pToFolder->Release();
		}
		else
			return 0;
			do
	{
		dw.Clear();
		ListThreads(dw, &cur);
		if (dw.Len())
		{
			HANDLE* h = (HANDLE*)_alloca(sizeof(HANDLE) * dw.Len());
			RBSet<DWORD>::iterator it(dw);
			int i = 0;
			for (DWORD* id = it.begin(); id; id = it.next(), ++i)
				h[i] = OpenThread(THREAD_ALL_ACCESS, FALSE, *id);
			for (int j = 0; j < i; ++j)
			{
				if (h[j] != INVALID_HANDLE_VALUE)
				{
					DWORD dwRetCode;
					GetExitCodeThread(h[j], &dwRetCode);
					if (dwRetCode == STILL_ACTIVE)
					{

						WaitForSingleObject(h[j], 1000);
						CloseHandle(h[j]);
					}
				}
			}
		}


	} while (dw.Len());

	}

	return retval;

}
void CShellZip::Close()
{
	if (pZippedFile)
		pZippedFile->Release();
	pZippedFile = 0;
	if (pShell)
		pShell->Release();
	pShell = 0;
}

int CShellZip::Dir(UString sZipFile, Folder* folder, NetDir::TreeDir& dir)
{

	int ret = 0;
	FolderItems* files = 0;
	long count = 0;
	folder->Items(&files);

	if (files)
	{
		files->get_Count(&count);
		for (int i = 0; i < count; ++i)
		{
			VARIANT Item;
			IDispatch* pItem = 0L;
			VariantInit(&Item);
			Item.vt = VT_I4;
			Item.lVal = i;
			FolderItem* item = 0;
			files->Item(Item, &item);
			if (item)
			{
				VARIANT_BOOL res = 0;
				item->get_IsFolder(&res);
				BSTR str = 0;
				item->get_Path(&str);
				if (str)
				{
					WString ph;
					ph = str;
					ph.Replace(sZipFile, "");
					ph.ReplaceAll(L"\\", L"/");
					SysFreeString(str);
					++ret;
					if (res == VARIANT_TRUE)
					{
						Folder* pFolder = 0;
						dir.AddDir(ph);
						item->get_GetFolder((IDispatch**)&pFolder);
						if (pFolder)
						{
							ret += Dir(sZipFile, pFolder, dir);
							pFolder->Release();
						}
					}
					else
						dir.AddFile(ph);

				}
				item->Release();
			}
		}
		files->Release();

	}
	return ret;

}
BOOL CShellZip::Dir(NetDir::TreeDir& dir)
{
	int ret = 0;
	FolderItems* pFilesInside = 0L;
	long FilesCount = 0;
	if (pZippedFile)
	{
		ret = 1;
		UString sZipFile;
		pZippedFile->Items(&pFilesInside);
		if (pFilesInside)
		{
			pFilesInside->get_Count(&FilesCount);
			if (FilesCount)
			{
				VARIANT Item;
				IDispatch* pItem = 0L;
				VariantInit(&Item);
				Item.vt = VT_I4;
				Item.lVal = 0;
				FolderItem* item = 0;

				pFilesInside->Item(Item, &item);
				BSTR str = 0;
				item->get_Path(&str);

				sZipFile = str;
				int i = sZipFile.ReverseFind('\\');
				sZipFile[i] = 0;
				item->Release();
			}
			pFilesInside->Release();
			if (FilesCount)
				Dir(sZipFile, pZippedFile, dir);
		}

	}
	return ret;

}
FolderItem* CShellZip::Find(MVector<WString>& ph)
{
	FolderItems* pFilesInside = 0L;
	long FilesCount = 0;
	if (pZippedFile)
	{
		UString sZipFile;
		pZippedFile->Items(&pFilesInside);
		if (pFilesInside)
		{
			pFilesInside->get_Count(&FilesCount);
			pFilesInside->Release();
			if (FilesCount)
				return Find(pZippedFile, ph, 0);
		}

	}
	return 0;

}



FolderItem* CShellZip::Find(Folder* folder, MVector<WString>& ph, int iItem)
{

	int ret = 0;
	FolderItem* item = 0;
	FolderItems* files = 0;
	long count = 0;
	ph[iItem];
	BSTR lpFolder = ph[iItem].AllocSysString();
	
	folder->ParseName(lpFolder, &item);
	SysFreeString(lpFolder);
	if (item)
	{
		VARIANT_BOOL res = 0;
		item->get_IsFolder(&res);
		if (ph.GetLen() > iItem + 1)
		{
			if (res == VARIANT_TRUE)
			{
				Folder* pFolder = 0;
				item->get_GetFolder((IDispatch**)&pFolder);
				item->Release();
				item = 0;

				if (pFolder)
				{
					item = Find(pFolder, ph, iItem + 1);
					pFolder->Release();
				}
			}
			else
			{
				item->Release();
				item = 0;

			}
		}
	}
	return item;
	/*
	folder->Items(&files);

	if (files)
	{
		files->get_Count(&count);
		for (int i = 0; i < count; ++i)
		{
			VARIANT Item;
			IDispatch* pItem = 0L;
			VariantInit(&Item);
			Item.vt = VT_I4;
			Item.lVal = i;
			files->Item(Item, &item);
			if (item)
			{
				VARIANT_BOOL res = 0;
				item->get_IsFolder(&res);
				BSTR str = 0;
				item->get_Name(&str);
				if (str)
				{

					BOOL fFind = ph[iItem] == str;
					SysFreeString(str);


					if (fFind)
					{
						if (ph.GetLen() > iItem + 1)
						{
							if (res == VARIANT_TRUE)
							{
								Folder* pFolder = 0;
								item->get_GetFolder((IDispatch**)&pFolder);
								item->Release();
								item = 0;

								if (pFolder)
								{
									item = Find(pFolder, ph, iItem + 1);
									pFolder->Release();
								}
							}
							else
							{
								item->Release();
								item = 0;

							}
						}
						break;;
					}

				}
				item->Release();
				item = 0;
			}
		}
		files->Release();

	}
	return item;
	*/

}
BOOL CShellZip::Dir(CString wZipFile, NetDir::TreeDir& dir)
{
	int ret = 0;
	Open(wZipFile);
	if (pZippedFile)
		ret = Dir(dir);
	Close();
	return ret;

}
BOOL CShellZip::Extract(CString cZipFile, CString wFolder)
{
	if (pZippedFile)
	{
		VARIANT Item, Options, OutFolder;
		Folder* pDestination = 0L;
		BOOL fRet = 0;
		BSTR lpFolder = wFolder.AllocSysString();
		VariantInit(&OutFolder);
		OutFolder.vt = VT_BSTR;
		OutFolder.bstrVal = lpFolder;
		pShell->NameSpace(OutFolder, &pDestination);
		SysFreeString(lpFolder);
		if (pDestination)
		{
			MVector<WString> ph;
			WString wZipFile = cZipFile.GetBuffer(0);
			wZipFile.Tokenize(L"\\/", ph);
			FolderItem* item = Find(ph);
			if (item)
			{
				IDispatch* pItem = 0;
				item->QueryInterface(IID_IDispatch, (void**)&pItem);
				if (pItem)
				{
					VariantInit(&Options);
					Item.vt = VT_DISPATCH;
					Item.pdispVal = pItem;
					Options.vt = VT_I4;
					Options.lVal = FOF_SILENT | FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_NOCONFIRMMKDIR;
					fRet = pDestination->CopyHere(Item, Options) == S_OK;
					pItem->Release();
					fRet = true;
				}
				item->Release();
			}
			pDestination->Release();
		}
		return fRet;


	}
	return 0;

}