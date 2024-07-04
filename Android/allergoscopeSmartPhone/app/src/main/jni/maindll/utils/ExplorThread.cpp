#include "stdafx.h"

#ifndef _CONSOLE_PROG_

#include "ExplorThread.h"


ExplorThread::ExplorThread():MSGThread(0)
{
    fPause = 0;
    fError = 0;
    dirMess =  diskMess = 0;
    dir_hWnd = disk_hWnd = 0;

    m_hHandleNotifyRereadDir = CreateEvent();
    m_hChangeNotification = CreateEvent();
    m_hHandleNotifyAddNewHandle = CreateEvent();

}
DWORD _GetDriveType(TString m_sPath)
{
    char tmp[4];
    memcpy(tmp, m_sPath, 4 * sizeof(char));
    tmp[3] = 0;
    return ::GetDriveType(tmp);
}

ExplorThread::~ExplorThread()
{
    Stop();

}
bool ExplorThread::Stop()
{
    m_Global.criticalAddHandle.Enter();
    SendMessage(WM_DESTROY);
    Sleep(100);
    m_Global.criticalAddHandle.Leave();
    m_Global.m_MesMap.Clear();
    m_Global.m_NameMap.Clear();
    return 1;

}

void ExplorThread::SetMessage(MVector<TString> &path, BOOL fExpand, HWND hWnd, DWORD dwMessage, WPARAM wParam, LPARAM lParam, DWORD dwFlags)
{
    if (fPause) return;

    m_Global.criticalAddHandle.Enter();
    m_Global.data.path = path;
    m_Global.data.fExpand = fExpand;
    m_Global.data.hWnd = hWnd;
    m_Global.data.dwMessage = dwMessage;
    m_Global.data.wParam = wParam;
    m_Global.data.lParam = lParam;
    m_Global.data.dwFlags = dwFlags;
    m_Global.criticalAddHandle.Leave();
    AfxGetApp()->fBlocking = TRUE;
    PostMessage(WM_USER + 1111, 0, 0);
    if (!WaitForSingleObject(m_hHandleNotifyAddNewHandle, 1500))
    {
        SetEvent(m_hHandleNotifyRereadDir);
        fError = 1;
    }
    AfxGetApp()->fBlocking = FALSE;

}
BOOL ExplorThread::Start()
{
    return th.Start(ThreadFunc, this, FALSE);
}
void ExplorThread::Play()
{
    if (fPause)
    {
        th.Resume();
        RBMap<int, TString>::iterator it(m_Selected);
        for (RBData<int, TString> *p = it.begin(); p; p = it.next())
            ::PostMessage(disk_hWnd, diskMess, p->key[0], 0);
        m_Selected.Clear();
        if (m_Path.StrLen())
        {
                ::PostMessage(dir_hWnd, dirMess, m_Path[0], 0);
            m_Path = "";
        }
    }
    fPause = 0;
}
void ExplorThread::Pause()
{
    if (!fPause)
    {
        MVector<TString> path;
        RBMap<int, TString>::iterator it(m_Selected);
        for (RBData<int, TString> *p = it.begin(); p; p = it.next())
            path.Add() =  p->key;
        if (path.GetLen())
            SetMessage(path, 0, disk_hWnd, diskMess, 0, 0, FILE_NOTIFY_CHANGE_SIZE);
        path.Clear();
        if (m_Path.StrLen())
        {
            path.Add() = m_Path;
            SetMessage(path, 0, dir_hWnd, dirMess, 0, 0, FILE_NOTIFY_CHANGE_SIZE);
        }

        th.Suspend();
    }
    fPause = 1;
}
void ExplorThread::AddDiskPath(HWND hWnd, DWORD mes)
{

    MVector<TString> arr;
    arr.Add() = _PATH_MOUNTED;
    arr.Add() = "/proc/mounts";
    arr.Add() = "/dev";
    SetDiskPath(hWnd, mes, arr);
}
void ExplorThread::SetDiskPath(HWND hWnd, DWORD mes, MVector<TString> &arr)
{

    RBMap<int, TString> dat;
    DWORD type;
    for (int i = 0; i < arr.GetLen(); ++i)
    {
        TString &m = arr[i];
        type = _GetDriveType(m);
        if ((type == DRIVE_FIXED || type == DRIVE_REMOVABLE))
            dat[m];
    }
    diskMess = mes;
    disk_hWnd = hWnd;
    MVector<TString> path;
    RBMap<int, TString>::iterator it(m_Selected);
    RBData<int, TString> *p;
    for (p = it.begin(); p; p = it.next())
        if (!dat.Lookup(p->key))
            path.Add() =  p->key;

    if (path.GetLen())
    {
        SetMessage(path, 0, hWnd, mes, 0, 0, FILE_NOTIFY_CHANGE_LAST_ACCESS);
        Sleep(10);
        int num = path.GetLen();
        for (int j = 0; j < num; ++j)
            m_Selected.Delete(path[j]);

    }
    path.Clear();
    RBMap<int, TString>::iterator it1(dat);
    for (p = it1.begin(); p; p = it1.next())
    {
        if (!m_Selected.Lookup(p->key) && p->key != m_Path)
            path.Add() = p->key;
        m_Selected[p->key] = p->value;

    }
    if (path.GetLen())
    {
        SetMessage(path, 1, hWnd, mes, 0, 1, FILE_NOTIFY_CHANGE_DIR_NAME);
        Sleep(0);
    }


}

