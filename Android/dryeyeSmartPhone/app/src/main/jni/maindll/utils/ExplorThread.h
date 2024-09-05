#ifndef __ExplorThread__H
#define __ExplorThread__H
#ifndef ANDROID_NDK
#pragma pack(push) //NADA
#pragma pack(4)
EXPORTNIX class ExplorThread : public MSGThread {
    struct Message {
        TString filename;
        HWND hWnd;
        DWORD dwMessage;
        WPARAM wParam;
        LPARAM lParam;
        DWORD dwFlags;

        Message()
        {}

        ~Message()
        {}

        Message(Message &m)
        { *this = m; }

        Message &operator=(Message &mes)
        {
            hWnd = mes.hWnd;
            dwMessage = mes.dwMessage;
            wParam = mes.wParam;
            lParam = mes.lParam;
            dwFlags = mes.dwFlags;
            filename = mes.filename;
            return *this;
        }

        bool operator==(Message &m)
        {
            return hWnd == m.hWnd &&
                   dwMessage == m.dwMessage && dwFlags == m.dwFlags;

        }

        void Set(Message *mes)
        {
            hWnd = mes->hWnd;
            dwMessage = mes->dwMessage;
            wParam = mes->wParam;
            lParam = mes->lParam;
            dwFlags = mes->dwFlags;
        };
    };

    struct MesData : public Message {
        MVector <TString> path;
        BOOL fExpand;

        MesData()
        {}

        ~MesData()
        {}
    };

public:
    ExplorThread();

    virtual ~ExplorThread();

    BOOL Start();

    void SetPath(const TString &str, HWND hWnd, DWORD mes);

    void AddDiskPath(HWND hWnd, DWORD mes);

    void Play();

    void Pause();

    BOOL LastError(BOOL f = -1)
    {
        BOOL f1 = fError;
        if (f != -1) fError = f;
        return f1;
    }

    virtual bool Stop();

protected:
    void SetDiskPath(HWND hWnd, DWORD mes, MVector <TString> &arr);

    void
    SetMessage(MVector <TString> &path, BOOL fExpand, HWND hWnd, DWORD dwMessage, WPARAM wParam,
               LPARAM lParam, DWORD dwFlags);

protected:

    BOOL fError;
    MSGTYPE m_hHandleNotifyRereadDir;
    MSGTYPE m_hChangeNotification;
    MSGTYPE m_hHandleNotifyAddNewHandle;
    MArray <MSGTYPE> handles;
    RBMap<int, TString> m_Selected;
    TString m_Path;
    BOOL fPause;
    UINT diskMess, dirMess;
    HWND dir_hWnd, disk_hWnd;
    MPThread th;
protected:
    struct GlobalData {
        RBMap <MArray<HANDLE>, TString> m_NameMap;
        RBMap <Message, HANDLE> m_MesMap;
        MesData data;
        CriticalSection criticalAddHandle;

        int fException;
    } m_Global;
public:
    static void *ThreadFunc(void *wnd);

    static DWORD WINAPI
    CheckData(MArray<MSGTYPE>
    &handles,
    GlobalData &global
    );
    static BOOL WINAPI
    DeleteHandle(TString
    path,
    HANDLE handle, GlobalData
    &global);

};
#pragma pack(pop) //NADA

#endif
#endif
