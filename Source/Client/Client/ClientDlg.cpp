﻿// ClientDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Client.h"
#include "ClientDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CClientDlg dialog


CClientDlg::CClientDlg(CWnd* pParent /*=NULL*/)
: CDialog(CClientDlg::IDD, pParent)
, m_msgString(_T(""))
, IP(_T(""))
,question(_T(""))
,answer(_T(""))
, m_userName(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_MSGBOX, m_msgString);
	DDX_Text(pDX, IDC_IP, IP);
	DDX_Text(pDX, IDC_USER, m_userName);
	DDX_Text(pDX, IDC_QUESTION, question);
	DDX_Text(pDX, IDC_ANSWER, answer);
}

BEGIN_MESSAGE_MAP(CClientDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE (WM_SOCKET, SockMsg)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_LOGIN, &CClientDlg::OnBnClickedLogin)
	ON_BN_CLICKED(IDC_LOGOUT, &CClientDlg::OnBnClickedLogout)
	ON_BN_CLICKED(IDC_START, &CClientDlg::OnBnClickedStart)
	ON_EN_CHANGE(IDC_QUESTION, &CClientDlg::OnEnChangeQuestion)
	ON_EN_CHANGE(IDC_ANSWER, &CClientDlg::OnEnChangeAnswer)
	ON_BN_CLICKED(IDC_SEND, &CClientDlg::OnBnClickedSend)
END_MESSAGE_MAP()


// CClientDlg message handlers

BOOL CClientDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	IP="127.0.0.1";
	GetDlgItem(IDC_LOGOUT)->EnableWindow(FALSE);
	GetDlgItem(IDC_SEND)->EnableWindow(FALSE);
	GetDlgItem(IDC_START)->EnableWindow(FALSE);
	UpdateData(FALSE);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CClientDlg::Split(CString src,CString des[2])
{
	int p1,p2;

	p1=src.Find(_T("\r\n"),0);
	des[0]=src.Mid(0,p1);

	p2=src.Find(_T("\r\n"),p1+1);
	des[1]=src.Mid(p1+2,p2-(p1+2));

}

char* CClientDlg::ConvertToChar(const CString &s)
{
	int nSize = s.GetLength();
	char *pAnsiString = new char[nSize+1];
	memset(pAnsiString,0,nSize+1);
	wcstombs(pAnsiString, s, nSize+1);
	return pAnsiString;
}

void CClientDlg::mSend(CString Command)
{
	int Len=Command.GetLength();
	Len+=Len;
	PBYTE sendBuff = new BYTE[1000];
	memset(sendBuff,0,1000);
	memcpy(sendBuff,(PBYTE)(LPCTSTR)Command, Len);
	send(sClient,(char*)&Len,sizeof(Len),0);
	send(sClient,(char*)sendBuff,Len,0);
	delete sendBuff;
}

int CClientDlg::mRecv(CString &Command)
{
	PBYTE buffer = new BYTE[1000];
	memset(buffer,0, 1000);
	recv(sClient ,(char*)&buffLength,sizeof(int),0);
	recv(sClient,(char*)buffer,buffLength,0);
	TCHAR* ttc = (TCHAR*)buffer;
	Command = ttc;

	if(Command.GetLength()==0)
		return -1 ;
	return 0;
}

LRESULT CClientDlg::SockMsg(WPARAM wParam, LPARAM lParam)
{

	if (WSAGETSELECTERROR(lParam))
	{
		// Display the error and close the socket
		closesocket(wParam);
	}
	switch(WSAGETSELECTEVENT(lParam))
	{
	case FD_READ:
		{
			CString temp;
			if(mRecv(temp)<0)
				break;

			Split(temp,strResult);
			int flag1=_ttoi(strResult[0]);
			int flag2=_ttoi(strResult[1]);
			switch(flag1)
			{
			// login
			case -1:
				{
					// Nếu không bị trùng tên đã đăng ký (-1,-1)
					if(flag2==1)
					{
						m_msgString+=_T("Đăng kí thành công!!\r\n");
						GetDlgItem(IDC_LOGOUT)->EnableWindow(TRUE);
						GetDlgItem(IDC_LOGIN)->EnableWindow(FALSE);
						GetDlgItem(IDC_START)->EnableWindow(TRUE);
					}
					// Nếu không bị trùng (-1,-2)
					else
						m_msgString+=_T("Tên đã được sử dụng!!\r\n");
	
					UpdateData(FALSE);
					break;
				}
			//limit=3 => Trả lời sai quá ba lần
			case -4:
				{
					//UpdateData(TRUE);
					m_msgString += _T("\r\nBạn đã trả lời sai 3 lần liên tiếp, bạn phải dừng cuộc chơi!\r\n");
					m_msgString += _T("\r\nID của bạn sẽ bị xóa khỏi server!\r\n");

					UpdateData(FALSE);
					GetDlgItem(IDC_SEND)->EnableWindow(FALSE);
					GetDlgItem(IDC_LOGOUT)->EnableWindow(FALSE);
					break;
				}

				// hoàn thành lượt chơi
			case -5:
				{
					m_msgString += _T("\r\nBạn đã hoàn thành lượt chơi với tổng điểm là: \r\n");
					m_msgString += strResult[1]+ _T("\r\n");
					UpdateData(FALSE);
					GetDlgItem(IDC_SEND)->EnableWindow(FALSE);
					break;
				}
				// Số điểm nhỏ hơn 0
			case -6:
			{
				
				m_msgString += _T("\r\nSố điểm của bạn nhỏ hơn 0, bạn phải dừng cuộc chơi!\r\n");
				m_msgString += _T("\r\nID của bạn sẽ bị xóa khỏi server!\r\n");
				UpdateData(FALSE);
				GetDlgItem(IDC_SEND)->EnableWindow(FALSE);
				GetDlgItem(IDC_LOGOUT)->EnableWindow(FALSE);
				break;

			}
			default:
			{
				question = strResult[1];
				UpdateData(FALSE);
			}
			}
			break;
		}
	case FD_CLOSE:
		{
			closesocket(sClient);
			m_msgString+=_T("Server đã đóng kết nối\r\n");
			GetDlgItem(IDC_LOGOUT)->EnableWindow(FALSE);
			GetDlgItem(IDC_LOGIN)->EnableWindow(TRUE);
			UpdateData(FALSE);
			break;
		}

	}
	return 0;
}

