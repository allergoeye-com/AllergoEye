#pragma once
#include "resource.h"

// TimerAndView dialog
#define USER_MSG_ACCEPT                 101
#define USER_CLIENT_CLOSE               102
#define USER_MSG_READ                   103
#define USER_MSG_TRAY                   104
#define USER_WAIT_ROI                   105
#define USER_ADD_BIN                   106
#define USER_ADD_TRACE                   107
#define USER_RETRANSLATE				108

#define USER_ON_ACCEPT (WM_USER + USER_MSG_ACCEPT)
#define USER_ON_CLIENT_CLOSE (WM_USER + USER_CLIENT_CLOSE)
#define USER_ON_READ (WM_USER + USER_MSG_READ)
#define USER_ON_TRAY (WM_USER + USER_MSG_TRAY)
#define USER_ON_WAIT_ROI (WM_USER + USER_WAIT_ROI)
#define USER_ON_ADD_BIN (WM_USER + USER_ADD_BIN)
#define USER_ON_ADD_TRACE (WM_USER + USER_ADD_TRACE)
#define USER_ON_RETRANSLATE (WM_USER + USER_RETRANSLATE)
#include "afxdtctl.h"
#include "MDNS.h"
#include "MPThread.h"
#include "afxwin.h"
#include "NetServerThread.h"
#include "NetDir.h"
#include "MDNSReader.h"
class TimerAndView;
struct MonitorData {
	HANDLE eventExit;
	TimerAndView *hWnd;
};

class TimerAndView : public CDialog
{
	DECLARE_DYNAMIC(TimerAndView)
public:
	TimerAndView(CWnd* pParent = 0);   // standard constructor
	virtual ~TimerAndView();
void ClosePort();
void OpenPort();

// Dialog Data
	enum { IDD = IDD_TimerAndView };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg LRESULT OnUserOnClientClose(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUserOnAccept(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUserOnTray(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUserWaitRoi(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUserAddBin(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUserTrace(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUserRetranslate(WPARAM wParam, LPARAM lParam);

	afx_msg void OnBnClickedOnpath();

	virtual BOOL OnInitDialog();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	CEdit m_OnHost;
	CEdit m_OnError;
	CEdit m_EditPath;
	CEdit m_EditPort;
	MThread m_Host;
	MThread m_Dir;
	HostData m_HostData;
	MDNS dns;
	MArray<NetServerThread *> client;
	MVector<WString> msg;
	void Minimize();
	NetDir::TreeDir dir_changes;
	MonitorData monitorData;
	afx_msg void OnDestroy();
//	afx_msg void OnClose();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	int dir_changes_tic_coint;
	MDNSReader *reader;
	RBSet<TString> names;
	PROCESS_INFORMATION pi;

	
	afx_msg void OnBnClickedClose();
};
