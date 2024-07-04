#pragma once
namespace NetDir
{
	void AddTab(WString &f, int iTab);
	void AddEnter(WString &f);

	struct Node
	{
		WString name;
		WString* data;
		MArray<Node*> child;
		bool fClosed;
		Node* parent;
		Node(Node* _parent, const WString & n);
		~Node();
		Node* FindData(const WString & nm);
		Node* FindFolder(const WString & nm);
		Node* Get(const WString & path);
		Node* GetChild(const WString & nm);
		void MakeHtml(int nTab, WString& data);
	};
	struct Tree
	{
		Node* root;
		Node* last;
		Tree();
		Tree(WString &xml);
		~Tree() 
		{
			delete root;
		}
		WString* Get(const WString & path);
		void CloseNode(const WString & name);
		void AddData(WString* data);
		void CreateNode(const WString & name);
		void MakeHtml(WString& data);
	};
	class DirNode;
	class FileNode {
	public:
		WString name;
		WString date;
		FileNode* parent;
		WString GetPath();
		virtual void Clear();
		WString GetPath(const WString& s);
		DirNode *IsDir();
		FileNode(const WString & _name);
		FileNode();
		FileNode(FileNode* _parent, Node* file);
		FileNode(FileNode* _parent, const WString & folder);
		FileNode(FileNode* _parent, const WString& folder, WString &time);
		FileNode(FileNode* _parent, WIN32_FIND_DATAW& FindFileData);
		BOOL CheckTime(WIN32_FIND_DATAW& FindFileData);
		virtual ~FileNode() {}


	};
	class DirNode : public  FileNode {
	public:
		MArray<FileNode*> childs;
		virtual ~DirNode()
		{
			Clear();
		}
		virtual void Clear();

		virtual void UpdateAddFile(const MVector<WString>& mask, const WString& folder);
		int CheckDir(MArray<FileNode> &);
		DirNode(FileNode* _parent, const WString & name, const WString & folder);
		DirNode(FileNode* _parent, Node* folder);
		DirNode(FileNode* _parent, const WString& folder) :FileNode(_parent, folder) {}
		DirNode* AddDir(MVector<WString>& st, int i = 0);
		FileNode *AddFile(MVector<WString>& st);
		void AddDir(WString &s);
		void AddFile(WString &s);
		int CheckDir(const WString& folder, LPCTSTR mask, MArray<FileNode *>& n);

		int GetFileList(MVector<WString>& mask, MArray<DirNode*>& dirs, MArray<FileNode*>& files);
		int GetFileList(MArray<DirNode*>& dirs, MArray<FileNode*>& files);
		FileNode *GetChild(const WString& name, bool fDir);
		int GetFileListAll(MArray<FileNode*>& list);
		int GetFileListAll(MVector<WString>& mask, MArray<FileNode*>& list);
		FileNode* GetChild(const WString& name);

		FileNode* GetNode(MVector<WString>& st, int i);
		FileNode* GetNode(MVector<WString>& st) { return GetNode(st, 0); }
		void MakeTree(Tree* data);
		void MakeXML(WString &data, int iTab);
		void DeleteNode(FileNode *n);
		virtual void MkDirs(const WString& _ph);
		void Sort();
		void CopyTo(DirNode* dir);



	};
	class TreeDir {
	public:
		DirNode* root;
		TreeDir() {
			root = new DirNode(0, 0);
		}
		TreeDir(Tree* data);
		TreeDir(const WString & path);
		~TreeDir() {
			if (root) delete root;
		}
		void Clear() {
			if (root) delete root;
			root = new DirNode(0, 0);
		}
		DirNode* Root() {
			return root;
		}
		bool IsEmpty() {
			return root->childs.GetLen() == 0;
		}

		FileNode *GetFile(const WString & filepath);
		int GetFileList(const WString & dir, MVector<WString>& mask, MArray<DirNode*>& dirs, MArray<FileNode*>& files);
		int GetFileList(const WString & dir, MArray<DirNode*>& dirs, MArray<FileNode*>& files);
		int GetFileList(MArray<FileNode*>& list);
		int GetFileList(MVector<WString>& mask, MArray<FileNode*>& list);
		void DeleteNode(FileNode *node);
		FileNode *MakeFileNode(const WString & name);
		void UpdateAddFile(const MVector<WString> &mask, const WString& folder);
		void AddDir(WString &s) {
			root->AddDir(s);
		}
		void AddFile(WString &s) {
			root->AddFile(s);
		}
		void MakeTree(Tree* data);
		void MakeXML(WString &data);
		void MkDirs(const WString& _ph);


	};




};
BOOL __CreateDirectoryW(LPTSTR ph, LPSECURITY_ATTRIBUTES n);