void CClientDlg::OnBnClickedLogin()
{
	// TODO: Add your control notification handler code here

	UpdateData(TRUE);
	if(m_userName=="")
	{
		MessageBox(_T("Vui lòng nhập user name!"));
		return;
	}
	if (m_userName.GetLength() > 8)
	{
		MessageBox(_T("Tên đăng nhập không vượt quá 8 ký tự!"));
		return;
	}

	sClient = socket(AF_INET, SOCK_STREAM, 0);
	hostent* host=NULL;
	if (sClient == INVALID_SOCKET)
	{
		MessageBox(_T("socket() failed"),_T("ERROR"), 0);
		return;
	}

	servAdd.sin_family = AF_INET;
	servAdd.sin_port = htons(PORT);

	char* cIP=ConvertToChar(IP);

	servAdd.sin_addr.s_addr = inet_addr(cIP);

	CStringA cpy_IP(IP); 

	if (servAdd.sin_addr.s_addr==INADDR_NONE)
	{
		host = (gethostbyname(cpy_IP));
		if (host == NULL)
		{
			MessageBox(_T("Không thể kết nối đến Server!"),_T("ERROR"), 0);
		}
		CopyMemory(&servAdd.sin_addr, host->h_addr_list[0],
			host->h_length);
		return;
	}

	int err=connect(sClient,(struct sockaddr*)&servAdd,sizeof(servAdd));
	if (err==SOCKET_ERROR){
		MessageBox(_T("Kết nối thất bại"),_T("ERROR"), 0);
		return;
	}

	Command=_T("-1\r\n");
	Command+= m_userName+_T("\r\n");

	mSend(Command);

	WSAAsyncSelect(sClient,m_hWnd,WM_SOCKET,FD_READ|FD_CLOSE);
	GetDlgItem(IDC_LOGOUT)->EnableWindow(TRUE);
	GetDlgItem(IDC_SEND)->EnableWindow(FALSE);
	GetDlgItem(IDC_START)->EnableWindow(TRUE);
	UpdateData(FALSE);
}

void CClientDlg::OnBnClickedLogout()
{
	// TODO: Add your control notification handler code here
	INT_PTR i = MessageBox(_T("Bạn muốn logout?"),_T("Confirm"), MB_OKCANCEL);
	if (i == IDCANCEL)
		return;
	Command=_T("-2\r\n\r\n");
	GetDlgItem(IDC_LOGIN)->EnableWindow(TRUE);
	GetDlgItem(IDC_START)->EnableWindow(FALSE);
	GetDlgItem(IDC_SEND)->EnableWindow(FALSE);
	mSend(Command);
}


void CClientDlg::OnBnClickedStart()
{
	// TODO: Add your control notification handler code here
	Command = "-3\r\n\r\n";
	GetDlgItem(IDC_START)->EnableWindow(FALSE);
	GetDlgItem(IDC_LOGIN)->EnableWindow(FALSE);
	GetDlgItem(IDC_LOGOUT)->EnableWindow(TRUE);
	GetDlgItem(IDC_SEND)->EnableWindow(TRUE);
	mSend(Command);
}


void CClientDlg::OnEnChangeQuestion()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}


void CClientDlg::OnEnChangeAnswer()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}


void CClientDlg::OnBnClickedSend()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	Command = strResult[0] + _T("\r\n") + answer + _T("\r\n");
	mSend(Command);
	answer = _T("");
	question = _T("");
	UpdateData(FALSE);
}
