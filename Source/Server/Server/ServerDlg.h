// ServerDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "stdlib.h"
#include "time.h"
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <cstring>

using namespace std;

#define PORT 25000
#define WM_SOCKET WM_USER+1

// CServerDlg dialog
class CServerDlg : public CDialog
{
	// Construction
public:
	CServerDlg(CWnd* pParent = NULL);	// standard constructor

	// Dialog Data
	enum { IDD = IDD_SERVER_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


	// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	LRESULT SockMsg(WPARAM wParam, LPARAM lParam);
	char* ConvertToChar(const CString &s);
	void Split(CString src,CString des[2]);
	void mSend(SOCKET sk, CString Command);
	int mRecv(SOCKET sk, CString &Command);

	vector<CString> question;
	vector<CString> answer;
	

	struct Player
	{

		SOCKET sockClient; 
		char Name[200]; // tên sockCLient
		int scores = 3; // Số điểm của sockClient
		int limit = 0;  // Sử dụng để kiểm tra client trả lời sai 3 câu liên tiếp hay không
		int limitQuestion = 1; // Số câu hỏi đã trả lời
	};

	SOCKET sockServer,sockClient,flag,sclient;
	struct sockaddr_in serverAdd;
	int buffLength;
	int number_Socket;
	Player *pSock;
	CString strResult[2];
	CString Command;
	int R;


	CString m_msgString;
	afx_msg void OnBnClickedListen();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnEnChangeHistory();
};
