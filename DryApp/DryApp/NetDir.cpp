#include "stdafx.h"
#include "NetDir.h"
int OnError();
int CompareFileTime(SYSTEMTIME& tm1, SYSTEMTIME& tm2)
{
	if (tm1.wYear > tm2.wYear) return 1;
	else if (tm1.wYear < tm2.wYear) return -1;
	if (tm1.wMonth > tm2.wMonth) return 1;
	else if (tm1.wMonth < tm2.wMonth) return -1;
	if (tm1.wDay > tm2.wDay) return 1;
	else if (tm1.wDay < tm2.wDay) return -1;
	if (tm1.wHour > tm2.wHour) return 1;
	else if (tm1.wHour < tm2.wHour) return -1;
	if (tm1.wMinute > tm2.wMinute) return 1;
	else if (tm1.wMinute < tm1.wMinute) return -1;

	if (tm1.wSecond > tm2.wSecond) return 1;
	else if (tm1.wSecond < tm2.wSecond) return -1;
	return 0;
}

namespace NetDir
{

	//--------------------------------------------------------------------
//
//--------------------------------------------------------------------
	int __cdecl SortFunc(const void* _a, const void* _b)
	{
		FileNode **a = (FileNode **)_a;
		FileNode** b = (FileNode **)_b;
		return (*a)->name.Compare((*b)->name);
	}

	//--------------------------------------------------------------------------------
	//
	//--------------------------------------------------------------------------------
	TreeDir::TreeDir(Tree* data)
	{
		root = 0;
		Node* node = data->root->FindFolder(L"file-area");
		if (node)
			root = new  DirNode(0, node);
		else
			root = new  DirNode(0, 0);
		root->Sort();

	}
	//--------------------------------------------------------------------------------
	//
	//--------------------------------------------------------------------------------
	TreeDir::TreeDir(const WString& data)
	{
		root = new  DirNode(0, "", data);
		root->Sort();
	}
	//--------------------------------------------------------------------------------
	//
	//--------------------------------------------------------------------------------
	void TreeDir::MakeTree(Tree* data)
	{
		if (!root)return;
		data->CreateNode(L"file-area");

		root->MakeTree(data);
		data->CloseNode(L"file-area");
	}
	//--------------------------------------------------------------------------------
	//
	//--------------------------------------------------------------------------------
	void TreeDir::MakeXML(WString& data)
	{
		data += "<file-area>";
		if (root)
			root->MakeXML(data, 1);
		data += "</file-area>";
	}
	//--------------------------------------------------------------------------------
	//
	//--------------------------------------------------------------------------------
	void TreeDir::MkDirs(const WString& _ph)
	{
		if (root)
		{
			root->MkDirs(_ph);
			root->Sort();
		}


	}