void ExplorThread::SetPath(const TString &str, HWND hWnd, DWORD mes)
{
    if (fPause) return;
    MVector<TString> path;
     dirMess = mes;
    dir_hWnd = hWnd;

    if (m_Path != str)
    {
        if (m_Path.StrLen())
        {
            path.Add() = m_Path;
            SetMessage(path, 0, hWnd, mes, 0, 0, FILE_NOTIFY_CHANGE_SIZE);
            path.Clear();
        }
        m_Path = str;
    if 	(m_Path.StrLen())
    {
        path.Add() = m_Path;
        SetMessage(path, 1, hWnd, mes, 0, 0, FILE_NOTIFY_CHANGE_SIZE);
    }

    }
}
BOOL WINAPI ExplorThread::DeleteHandle(TString path, HANDLE handle,  GlobalData &global)
{
    MArray<HANDLE > h;
    if (global.m_NameMap.Lookup(path, h))
    {
        int i, k = -1;
        int nh = h.GetLen();
        for (i = 0; i < nh; ++i)
            if (h[i] == handle)
            {
                k = i;
                break;
            }
        if (k == -1) return 0;

        global.m_MesMap.Delete(handle);
        FindCloseChangeNotification(handle);
        if (nh == 1)
            global.m_NameMap.Delete(path);
        else
            global.m_NameMap[path].Remove(k, 1);
        return 1;
    }
    return 0;
}
DWORD WINAPI ExplorThread::CheckData(MArray<MSGTYPE> &handles, GlobalData &global)
{
    DWORD fRet = 1;
    if (!global.criticalAddHandle.TryEnter()) return 0;
    try {
    int fRes, num = global.data.path.GetLen();
    for (int j = 0; j < num; ++j)
        if (global.data.fExpand)
        {
            MArray<HANDLE> h;
            if (fRes = global.m_NameMap.Lookup(global.data.path[j], h))
            {
                fRes = 0;
                for (int i = 0; i < h.GetLen(); ++i)
                    if (global.m_MesMap[h[i]] == *(Message *)&global.data)
                    {
                        fRes = 1;
                        break;
                    }
            }
            if (!fRes)
            {
                HANDLE h1 = FindFirstChangeNotification(global.data.path[j], (BOOL)global.data.lParam, IN_CLOSE_WRITE |  IN_MOVE | IN_CREATE	| IN_DELETE);
                if (h1 != INVALID_HANDLE_VALUE)
                {
                    handles.Add() = h1;
                    global.m_MesMap[h1].Set(&global.data);
                    global.m_MesMap[h1].filename = global.data.path[j];
                    global.m_MesMap[h1].wParam = global.data.path[j][0];
                    global.m_NameMap[global.data.path[j]].Add() = h1;

                }
            }
        }
        else
        {
            MArray<HANDLE > h;
            if (global.m_NameMap.Lookup(global.data.path[j], h))
            {
                int i, n, k = -1;
                int nh = h.GetLen();
                for (i = 0; i < nh; ++i)
                    if (global.m_MesMap[h[i]] == *(Message *)&global.data)
                    {
                        k = i;
                        break;
                    }
                if (k != -1)
                {
                    n = handles.GetLen();
                    for (i = 0; i < n; ++i)
                        if (handles[i] == h[k])
                        {
                            handles.Remove(i, 1);
                            break;
                        }
                    if (i < n)
                    {
                        global.m_MesMap.Delete(h[k]);
                        FindCloseChangeNotification(h[k]);
                    }
                    if (nh == 1)
                        global.m_NameMap.Delete(global.data.path[j]);
                    else
                        global.m_NameMap[global.data.path[j]].Remove(k, 1);
                }
            }
            else
                fRet = 0;


    }
    }
    catch(...)
    {
        global.criticalAddHandle.Leave();
        return 0;
    }
    global.criticalAddHandle.Leave();
    return fRet;

}

