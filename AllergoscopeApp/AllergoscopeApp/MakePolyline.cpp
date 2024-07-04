#include "stdafx.h"
#include "MakePolyline.h"
#include "AproxLine.h"
#include "AllergoscopeApp.h"
#include "BuilderVoron2D.h"

#include <atlimage.h>
#include "_ffile.h"
float dX = 0;
float dY = 0;
extern int OnError(LPCTSTR);
MakePolyLine::MakePolyLine(MVector<WString> &path, MVector <Variable> &vresult, BOOL fRecalc)
{
	
	BOOL fEnter = TRUE;
	((AllergoscopeApp*)AfxGetApp())->waitExe.Enter();

	vresult.Clear();
	if (path.GetLen())
	{
		STARTUPINFO si = { sizeof(si) };
		PROCESS_INFORMATION pi;
		MVector<WString> path_bm = path;
		FFileINI file("allergoscopeeditor.ini", true);

		CString FileName = file.GetString(L"PATH", L"PLUK");
		if (FileName.GetBuffer(0)[FileName.GetLength() - 1] != '\\')
			FileName += "\\";
		if (false)
		{
			CString NN;
			NN = file.GetString(L"NAME", L"NN");
			if (!NN.GetLength())
				NN = L"NN-2019-11-08-L21-0.976831--29172.5.nn";
			CString sz = FileName;
			sz += L"nn-test.exe ";
			sz += L"\"";

			sz += FileName;
			sz += NN; // L"NN-2019-11-08-L21-0.976831--29172.5.nn";
			sz += L"\"";
			sz += " ";
			sz += L"\"";
			WString tmpDir;
			MFILE f; // ("h:\\bin64u\\AEExchange\\ae_exchange.tmp");
			if (f.Open(true))
			{
				WString s;
				for (int i = 0; i < path_bm.GetLen(); ++i)
				{
					path_bm[i].Replace(L".bin", L".jpg");
					s += path_bm[i];
					s += L"\r\n";
				}
				s.ReplaceAll(L"/", L"\\");
				f.Write(s.GetPtr(), s.StrLen() * 2);
				f.Close();
			}
			WString _path_bm = f.Name();
			sz += _path_bm;
			sz += L"\"";
			//		sz += " ";
			/*		sz += L"\"";
					path_bm.Replace(L".jpg", L".bin");
					sz += path_bm;*/
			sz += L" ";
			if (fRecalc)
			{
				sz += L"/recalc";
				sz += L" ";
			}

			sz += L"/lAllergoscopeApp.log";
			CString ss = sz + "\r\n";
			LPCTSTR Err = ss.GetBuffer(0);

			if (CreateProcess(NULL, sz.GetBuffer(0),
				NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
			{
				while (WaitForSingleObject(pi.hProcess, 1000) == WAIT_TIMEOUT)
				{
					;
				}
				CloseHandle(pi.hThread);
				CloseHandle(pi.hProcess);
			}
			else
			{
				OnError(Err);
				((AllergoscopeApp*)AfxGetApp())->waitExe.Leave();
				return;
			}

		}
		else
		{
			WString tmpDir;
			GUID id;
			CoCreateGuid(&id);
			WString olestr;
			olestr.SetLen(128);
			StringFromGUID2(id, olestr.GetPtr(), 128);
			WString cmp = FileName + L"AEExchange\\";;
			cmp += olestr + ".cmp";
			WString tmp = FileName + L"AEExchange\\";
			tmp += olestr + ".tmp";
			FILE* f = _wfopen(cmp.GetPtr(), _T("wb"));
			if (f)
			{
				WString s;
				for (int i = 0; i < path_bm.GetLen(); ++i)
				{
					path_bm[i].Replace(L".bin", L".jpg");
					s += path_bm[i];
					s += L"\r\n";
				}
				s.ReplaceAll(L"/", L"\\");
				fwrite(s.GetPtr(), s.StrLen() * 2, 1, f);

				fclose(f);

				MoveFile(cmp.GetPtr(), tmp.GetPtr());
				while (GetFileAttributes(tmp.GetPtr()) != 0xffffffff)
				{
					Sleep(1000);

				}
			}
			else
				return;
		}
	}
	

	UString filename;
	vresult.SetLen(path.GetLen());
	for (int i = 0; i < path.GetLen(); ++i)
	{
		filename = path[i];
		filename.Replace(L".bin", L".ctr");
		X = Y = 0;
		float _X = 0;
		float _Y = 0;
		Variable& result = vresult[i];
		MFILE file(filename.GetPtr());
		if (file.Open(false))
		{
			MArray<char> ch(file.GetSize());
			file.Read(0, ch.GetPtr(), file.GetSize());

			X = 600;
			Y = 158;
			dX = _X / (float)X;
			dY = _Y / (float)Y;
			file.Close();
			if (!memcmp(ch.GetPtr(), "CTR01", 5))
			{
				char* p = ch.GetPtr();
				p += 5;
				int len = *(short*)p;
				p += 2;
				result.Dim(len);

				for (int i = 0; i < len; ++i)
				{
					int l = *(int*)p;
					p += 4;

					Variable& v = result[i];
					v.Dim(2);
					TString n;
					n.Format("%d", i + 1);
					v[0] = n;
					MArray<BYTE>* b = v[1].InitArray();
					b->SetLen(l * sizeof(Vector2F));
					Vector2F* a = (Vector2F*)b->GetPtr();
					for (int j = 0; j < l; ++j)
					{
						a[j].x = *(float*)p;
						p += 4;
						a[j].y = *(float*)p;
						p += 4;
					}

				}

			}

		}
	}
	((AllergoscopeApp*)AfxGetApp())->waitExe.Leave();
}
//--------------------------------------------------------------
//
//--------------------------------------------------------------
MakePolyLine::MakePolyLine(LPCTSTR path, Variable& result, BOOL fRecalc)
{
	UString filename = path;
	result.Clear();
	((AllergoscopeApp*)AfxGetApp())->waitExe.Enter();
	if (fRecalc || GetFileAttributes(path) == 0xffffffff )
	{
		STARTUPINFO si = { sizeof(si) };
		PROCESS_INFORMATION pi;
		WString path_bm = path;
		FFileINI file("allergoscopeeditor.ini", true);

		CString FileName = file.GetString(L"PATH", L"PLUK");
		if (FileName.GetBuffer(0)[FileName.GetLength() - 1] != '\\')
			FileName += "\\";
		if (false)
		{
			CString NN;
			NN = file.GetString(L"NAME", L"NN");
			if (!NN.GetLength())
				NN = L"NN-2019-11-08-L21-0.976831--29172.5.nn";
			CString sz = FileName;
			sz += L"nn-test.exe ";
			sz += L"\"";

			sz += FileName;
			sz += NN; // L"NN-2019-11-08-L21-0.976831--29172.5.nn";
			sz += L"\"";
			sz += " ";
			sz += L"\"";
			WString tmpDir;
			MFILE f;
			if (f.Open(true))
			{
				path_bm.Replace(L".bin", L".jpg");
				f.Write(path_bm.GetPtr(), path_bm.StrLen() * 2);
				f.Close();
			}
			path_bm = f.Name();
			sz += path_bm;
			sz += L"\"";
			//		sz += " ";
			/*		sz += L"\"";
					path_bm.Replace(L".jpg", L".bin");
					sz += path_bm;*/
			sz += L" ";
			if (fRecalc)
			{
				sz += L"/recalc";
				sz += L" ";
			}

			sz += L"/lAllergoscopeApp.log";
			CString ss = sz + "\r\n";
			LPCTSTR Err = ss.GetBuffer(0);

			if (CreateProcess(NULL, sz.GetBuffer(0),
				NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
			{
				while (WaitForSingleObject(pi.hProcess, 1000) == WAIT_TIMEOUT)
				{
					;
				}
				CloseHandle(pi.hThread);
				CloseHandle(pi.hProcess);
			}
			else
			{
				OnError(Err);
				((AllergoscopeApp*)AfxGetApp())->waitExe.Leave();
				return;
			}
		}
		else
		{
			WString tmpDir;
			GUID id;
			CoCreateGuid(&id);
			WString olestr;
			olestr.SetLen(128);
			StringFromGUID2(id, olestr.GetPtr(), 128);
			WString cmp = FileName + L"AEExchange\\";
			cmp += olestr + ".cmp";
			WString tmp = FileName + L"AEExchange\\";
			tmp += olestr + ".tmp";
			FILE* f = _wfopen(cmp.GetPtr(), _T("wb"));
			if (f)
			{
				fwrite(path_bm.GetPtr(), path_bm.StrLen() * 2, 1, f);
				fclose(f);

				MoveFile(cmp.GetPtr(), tmp.GetPtr());
				while (GetFileAttributes(tmp.GetPtr()) != 0xffffffff)
				{
					Sleep(1000);

				}
				Sleep(1000);

			}
			else
				return;
			

		}

	}
	//CString path_bm = path;
	filename.Replace(L".bin", L".ctr");
	X = Y = 0;
	float _X = 0;
	float _Y = 0;
	/*
	{
		CImage img;
		img.Load(path_bm);
		_X = img.GetWidth();
		_Y = img.GetHeight();

		//X = img.GetWidth()/2;
		//Y = img.GetHeight() / 2;
		
	}
	*/
//	path_bm = path;
	MFILE file(filename.GetPtr());
	if (file.Open(false))
	{
		MArray<char> ch(file.GetSize());
		file.Read(0, ch.GetPtr(), file.GetSize());

		X = 600;
		Y = 158;
		dX = _X / (float)X;
		dY = _Y / (float)Y;
		file.Close();
		if (!memcmp(ch.GetPtr(), "CTR01", 5))
		{
			char* p = ch.GetPtr();
			p += 5;
			int len = *(short*)p;
			p += 2;
			result.Dim(len);

			for (int i = 0; i < len; ++i)
			{
				int l = *(int*)p;
				p += 4;
				
				Variable& v = result[i];
				v.Dim(2);
				TString n;
				n.Format("%d", i + 1);
				v[0] = n;
				MArray<BYTE>* b = v[1].InitArray();
				b->SetLen(l * sizeof(Vector2F));
				Vector2F* a = (Vector2F * )b->GetPtr();
				for (int j = 0; j < l; ++j)
				{
					a[j].x = *(float*)p;
					p += 4;
					a[j].y = *(float*)p;
					p += 4;
				}

			}
			
		}

#if (0)
		const char* names[] = { "left iris", "left pupis",  "right iris", "right pupis", "nose" };
		int k = 0;
		result.Dim(1);
		for (char i = 1; i <= 1; ++i, ++k)
		{
				if (Init(i, ch, X, Y))
			{
				Variable& v = result[k];
				v.Dim(1 + res.GetLen());
				v[0] = names[k];
				for (int j = 0; j < res.GetLen(); ++j)
				{
					MArray<BYTE>* b = v[j + 1].InitArray();
					b->Copy((BYTE*)res[j].GetPtr(), res[j].GetLen() * sizeof(Vector2F));
				}
			}
		}
#endif
	}
	((AllergoscopeApp*)AfxGetApp())->waitExe.Leave();


}
//--------------------------------------------------------------
//
//--------------------------------------------------------------
bool MakePolyLine::Init(char color, MArray<char>& _data, int _X, int _Y)
{
	data.Clear();
	pindex.Clear();
	index.Clear();;
	adj.Clear();
	arr.Clear();
	ord.Clear();
	stack.Clear();
	res.Clear();
	X = _X;
	Y = _Y;
	char* p = _data.GetPtr() +  (Y - 1)*X;

	for (int y = _Y - 1; y >= 0; --y, p -= X)
	{
		data.Append(p, X);
	}
//	data = _data;
	MArray<char> ou(data.GetLen(), 0);

	p = data.GetPtr();

	for (int i = 0, l = data.GetLen(); i < l; ++i)
	{
		if (p[i] != color)
			p[i] = 0;

	}
	FilterBmpLoop(1, (BYTE*)p, (BYTE*)ou.GetPtr(), X, Y, color);
	FilterBmpLoop(0, (BYTE*)p, (BYTE*)ou.GetPtr(), X, Y, color);
	data = ou;
	ou.Clear();


	int N1 = 0, N = 0;
	for (int i = 0, l = data.GetLen(); i < l; ++i)
		N1 += p[i] != 0;
	if (!N1) return false;

	Make8Graph();
	MakeLines();
	MArray<Vector2F> arr_tmp;
	MArray<int> pos;

	SmoothPoints(arr_tmp, pos);


	InitEdgeGraph(arr_tmp, pos, true, true);
	for (int j, l, i = 0, len = index.GetLen(); i < len; ++i)
		if ((l = index[i].GetLen()))
		{
			MArray<Vector2F>& a = res.Add();
			MArray<int>& id = index[i];
			a.SetLen(l);
			for (j = 0; j < l; ++j)
			{
				a[j] = arr[id[j]];
				a[j].x *= dX;
				a[j].y *= dY;
			}

			
		}
	return true;

}
//--------------------------------------------------------------
//
//--------------------------------------------------------------
void MakePolyLine::FilterBmpLoop(int build, BYTE* ptr, BYTE* out, int _width, int _height, BYTE color)
{
	int  i;
	int sw = 2;
	BYTE* ptr1 = ptr + _width + 1;
	BYTE* px1 = out + _width + 1;

	int lenx = _width - 1;
	int leny = _height - 1;
	lenx -= 2;
	memset(out, 0, _width * _height);
	int sh0 = -1 - _width;
	int sh1 = -_width;
	int sh2 = 1 - _width;
	int sh6 = _width - 1;
	int sh8 = 1 + _width;
	tstFilter matr;
	matr.n = 0;
	BYTE bg_color = 0;

	if (build)
	{
		leny = _height - 1;
		while (--leny)
		{

			matr.matr0 = *(ptr1 + sh0) == color;
			matr.matr1 = *(ptr1 - 1) == color;
			matr.matr2 = *(ptr1 + sh6) == color;
			matr.matr3 = *(ptr1 + sh1) == color;
			matr.matr4 = *ptr1 == color;
			matr.matr5 = *(ptr1 + _width) == color;
			matr.matr6 = *(ptr1 + sh2) == color;
			matr.matr7 = *(ptr1 + 1) == color;
			matr.matr8 = *(ptr1 + sh8) == color;
			px1 += lenx + 1;
			ptr1 += lenx + 1;
			i = lenx + 1;
			while (--i)
			{
				matr.n = (matr.n >> 3) |
					(((*(ptr1 - i + sh2) == color) << 6) & 0x40) |
					(((*(ptr1 - i + 1) == color) << 7) & 0x80) |
					(((*(ptr1 - i + sh8) == color) << 8) & 0x100);

				if ((matr.n & 0x10))
				{
					INT J1 = matr.matr6 + matr.matr3 + matr.matr0;
					INT J2 = matr.matr1 + matr.matr7;
					INT J3 = matr.matr2 + matr.matr5 + matr.matr8;
					if (!J1 && !J3)
						* (px1 - i) = 2;
					else
					{
						if (*(px1 - i + 1) == 2)
						{
							int I = lenx + 1;
							while (--I)
								if (*(px1 - I) == 2)
								{
									*(ptr1 - I) = color;
									*(px1 - I) = 0;
								}

						}
					}

				}
				else
				{
					if (*(px1 - i - 1) == 2)
					{
						int I = lenx + 1;
						while (--I)
							if (*(px1 - I) == 2)
							{
								*(ptr1 - I) = 0;
								*(px1 - I) = 0;
							}

					}
				}
			}
			ptr1 += sw;
			px1 += sw;
		}

		ptr1 = ptr + _width + 1;
		px1 = out + _width + 1;
		i = _width - 2;
		memset(out, 0, _width * _height);
		while (--i)
		{

			leny = _height - 1;
			ptr1 = ptr + _width * 2 - i;
			px1 = out + _width * 2 - i;
			while (--leny)
			{


				matr.matr0 = *(ptr1 + sh0) == color;
				matr.matr1 = *(ptr1 - 1) == color;
				matr.matr2 = *(ptr1 + sh6) == color;
				matr.matr3 = *(ptr1 + sh1) == color;
				matr.matr4 = *ptr1 == color;
				matr.matr5 = *(ptr1 + _width) == color;
				matr.matr6 = *(ptr1 + sh2) == color;
				matr.matr7 = *(ptr1 + 1) == color;
				matr.matr8 = *(ptr1 + sh8) == color;


				if ((matr.n & 0x10))
				{
					INT J1 = matr.matr1 + matr.matr2 + matr.matr0;
					INT J2 = matr.matr3 + matr.matr5;
					INT J3 = matr.matr6 + matr.matr7 + matr.matr8;
					if (!J1 && !J3)
						* px1 = 2;
					else
					{

						if (*(px1 + sh1) == 2)
						{
							BYTE* _ptr = ptr + _width - i;
							BYTE* _px = out + _width - i;

							int leny1 = _height - 1;
							while (--leny1)
							{
								if (*_px == 2)
								{
									*_ptr = color;
									*_px = 0;
								}
								_ptr += _width;
								_px += _width;
							}

						}
					}

				}
				else
				{

					if (*(px1 + sh1) == 2)
					{
						BYTE* _ptr = ptr + _width - i;
						BYTE* _px = out + _width - i;

						int leny1 = _height - 1;
						while (--leny1)
						{
							if (*_px == 2)
							{
								*_ptr = 0;
								*_px = 0;
							}
							_ptr += _width;
							_px += _width;
						}
					}
				}
				ptr1 += _width;
				px1 += _width;
			}

		}
	}
	leny = _height - 1;
	ptr1 = ptr + _width + 1;
	px1 = out + _width + 1;
	BYTE C = build ? color : 1;
	while (--leny)
	{

		matr.matr0 = *(ptr1 + sh0) == color;
		matr.matr1 = *(ptr1 - 1) == color;
		matr.matr2 = *(ptr1 + sh6) == color;
		matr.matr3 = *(ptr1 + sh1) == color;
		matr.matr4 = *ptr1 == color;
		matr.matr5 = *(ptr1 + _width) == color;
		matr.matr6 = *(ptr1 + sh2) == color;
		matr.matr7 = *(ptr1 + 1) == color;
		matr.matr8 = *(ptr1 + sh8) == color;

		if (matr.n && !(matr.n & 0x10))
			* px1 = C;
		px1 += lenx + 1;
		ptr1 += lenx + 1;
		i = lenx + 1;
		while (--i)
		{
			if ((matr.n = (matr.n >> 3) |
				(((*(ptr1 - i + sh2) == color) << 6) & 0x40) |
				(((*(ptr1 - i + 1) == color) << 7) & 0x80) |
				(((*(ptr1 - i + sh8) == color) << 8) & 0x100))
				&& !(matr.n & 0x10))
			{
				*(px1 - i) = C;
			}
		}
		ptr1 += sw;
		px1 += sw;
	}
	leny = _height - 1;
	ptr1 = out + _width + 1;
	px1 = ptr + _width + 1;

	if (build)
	{

		int l = _width * _height;
		while (l--)
		{
			if (*out == color)
				* ptr = *out;
			ptr++;
			out++;
		}
	}
	else
	{
		while (--leny)
		{

			matr.matr0 = *(ptr1 + sh0);
			matr.matr1 = *(ptr1 - 1);
			matr.matr2 = *(ptr1 + sh6);
			matr.matr3 = *(ptr1 + sh1);
			matr.matr4 = *ptr1;
			matr.matr5 = *(ptr1 + _width);
			matr.matr6 = *(ptr1 + sh2);
			matr.matr7 = *(ptr1 + 1);
			matr.matr8 = *(ptr1 + sh8);
			ptr1 += lenx + 1;
			i = lenx + 1;
			while (--i)
			{
				matr.n = (matr.n >> 3) |
					(((*(ptr1 - i + sh2)) << 6) & 0x40) |
					(((*(ptr1 - i + 1)) << 7) & 0x80) |
					(((*(ptr1 - i + sh8)) << 8) & 0x100);

				if ((matr.n & 0x10))
				{
					INT J1 = matr.matr1 + matr.matr2 + matr.matr0;
					INT J2 = matr.matr3 + matr.matr5;
					INT J3 = matr.matr6 + matr.matr7 + matr.matr8;
					if ((J1 == 3 && !J2 && !J3) || (J3 == 3 && !J2 && !J1))
						* (ptr1 - i) = 0;
					else
					{
						INT J1 = matr.matr6 + matr.matr3 + matr.matr0;
						INT J2 = matr.matr1 + matr.matr7;
						INT J3 = matr.matr2 + matr.matr5 + matr.matr8;
						if ((J1 == 3 && !J2 && !J3) || (J3 == 3 && !J2 && !J1))
							* (ptr1 - i) = 0;

					}
				}
			}
			ptr1 += sw;
		}
	}
}

//--------------------------------------------------------------
//
//--------------------------------------------------------------
int MakePolyLine::SortIndexFunc(MArray<int>* a, MArray<int>* b)
{
	if (!a->GetLen() && !b->GetLen()) return 0;
	if (!a->GetLen()) return -1;
	if (!b->GetLen()) return 1;

	if (arr[(*a)[0]] > arr[(*b)[0]] || arr[(*a)[0]] > arr[(*b)[b->GetLen() - 1]] ||
		arr[(*a)[a->GetLen() - 1]] > arr[(*b)[0]] || arr[(*a)[a->GetLen() - 1]] > arr[(*b)[b->GetLen() - 1]])
		return 1;
	if (arr[(*b)[0]] > arr[(*a)[0]] || arr[(*b)[0]] > arr[(*a)[a->GetLen() - 1]] ||
		arr[(*b)[b->GetLen() - 1]] > arr[(*a)[0]] || arr[(*b)[b->GetLen() - 1]] > arr[(*a)[a->GetLen() - 1]])
		return -1;

	return 0;
}
//--------------------------------------------------------------------
//
//--------------------------------------------------------------------
int __cdecl SortFunc(const void* _a, const void* _b)
{
	MArray<int>* a = (MArray<int>*)_a;
	MArray<int>* b = (MArray<int>*)_b;
	if (a->GetLen() > b->GetLen()) return -1;
	if (a->GetLen() < b->GetLen()) return 1;
	return 0;
}

//--------------------------------------------------------------------
//
//--------------------------------------------------------------------
void MakePolyLine::helpSort(int* item, int left, int right)
{
	int i, j, y;
	i = left;
	j = right;
	MArray<int>* Data = index.GetPtr() + item[(i + j) / 2];


	do
	{
		while (SortIndexFunc(index.GetPtr() + item[i], Data) < 0 && i <= right)
			++i;
		while (j >= 0 && SortIndexFunc(Data, index.GetPtr() + item[j]) < 0 && j >= left)
			--j;
		if (i <= j)
		{
			if (i < j)
			{
				y = item[i];
				item[i] = item[j];
				item[j] = y;
			}
			++i;
			--j;
		}
	} while (i <= j);

	if (left < j)
		helpSort(item, left, j);
	if (i < right)
		helpSort(item, i, right);
}
//--------------------------------------------------------------
//
//--------------------------------------------------------------

void MakePolyLine::SmoothPoints(MArray<Vector2F>& arr_tmp, MArray<int>& pos)
{

	MArray<Vector2F> tmp, tmp1;
	MArray<int> ipos;
	ipos.SetLen(index.GetLen());
	int* p = ipos.GetPtr();
	int l, i = 0, len = index.GetLen();
	for (; i < len; ++i)
		p[i] = i;

	if (len > 2)
		helpSort(ipos.GetPtr(), 0, len - 1);
	AproxLine<Vector2F, float> test;

	for (i = 0; i < len; ++i)
		if ((l = index[p[i]].GetLen()))
		{

			if (l / 3 > 20)
			{
				MArray<int>& id = index[p[i]];
				tmp.SetLen(l);
				for (int j = 0; j < l; ++j)
					tmp[j] = arr[id[j]];

				pos.Add() = arr_tmp.GetLen();
				tmp1.SetLen(0);
				test.Exec(tmp, tmp1, 20);

				if (tmp[0] != tmp1[0])
					tmp1[0] = tmp[0];
				if (tmp[l - 1] != tmp1[tmp1.GetLen() - 1])
					tmp1[tmp1.GetLen() - 1] = tmp[l - 1];

				arr_tmp += tmp1;
			}
			else
				if (l / 3 > 4)
				{
					MArray<int>& id = index[p[i]];
					tmp.SetLen(l);
					for (int j = 0; j < l; ++j)
						tmp[j] = arr[id[j]];

					pos.Add() = arr_tmp.GetLen();
					tmp1.SetLen(0);
					test.Exec(tmp, tmp1, 4);

					if (tmp[0] != tmp1[0])
						tmp1[0] = tmp[0];
					if (tmp[l - 1] != tmp1[tmp1.GetLen() - 1])
						tmp1[tmp1.GetLen() - 1] = tmp[l - 1];

					arr_tmp += tmp1;
				}
			if (0)
			{
				MArray<int>& id = index[p[i]];
				tmp.SetLen(l);
				for (int j = 0; j < l; ++j)
					tmp[j] = arr[id[j]];

				pos.Add() = arr_tmp.GetLen();
				arr_tmp += tmp;

			}

		}
	pos.Add() = arr_tmp.GetLen();

}
//--------------------------------------------------------------
//
//--------------------------------------------------------------
void MakePolyLine::Make8Graph()
{
	struct Char2
	{
		char x;
		char y;
	};
	const  Char2 ind4[4] = { {-1, 0}, {0, -1}, {0, 1},  {1, 0} };
	const  Char2 ind42[8] = { {-1, -1},{-1, 1}, {1, -1}, {1, 1} };

	//const  Char2 ind9[8] = { {-1, -1},{-1, 0},{-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0},{1, 1} };
	RBMap<int, Vector2F> map;
	int L = X * Y;
	int x, y, num = 0, XY = X * Y;
	Vector2F c;

	MArray<Vector2F> tmp;
	tmp.SetAddedRealSize(X);
	for (y = 0; y < Y; ++y)
	{
		for (x = 0; x < X; ++x)
		{
			int bv = data[x + y * X];
			if (bv != 0)
			{
				c.Set(x, y);
				tmp.Add(c);

				map[c] = num++;
			}
		}
	}

	tmp.Clear();
	adj.Clear();
	arr.Clear();
	arr.SetAddedRealSize(1024);
	adj.SetAddedRealSize(1024);
	adj.SetLen(num + num * 8);

	arr.SetLen(num);
	Vector2F cc, * pos = arr.GetPtr();
	int l = num;
	GNode* _adj = adj.GetPtr();
	memset(_adj, 0, sizeof(GNode) * adj.GetLen());
	memset(pos, 0, sizeof(Vector2F) * num);
	int key;
	bool fRestat = 0;
	RBMap<int, Vector2F>::iterator it(map);
	for (RBData<int, Vector2F>* p = it.begin(); p; p = it.next())
	{
		c = p->key;
		_adj[p->value].num = 0;
		_adj[p->value].pw = l;
		pos[p->value] = c;
		for (int i = 0; i < 4 && _adj[p->value].num < 2; ++i)
		{

			cc.Set(c.x + ind4[i].x, c.y + ind4[i].y);
			if (cc.x >= 0.0f && c.y >= 0.0f && cc.x < X && c.y < Y && map.Lookup(cc, key))
			{
				_adj[l].Set(p->value, key);
				++l;
				++_adj[p->value].num;

			}
		}
		if (_adj[p->value].num <= 1)
		{

			for (int i = 0; i < 4; ++i)
			{

				cc.Set(c.x + ind42[i].x, c.y + ind42[i].y);
				if (cc.x >= 0.0f && c.y >= 0.0f && cc.x < X && c.y < Y && map.Lookup(cc, key))
				{
					_adj[l].Set(p->value, key);
					++l;
					++_adj[p->value].num;

				}
			}
			if (_adj[p->value].num == 1)
			{
				data[(int)c.x + (int)c.y * X] = 0;
				fRestat = 1;
			}
		}

	}
	if (fRestat)
	{
		Make8Graph();
	}
	else
		adj.SetLen(l);

}
//--------------------------------------------------------------
//
//--------------------------------------------------------------
void MakePolyLine::MakeLines(int N, MArray<bool>* a_ord, int fFirstAdd)
{

	pindex.Clear();
	MFifo<int> stack;
	MArray<bool> _ord(arr.GetLen(), false);
	if (!a_ord)
		a_ord = &_ord;
	pindex.SetLen(arr.GetLen());
	bool* ord = a_ord->GetPtr();
	GNode* aFirst = adj.GetPtr();
	for (int i = N, l = arr.GetLen(); i < l; ++i)
	{
		if (!ord[i])
		{
			ord[i] = 1;
			stack.Push(i);
			MArray<int>* ind;
			if (fFirstAdd < 0)
				ind = &index.Add();
			else
				ind = &index[fFirstAdd];

			while (!stack.IsEmpty())
			{
				int j = stack.Pop();
				GNode* node = aFirst + adj[j].pw;

				if (fFirstAdd < 0)
				{
					ind->Add(j);
					fFirstAdd = -1;
				}
				pindex[j].Set(ind->GetLen() - 1, index.GetLen() - 1);
				switch (adj[j].num)
				{
				case 1:
					if (!ord[node[0].pw])
					{
						stack.Push(node[0].pw);
						ord[node[0].pw] = 1;
					}
					break;
				case 2:
				{
					for (int k = 0, lk = adj[j].num; k < lk; ++k)
						if (!ord[node[k].pw])
						{
							stack.Push(node[k].pw);
							ord[node[k].pw] = 1;
							break;
						}
					break;
				}
				case 3:
				{
					GNode* pnode[3] = { 0, 0, 0 };
					int n1[3] = { -1, -1, -1 };
					int n = 0;
					for (int k = 0, lk = adj[j].num; k < lk; ++k)
						if (!ord[node[k].pw])
						{
							pnode[n] = node + k;
							++n;
						}

					if (n == 2)
					{
						GNode* node1 = aFirst + pnode[0]->pw;
						GNode* node2 = aFirst + pnode[1]->pw;

						n = 0;
						if (node2->num == 2)
						{
							n = 1;
							GNode* node3 = node1;
							node1 = node2;
							node2 = node3;
						}
						if (node1->num == 2)
						{
							GNode* node3 = aFirst + node1->pw;
							if ((node3[0].pw == node2->pw || node3[1].pw == j) ||
								(node3[1].pw == node2->pw || node3[0].pw == j))
							{
								ord[pnode[n]->pw] = 1;
								ord[pnode[!n]->pw] = 1;
								stack.Push(node[!n].pw);
							}

						}
					}
					break;
				}
				default:
					for (int k = 0, lk = adj[j].num; k < lk; ++k)
						if (!ord[node[k].pw])
						{
							stack.Push(node[k].pw);
							ord[node[k].pw] = 1;
							break;
						}

				}
			}
		}
	}
	//MakeLines_Step2();

}
//--------------------------------------------------------------
//
//--------------------------------------------------------------
void MakePolyLine::InitEdgeGraph(MArray<Vector2F>& arr_tmp, MArray<int>& pos, bool fInitMinus, bool fFullGraph)
{
	MHashMap<int, int> edge;
	MHashMap<int, int> edge_inv;
	MArray<GNode> edge_adj;
	MArray<Vector2F> tmp;

	arr.Clear();
	pindex.Clear();
	index.Clear();
	adj.Clear();
	tmp.SetLen(pos.GetLen() * 2);
	int i, len, k = 0;
	RBSet<Vector2F> test;
	Vector2F* at = arr_tmp.GetPtr();
	for (i = 0, len = pos.GetLen() - 1; i < len; ++i)
	{
		edge[pos[i]] = k;
		edge_inv[k] = pos[i];
		tmp[k++] = at[pos[i]];
		test.Add(at[pos[i]]);
		if (at[pos[i + 1] - 1] != at[pos[i]])
		{
			edge_inv[k] = pos[i + 1] - 1;
			edge[pos[i + 1] - 1] = k;
			tmp[k++] = at[pos[i + 1] - 1];
			test.Add(at[pos[i + 1] - 1]);
		}
	}
	tmp.SetLen(k);
	if (fInitMinus)
	{
		MArray<Vector2F> minus;

		minus.SetLen(arr_tmp.GetLen());
		Vector2F* mt = minus.GetPtr();

		for (k = i = 0, len = arr_tmp.GetLen(); i < len; ++i)
		{
			if (!test.Find(at[i]))
			{
				mt[k] = at[i];
				test.Add(at[i]);
				++k;
			}

		}
		minus.SetLen(k);
		BuilderVoron2D vor;
		vor.BuildGabriel(tmp, minus, X, Y, edge_adj);
	}
	else
	{
		MArray<Vector2F> minus;
		BuilderVoron2D vor;
		vor.Build(tmp, minus, X, Y, edge_adj);
	}
	arr.Swap(arr_tmp);
	pindex.SetLen(arr.GetLen());
	len = pos.GetLen() - 1;
	index.SetLen(len);
	adj.SetLen(arr.GetLen());
	adj.SetAddedRealSize(1024);
	IPoint* ppi = pindex.GetPtr();
	k = 0;
	GNode* new_n = edge_adj.GetPtr();
	int l;
	for (i = 0; i < len; ++i)
	{
		index[i].SetLen(l = (pos[i + 1] - pos[i]));
		int* ip = index[i].GetPtr();
		for (int j = 0; j < l; ++j, ++k)
		{
			ip[j] = k;
			ppi[k].Set(j, i);
			if (!j || j == l - 1)
			{
				int p = edge[k];
				GNode* node_n = new_n + new_n[p].pw;
				int ps = adj.GetLen();
				adj[k].Set(1 + new_n[p].num, ps);
				adj.SetLen(ps + adj[k].num);
				GNode* node = adj.GetPtr() + ps;
				if (fFullGraph)
				{
					node->Set(0, !j ? k + 1 : k - 1);
					++node;
				}
				else
				{
					node->Set(0, !j ? pos[i + 1] - 1 : pos[i]);
					++node;
					//--adj[k].num;
				}
				for (int n = 0; n < new_n[p].num; ++n)
				{
					int ps = edge_inv[node_n[n].pw];
					if ((ps == pos[i + 1] - 1) || ps == pos[i])
					{
						--adj[k].num;
						continue;
					}
					node->Set(node_n[n].pwt, ps);
					++node;
				}
			}
			else
			{
				if (fFullGraph)
				{
					adj[k].Set(2, adj.GetLen());
					adj.Add().Set(0, k - 1);
					adj.Add().Set(0, k + 1);
				}
				else
					adj[k].Set(0, 0);

			}
		}
	}
}