	//--------------------------------------------------------------------------------
	//
	//--------------------------------------------------------------------------------
	FileNode* TreeDir::MakeFileNode(const WString& name)
	{
		return new FileNode(name);
	}
	//--------------------------------------------------------------------------------
	//
	//--------------------------------------------------------------------------------
	int TreeDir::GetFileList(MArray<FileNode*>& list)
	{
		return root == 0 ? 0 : root->GetFileListAll(list);

	}
	int TreeDir::GetFileList(MVector<WString>& mask, MArray<FileNode*>& list)
	{
		return root == 0 ? 0 : root->GetFileListAll(mask, list);

	}
	void TreeDir::DeleteNode(FileNode* node)
	{
		DirNode* dir = dynamic_cast<DirNode*>(node->parent);
		if (dir)
		{
			dir->DeleteNode(node);
			if (!dir->childs.GetLen())
				DeleteNode(dir);
		}
	}
	//--------------------------------------------------------------------------------
	//
	//--------------------------------------------------------------------------------
	int TreeDir::GetFileList(const WString& _dir, MArray<DirNode*>& dirs, MArray<FileNode*>& files)
	{
		if (root)
		{
			WString _st = _dir;
			MVector< WString> st;
			_st.Tokenize(L"/\\", st);
			DirNode* dir;
			if (st.GetLen())
			{
				FileNode* node = root->GetNode(st);
				if (node)
				{
					if (dir = node->IsDir())
						dir->GetFileList(dirs, files);
					else
						files.Add(node);
				}
			}
			else
				root->GetFileList(dirs, files);

		}
		return files.GetLen() + dirs.GetLen();

	}
	//-----------------------------------
	//
	//-----------------------------------
	void TreeDir::UpdateAddFile(const MVector<WString>& mask, const WString& folder)
	{
		if (root)
		{
			root->UpdateAddFile(mask, folder);
			
		}

	}
	//--------------------------------------------------------------------------------
	//
	//--------------------------------------------------------------------------------
	int TreeDir::GetFileList(const WString& _dir, MVector<WString>& mask, MArray<DirNode*>& dirs, MArray<FileNode*>& files)
	{
		if (root)
		{
			WString _st = _dir;
			MVector< WString> st;
			_st.Tokenize(L"/\\", st);
			DirNode* dir;
			if (st.GetLen())
			{

				FileNode* node = root->GetNode(st);
				if (node)
				{
					if (dir = node->IsDir())
						dir->GetFileList(mask, dirs, files);
				}
			}
			else
				root->GetFileList(mask, dirs, files);

		}
		return files.GetLen() + dirs.GetLen();

	}