void *ExplorThread::ThreadFunc(void *h)
{
//	SendMessageTimeout

    ExplorThread &self = *(ExplorThread *)h;
    self.InitMsg();
    GlobalData &global = self.m_Global;
    MArray<MSGTYPE> &handles = self.handles;
    handles.SetLen(3);
    handles[0] = (MSGTYPE)WM_DESTROY;
    handles[1] = (MSGTYPE)self.m_hHandleNotifyRereadDir;
    handles[2] = (MSGTYPE)(WM_USER + 1111);

    DWORD dwRes;
    while (true)
    {
        dwRes = WaitForMultipleObjects(handles.GetLen(), handles.GetPtr(), FALSE, INFINITE);
                if (dwRes == WAIT_FAILED)
                    continue;
        if (dwRes == WAIT_OBJECT_0 + 1)
        {
            dwRes = GetLastError();
            SetLastError(0);

            for (int i = 0; i < handles.GetLen(); ++i)
            {
                Message mess;
                if (global.m_MesMap.Lookup(handles[i], mess))
                {
                    int n;
                    TString s = mess.filename;
                    if ((n = mess.filename.StrLen()) > 1 && s[n-1] == FD)
                        s = s.Left(n - 1);
                    if (GetFileAttributes(s) == (DWORD)-1)
                        goto label_del;
                    if (!DeleteHandle(mess.filename, handles[i], global))
                        goto label_del;
                    HANDLE h1 = FindFirstChangeNotification(mess.filename, (BOOL)mess.lParam, IN_CLOSE_WRITE |  IN_MOVE | IN_CREATE	| IN_DELETE); //global.data.dwFlags);
                    if (h1 != INVALID_HANDLE_VALUE)
                    {
                        handles[i] = h1;
                        global.m_MesMap[h1] = mess;
                        global.m_NameMap[mess.filename].Add() = h1;
                    }
                    else
                        goto label_del;


                }
                continue;
            label_del:
                handles.Remove(i, 1);
                --i;
            }
            SetLastError(0);

        }
        else
        if (dwRes == WAIT_OBJECT_0)
        {
            break;
        }
        else
        if (dwRes == WAIT_OBJECT_0 + 2)
        {
            TMSG msg ;
            msg.message = WM_USER + 1111;
            if (msg.message == WM_USER + 1111)
            {
                if (dwRes = CheckData(handles, global))
                ::PostMessage(AfxGetMainThread(), self.m_hHandleNotifyAddNewHandle, 0, 0);

            }
        }

        else
        if (dwRes != (DWORD)handles.GetLen())
        {
            dwRes -= WAIT_OBJECT_0;
            if (dwRes < (DWORD)handles.GetLen())
            {
                Message *mess = 0;
                if (global.m_MesMap.Lookup(handles[dwRes], &mess))
                {
                    bool del = 0;
                    DWORD dw;
                    int n;
                    int iSend = 0;
                    TString s = mess->filename;
                    do
                    {
                        if (GetFileAttributes(s) == (DWORD)-1)
                        {
                            del = 1;
                            break;
                        }
                        if (!FindNextChangeNotification(handles[dwRes]))
                        {
                            dw = WAIT_TIMEOUT;
                            break;
                        }
                        else
                            ++iSend;
                    }
                    while ((dw = WaitForMultipleObjects(1, handles.GetPtr() + dwRes, FALSE, 150)) == WAIT_OBJECT_0);
                    if (!del)
                        del = dw != WAIT_TIMEOUT;
                    if (iSend && IsWindow(mess->hWnd))
                    ::PostMessage(mess->hWnd, AfxGetMainThread(), mess->dwMessage, mess->wParam, 0);
                    if (del)
                    {
                        s = mess->filename;
                        DeleteHandle(s, handles[dwRes], global);
                    //	FindCloseChangeNotification(handles[dwRes]);
                        handles.Remove(dwRes, 1);
                    }
                }
            }


        }

    }
    return 0;
}
#endif