	//--------------------------------------------------------------------------------
	//
	//--------------------------------------------------------------------------------
	FileNode* TreeDir::GetFile(const WString& filepath)
	{
		FileNode* node = 0;
		if (node = root)
		{
			WString _st = filepath;
			MVector< WString> st;
			_st.Tokenize(L"/\\", st);
			if (st.GetLen())
				node = root->GetNode(st);
		}
		return node;
	}
	//-----------------------------------
	//
	//-----------------------------------
	DirNode::DirNode(FileNode* _parent, Node* folder) :FileNode(_parent, folder)
	{

		if (folder)
		{
			Node* files = folder->Get(L"folders");
			if (files)
				for (int i = 0, l = files->child.GetLen(); i < l; ++i)
				{
					Node* file = files->child[i];
					childs.Add(new DirNode(this, file));
				}

			files = folder->Get(L"files");
			if (files)
				for (int i = 0, l = files->child.GetLen(); i < l; ++i)
				{
					Node* file = files->child[i];
					childs.Add(new FileNode(this, file));
				}
		}
	}
	bool GetDirList(const WString& folder, LPCTSTR mask, RBSet<WString> &dir, RBSet<WString> &file)
	{
		WString Path = folder;
		if (Path[Path.StrLen() - 1] != L'\\')
			Path += L"\\";
		else
			while (Path.StrLen() > 2 && Path[Path.StrLen() - 2] == L'\\')
				Path[Path.StrLen() - 1] = 0;

		WString _path = Path;
		WString msk = mask;
		if (!msk.StrLen())
			msk = L"*.*";
		_path += msk;

		WIN32_FIND_DATAW FindFileData;

		memset(&FindFileData, 0, sizeof(WIN32_FIND_DATAW));



		HANDLE hFile = FindFirstFile(_path.GetPtr(), &FindFileData);
		WString s;
		if (hFile == INVALID_HANDLE_VALUE)
		{
			DWORD dw = GetLastError();
			SetLastError(0);
			return 0;
		}
		
		do
		{
			if (mstrcmp(FindFileData.cFileName, _TEXT(".")) && mstrcmp(FindFileData.cFileName, _TEXT("..")))
			{
				s = FindFileData.cFileName;
				if ((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
					dir.Add(s);
				else
					file.Add(s);
			}
			memset(&FindFileData, 0, sizeof(WIN32_FIND_DATA));

		} while (FindNextFile(hFile, &FindFileData));

		FindClose(hFile);
		return true;

	}
	//-----------------------------------
	//
	//-----------------------------------
	DirNode::DirNode(FileNode* _parent, const WString& name, const WString& folder) :FileNode(_parent, name)
	{
		_parent = parent;
		WString ppath = folder;
		WString  path;
		path = ppath.GetPtr();
		int ret = 0;
		int len = path.StrLen();
		if (path.Right(1) != L"\\")
			path += L"\\";
		ppath = path;
		path += L"*.*";

		WIN32_FIND_DATAW FindFileData;
		memset(&FindFileData, 0, sizeof(WIN32_FIND_DATAW));

		HANDLE hFile = FindFirstFileW(path, &FindFileData);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			DWORD dw = GetLastError();
			if (GetLastError() != ERROR_FILE_NOT_FOUND)
				return;
			return;
		}
		do
		{
			if (!wcscmp(FindFileData.cFileName, L"."))
				continue;
			if (!wcscmp(FindFileData.cFileName, L".."))
				continue;
			WString res;
			res = ppath;
			WString name;
			name = FindFileData.cFileName;
			res += FindFileData.cFileName;

			if ((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
				childs.BInsert(new DirNode(this, name, res), SortFunc);
			else
				childs.BInsert(new FileNode(this, FindFileData), SortFunc);

			memset(&FindFileData, 0, sizeof(WIN32_FIND_DATAW));
		} while (FindNextFileW(hFile, &FindFileData));
		FindClose(hFile);

	}
	//---------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	int DirNode::CheckDir(const WString& folder, LPCTSTR mask, MArray<FileNode *>& n)
	{
		RBSet<WString> dir, file;
		WString Path = folder;
		if (Path[Path.StrLen() - 1] != L'\\')
			Path += L"\\";
		else
			while (Path.StrLen() > 2 && Path[Path.StrLen() - 2] == L'\\')
				Path[Path.StrLen() - 1] = 0;

		WString _path = Path;
		WString msk = mask;
		if (!msk.StrLen())
			msk = L"*.*";
		_path += msk;

		WIN32_FIND_DATAW FindFileData;

		memset(&FindFileData, 0, sizeof(WIN32_FIND_DATAW));



		HANDLE hFile = FindFirstFile(_path.GetPtr(), &FindFileData);
		WString s;
		if (hFile == INVALID_HANDLE_VALUE)
		{
			DWORD dw = GetLastError();
			SetLastError(0);
			return 0;
		}

		do
		{
			if (mstrcmp(FindFileData.cFileName, _TEXT(".")) && mstrcmp(FindFileData.cFileName, _TEXT("..")))
			{
				s = FindFileData.cFileName;
				FileNode *node = GetChild(s);
				if (!node)
				{
					WString res;
					res = folder;
					WString name;
					name = FindFileData.cFileName;
					res += FindFileData.cFileName;

					if ((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
						node = new DirNode(this, name, res);
					else
						node = new FileNode(this, FindFileData);

					childs.BInsert(node, SortFunc);
					n.Add() = node;

				}
				else
				{

					if (node->CheckTime(FindFileData))
						n.Add() = node;
				}

				
				if ((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
				{
					Path = folder;
					Path += L"\\";
					Path += FindFileData.cFileName;
					((DirNode*)node)->CheckDir(Path, mask, n);
				}


			}
			memset(&FindFileData, 0, sizeof(WIN32_FIND_DATA));

		} while (FindNextFile(hFile, &FindFileData));

		FindClose(hFile);
		return true;
		return 1;
	}

	void DirNode::UpdateAddFile(const MVector<WString>& mask, const WString& folder)
	{

		WString  path;
		FileNode* node;
		DirNode* dirnode;
		WString ppath = folder;
		path += name;
		path += L"\\";

		if (ppath.Right(1) != L"\\")
			ppath += L"\\";

		for (int i = 0, l = childs.GetLen(); i < l; ++i)
		{
			node = childs[i];
			if (dirnode = node->IsDir())
			{
				path = ppath;
				path += node->name;
				dirnode->UpdateAddFile(mask, path);
			}
		}

		path = ppath.GetPtr();
		ppath = path;
		path += L"*.*";

		WIN32_FIND_DATAW FindFileData;
		memset(&FindFileData, 0, sizeof(WIN32_FIND_DATAW));

		HANDLE hFile = FindFirstFileW(path, &FindFileData);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			DWORD dw = GetLastError();
			if (GetLastError() != ERROR_FILE_NOT_FOUND)
				return;
			return;
		}
		do
		{

			if (!wcscmp(FindFileData.cFileName, L"."))
				continue;
			if (!wcscmp(FindFileData.cFileName, L".."))
				continue;
			WString name;
			name = FindFileData.cFileName;
			if ((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY)
			{
				WString n = FindFileData.cFileName;
				int lj, j = n.ReverseFind('.');
				if (j != -1)
				{
					WString ext = n.Right(n.StrLen() - j);
					for (j = 0, lj = mask.GetLen(); j < lj; ++j)

						if (!ext.CompareNoCase(mask[j]))
						{
							childs.BInsert(new FileNode(this, FindFileData), SortFunc);
							break;
						}
				}

			}
			memset(&FindFileData, 0, sizeof(WIN32_FIND_DATAW));
		} while (FindNextFileW(hFile, &FindFileData));
		FindClose(hFile);


	}
	void DirNode::Clear()
	{
		FileNode::Clear();
		for (int i = 0, l = childs.GetLen(); i < l; ++i)
		{
			delete childs[i];
		}
		childs.Clear();

	}
	//-----------------------------------
	//
	//-----------------------------------

	int DirNode::GetFileList(MVector<WString>& mask, MArray<DirNode*>& dirs, MArray<FileNode*>& files)
	{
		FileNode* node;
		DirNode* dirnode;
		for (int i = 0, l = childs.GetLen(); i < l; ++i)
		{
			node = childs[i];

			if (dirnode = node->IsDir())
				dirs.Add(dirnode);
			else
			{
				WString n = node->name;
				if (n.StrLen() > 4)
				{
					int lj, j = n.ReverseFind('.');
					if (j != -1)
					{
						WString ext = n.Right(n.StrLen() - j);
						if (!mask.GetLen())
							files.Add(node);
						else
							for (j = 0, lj = mask.GetLen(); j < lj; ++j)

								if (ext.CompareNoCase(mask[j]))
								{
									files.Add(node);
									break;
								}
					}
				}
			}
		}
		return childs.GetLen();
	}
	//-----------------------------------
	//
	//-----------------------------------

	int DirNode::GetFileList(MArray<DirNode*>& dirs, MArray<FileNode*>& files)
	{

		FileNode* node;
		DirNode* dir;
		for (int i = 0, l = childs.GetLen(); i < l; ++i)
		{
			node = childs[i];

			if ((dir = node->IsDir()))
				dirs.Add(dir);
			else
				files.Add(node);

		}
		return childs.GetLen();
	}

	//-----------------------------------
	//
	//-----------------------------------
	void DirNode::MakeTree(Tree* data)
	{
		FileNode* node;
		DirNode* dir;
		bool fAdd = true;
		for (int i = 0, l = childs.GetLen(); i < l; ++i)
		{
			node = childs[i];

			if ((dir = node->IsDir()))
			{
				if (fAdd)
				{
					fAdd = false;
					data->CreateNode(L"folders");
				}
				data->CreateNode(L"folder");
				data->CreateNode(L"name");
				data->AddData(&node->name);
				data->CloseNode(L"name");
				dir->MakeTree(data);
				data->CloseNode(L"folder");
			}

		}
		if (!fAdd)
			data->CloseNode(L"folders");
		fAdd = true;
		for (int i = 0, l = childs.GetLen(); i < l; ++i)
		{
			node = childs[i];

			if (!node->IsDir())
			{
				if (fAdd)
				{
					fAdd = false;
					data->CreateNode(L"files");
				}
				data->CreateNode(L"file");
				data->CreateNode(L"name");
				data->AddData(&node->name);
				data->CloseNode(L"name");
				data->CreateNode(L"date");
				data->AddData(&node->date);
				data->CloseNode(L"date");

				data->CloseNode(L"file");
			}
		}
		if (!fAdd)
			data->CloseNode(L"files");

	}
	//-----------------------------------
	//
	//-----------------------------------
	void AddTab(WString& f, int iTab)
	{
		for (int i = 0; i < iTab; ++i)
			f += "    ";

	}
	//-----------------------------------
		//
		//-----------------------------------
	void AddEnter(WString& f)
	{
		f += "\r\n";
	}
	//-----------------------------------
		//
		//-----------------------------------

	void DirNode::DeleteNode(FileNode* n)
	{
		for (int i = 0, l = childs.GetLen(); i < l; ++i)
			if (childs[i] == n)
			{
				delete childs[i];
				childs.Remove(i, 1);
				break;
			}

	}
	//-----------------------------------
	//
	//-----------------------------------

	void DirNode::MakeXML(WString& data, int iTab)
	{
		FileNode* node;
		DirNode* dir;
		bool fAdd = true;
		for (int i = 0, l = childs.GetLen(); i < l; ++i)
		{
			node = childs[i];

			if ((dir = node->IsDir()))
			{
				if (fAdd)
				{
					fAdd = false;
					data += "<folders>";
					++iTab;
				}
				data += "<folder>";
				++iTab;

				data += "<name>";
				data += node->name;
				data += "</name>";
				dir->MakeXML(data, iTab);
				--iTab;
				data += "</folder>";
			}

		}
		if (!fAdd)
		{
			--iTab;
			data += "</folders>";
		}

		fAdd = true;
		for (int i = 0, l = childs.GetLen(); i < l; ++i)
		{
			node = childs[i];

			if (!node->IsDir())
			{
				if (fAdd)
				{
					fAdd = false;
					data += "<files>";
				}
				data += "<file>";
				data += "<name>";
				
				data += node->name;
				data += "</name>";
				if (node->date.StrLen())
				{
					data += "<date>";
					data += node->date;
					data += "</date>";
				}

				data += "</file>";
			}
		}
		if (!fAdd)
			data += "</files>";

	}

	//-----------------------------------
	//
	//-----------------------------------

	int DirNode::GetFileListAll(MArray<FileNode*>& list)
	{
		FileNode* node;
		DirNode* dir;
		int j = 0;


		for (int i = 0, l = childs.GetLen(); i < l; ++i)
		{
			node = childs[i];
			if (dir = node->IsDir())
				j += dir->GetFileListAll(list);
			else
			{
				list.Add(node);
				++j;
			}
		}
		return j;
	}
	int DirNode::GetFileListAll(MVector<WString>& mask, MArray<FileNode*>& list)
	{
		FileNode* node;
		DirNode* dir;
		int j = 0;


		for (int i = 0, l = childs.GetLen(); i < l; ++i)
		{
			node = childs[i];
			if (dir = node->IsDir())
				j += dir->GetFileListAll(mask, list);
			else
			{
				WString n = node->name;
				int lj, j = n.ReverseFind('.');
				if (j != -1)
				{
					WString ext = n.Right(n.StrLen() - j);
					for (j = 0, lj = mask.GetLen(); j < lj; ++j)

						if (!ext.CompareNoCase(mask[j]))
						{
							list.Add(node);
							++j;
							break;
						}
				}
			}
		}
		return j;
	}
	//-----------------------------------
	//
	//-----------------------------------

	FileNode* DirNode::GetChild(const WString& name, bool fDir)
	{
		FileNode* node;
		FileNode f(name);
		int l = childs.GetLen(), i = childs.BFind(&f, SortFunc);
		if (i > -1)
		for (; i < l; ++i)
		{
			node = childs[i];
			if (node->name == name)
			{
				if ((fDir && node->IsDir()) || (!fDir && !node->IsDir()))
						return node;
			}
			else
				break;
		}
		return 0;
	}
	//-----------------------------------
	//
	//-----------------------------------

	FileNode* DirNode::GetChild(const WString& name)
	{
		FileNode f(name);
		int i = childs.BFind(&f, SortFunc);
		return i > -1 ? childs[i] : 0;
		/*for (int i = 0, l = childs.GetLen(); i < l; ++i)
		{
			if (childs[i]->name == name)
				return childs[i];
		}
		*/
		return 0;
	}
	//-----------------------------------
	//
	//-----------------------------------

	DirNode *DirNode::AddDir(MVector<WString>& st, int i)
	{

		WString& s = st[i];
		DirNode* dir = 0;
		FileNode* node = GetChild(s);
		if (node == 0 || !(dir = node->IsDir()))
		{
			dir = new DirNode(this, st[i]);
			childs.BInsert(dir, SortFunc);
		}
		if (st.GetLen() > i + 1)
			dir = dir->AddDir(st, ++i);
		return dir;
	}
	FileNode* DirNode::AddFile(MVector<WString>& st)
	{
		WString sfile = st[st.GetLen() - 1];
		st.SetLen(st.GetLen() - 1);
		DirNode* dir = AddDir(st);
		FileNode* file = 0;
		if (dir)
		{
			if (!(file = dir->GetChild(sfile)))
			{
				file = new FileNode(dir, sfile);
				dir->childs.BInsert(file, SortFunc);
			}
		}
		return file;

	}
	void DirNode::AddFile(WString &s)
	{
		MVector<WString> st;
		s.Tokenize(L"\\/,", st);
		AddFile(st);
	}
	void DirNode::AddDir(WString &s)
	{
		MVector<WString> st;
		s.Tokenize(L"\\/,", st);
		AddDir(st);
	}

//	DirNode* AddDir(MVector<WString>& st, int i);
	//void AddFile(WString& st);

	//-----------------------------------
	//
	//-----------------------------------

	FileNode* DirNode::GetNode(MVector<WString>& st, int i)
	{

		WString& s = st[i];
		DirNode* dir;
		FileNode* node = GetChild(s);
		if (st.GetLen() > i + 1)
			if (node)
			{
				if (node)
				{
					if (dir = node->IsDir())
						dir->GetNode(st, ++i);
				}
			}
		return node;
	}
	//-----------------------------------
		//
		//-----------------------------------
	FileNode::FileNode(FileNode* _parent, Node* file)
	{
		parent = _parent;
		if (file)
		{
			Node* n = file->Get(L"name");
			if (n && n->data)
				name = *n->data;
			n = file->Get(L"date");
			if (n && n->data)
				date = *n->data;
		}

	}
	FileNode::FileNode(FileNode* _parent, const WString& folder)
	{
		parent = _parent;
		name = folder;

	}
	FileNode::FileNode(FileNode* _parent, const WString& folder, WString& time)
	{
		parent = _parent;
		name = folder;
		date = time;

	}
	FileNode::FileNode(FileNode* _parent, WIN32_FIND_DATAW &FindFileData)
	{
		parent = _parent;
		name = FindFileData.cFileName;
		FILETIME localTime;
		if (FileTimeToLocalFileTime(&FindFileData.ftLastWriteTime, &localTime))
		{
			SYSTEMTIME st;
			FileTimeToSystemTime(&localTime, &st);
			date.Format(L"%d-%02d-%02d %02d-%02d-%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
		}
		
	}
	BOOL FileNode::CheckTime(WIN32_FIND_DATAW& FindFileData)
	{
		FILETIME localTime;
		if (date.StrLen())
		if (FileTimeToLocalFileTime(&FindFileData.ftLastWriteTime, &localTime))
		{
			SYSTEMTIME st, newst;
			FileTimeToSystemTime(&localTime, &newst);
			MVector<WString> wdate;
			date.Tokenize(L"- ", wdate);
			if (wdate.GetLen() == 6)
			{
				st.wYear = wdate[0].ToInt();
				st.wMonth = wdate[1].ToInt();
				st.wDay = wdate[2].ToInt();
				st.wHour = wdate[3].ToInt();
				st.wMinute = wdate[4].ToInt();
				st.wSecond = wdate[5].ToInt();
				return CompareFileTime(newst, st) > 0;
					
			}

			
		}
		return 0;
	}

	//-----------------------------------
	//
	//-----------------------------------
	FileNode::FileNode()
	{
		parent = 0;
	}
	//-----------------------------------
	//
	//-----------------------------------
	FileNode::FileNode(const WString& _name)
	{
		parent = 0;

		name = _name;
	}
	//-----------------------------------
	//
	//-----------------------------------
	void FileNode::Clear()
	{
		parent = 0;
		name.Clear();
	}

	DirNode* FileNode::IsDir()
	{
		FileNode* p = this;
		return dynamic_cast<DirNode*>(p);
	}
	//-----------------------------------
	//
	//-----------------------------------
	WString FileNode::GetPath(const WString& s)
	{
		if (parent)
			return parent->GetPath(name + "/" + s);
		WString ret = "/";
		ret += s;
		return ret;
	}
	//-----------------------------------
	//
	//-----------------------------------
	WString FileNode::GetPath()
	{
		if (parent && name.StrLen())
			return parent->GetPath(name);
		return "/";

	}
	//=============================================
	//
	//=============================================
	void DirNode::CopyTo(DirNode* dir)
	{
		dir->childs.SetLen(childs.GetLen());
		DirNode* _dir, *ldir;
		for (int i = 0, l = childs.GetLen(); i < l; ++i)
		{
			if (ldir = childs[i]->IsDir())
			{
				_dir = new DirNode(dir, childs[i]->name);
				dir->childs[i] = _dir;
				ldir->CopyTo(_dir);
			}
			else
			dir->childs[i] =  new FileNode(dir, childs[i]->name, childs[i]->date);
		}
	}
	//-----------------------------------
	//
	//-----------------------------------
	void DirNode::MkDirs(const WString& _ph)
	{
		DWORD dw;
		if ((dw = GetFileAttributesW(_ph.GetPtr())) == 0xFFFFFFFF || (dw & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY)
			CreateDirectoryW(_ph.GetPtr(), 0);
		DirNode *dirnode;
		for (int i = 0, l = childs.GetLen(); i < l; ++i)
		{
			FileNode* node = childs[i];
			if (dirnode = node->IsDir())
			{
				WString ph = _ph;
				ph += L"\\";
				ph += node->name;
				dirnode->MkDirs(ph);
			}
		}

	}
	void DirNode::Sort()
	{
		DirNode* dirnode;
		if (childs.GetLen() > 1)
		{
			childs.QSort(SortFunc);
			for (int i = 0, l = childs.GetLen(); i < l; ++i)
			{
				if (dirnode = childs[i]->IsDir())
				{
					dirnode->Sort();
				}
			}
		}

	}



	Tree::Tree()
	{

		root = new Node(0, 0);
		last = root;
	}
	void Tree::MakeHtml(WString& data)
	{

		root->MakeHtml(0, data);
	}
	Tree::Tree(WString& xml)
	{
		root = new Node(0, 0);
		last = root;

		WString  tmp;
		tmp = xml;
		int i = tmp.Find(L"<file-area>", 0);
		if (i == -1) return;
		WString orig = L"<file-area>";

		bool first = 0;
		bool last = 0;
		wchar_t* token, * string = tmp.GetPtr() + i;
		const wchar_t* ctl;
		const wchar_t* control = L"<>";
		for (;;)
		{
			while (*string)
			{
				for (ctl = control; *ctl && *ctl != *string; ++ctl);
				if (!*ctl)
					break;
				if (*string == L'<')
					first = 1;

				if (*string == L'>')
					last = 1;
				string++;
			}
			token = string;
			for (; *string; ++string)
			{
				for (ctl = control; *ctl && *ctl != *string; ++ctl);
				if (*ctl)
				{
					if (*string == L'<')
						first = 1;

					if (*string == L'>')
						last = 1;

					*string++ = L'\0';
					break;
				}
			}
			WString test = token;
			if (test == L"file-area")
			{
				i = 1;
			}
			if (token == string)
				break;
			if (first && last)
			{
				if (token[0] == L'/')
					CloseNode(token + 1);
				else
					CreateNode(token);
				first = false;
				last = false;
			}
			else
			{
				WString s = token;
				AddData(&s);
			}

			//arr.Add() = token;
		}


	}
	//------------------------------------------------------
		  //
		  //------------------------------------------------------
	void Tree::CreateNode(const WString& name)
	{
		Node* node = new Node(last, name);
		last->child.Add(node);
		last = node;
	}
	//------------------------------------------------------
	//
	//------------------------------------------------------
	void Tree::AddData(WString* data)
	{
		last->data = data == 0 ? 0 : new WString(*data);
	}
	//------------------------------------------------------
	//
	//------------------------------------------------------
	void Tree::CloseNode(const WString& name)
	{
		if (last->name != name)
			return;
		last = last->parent;
	}
	//------------------------------------------------------
	//
	//------------------------------------------------------
	WString* Tree::Get(const WString& path)
	{
		Node* n = root->Get(path);
		return n ? n->data : 0;
	}

	Node::Node(Node* _parent, const WString& n)
	{
		parent = _parent;
		name = n;
		data = 0;

		fClosed = false;

	}
	//------------------------------------------------------
	//
	//------------------------------------------------------
	Node* Node::GetChild(const WString& nm)
	{
		for (int i = 0, l = child.GetLen(); i < l; ++i)
		{
			if (child[i]->name == nm)
				return child[i];
		}
		return 0;
	}
	//------------------------------------------------------
	//
	//------------------------------------------------------
	Node* Node::Get(const WString& _path)
	{
		Node* node = this;
		MVector<WString> res;
		WString path = _path;
		path.Tokenize(L"/", res);
		for (int i = 0, l = res.GetLen(); i < l; ++i)
		{
			node = node->GetChild(res[i]);
			if (!node)
				return 0;
		}
		return node;
	}
	void Node::MakeHtml(int nTab, WString& str)
	{
		if (name.StrLen())
		{
			//	for (int i = 0; i < nTab; ++i)
			//		str += "    ";
			str += "<";
			str += name;
			str += ">";
			++nTab;
		}
		if (data != 0)
			str += *data;
		//	else
		//	str += "\r\n";


		for (int i = 0, l = child.GetLen(); i < l; ++i)
		{
			child[i]->MakeHtml(nTab, str);
		}
		if (name.StrLen())
		{

			--nTab;
			//	if (data == 0)
			//	for (int i = 0; i < nTab; ++i)
				//	str += "    ";

			str += "</";
			str += name;
			str += ">";
		}
	}
	//------------------------------------------------------
	//
	//------------------------------------------------------
	Node* Node::FindFolder(const WString& nm)
	{
		if (name == nm) return this;
		for (int i = 0, l = child.GetLen(); i < l; ++i)
		{
			Node* d = child[i]->FindFolder(nm);

			if (d)
				return d;
		}

		return 0;
	}
	Node::~Node()
	{
		if (data != 0)
			delete data;
		for (int i = 0, l = child.GetLen(); i < l; ++i)
			delete child[i];

	}
	//------------------------------------------------------
	//
	//------------------------------------------------------
	Node* Node::FindData(const WString& nm)
	{

		if (child.GetLen())
		{
			for (int i = 0, l = child.GetLen(); i < l; ++i)
			{
				Node* c = child[i];
				if (c->data && *c->data == nm)
					return c;
				Node* d = c->FindData(nm);

				if (d)
					return d;
			}
		}
		return 0;

	}
}
