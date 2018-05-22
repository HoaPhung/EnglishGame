// ServerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Server.h"
#include "ServerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CServerDlg dialog




CServerDlg::CServerDlg(CWnd* pParent) : CDialog(CServerDlg::IDD, pParent), m_msgString(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME); // load giao diện
}

void CServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_BOXCHAT, m_msgString); //https://msdn.microsoft.com/en-us/library/a1xttfdt.aspx
}

BEGIN_MESSAGE_MAP(CServerDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_SOCKET, SockMsg)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_LISTEN, &CServerDlg::OnBnClickedListen)
	ON_BN_CLICKED(IDC_CANCEL, &CServerDlg::OnBnClickedCancel)
	ON_EN_CHANGE(IDC_HISTORY, &CServerDlg::OnEnChangeHistory)
END_MESSAGE_MAP()


// CServerDlg message handlers

BOOL CServerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CServerDlg::OnPaint()
{
	if (IsIconic()) //determines whether the specified window is minimized (iconic).
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
HCURSOR CServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CServerDlg::Split(CString src, CString des[2])
{
	int p1, p2;

	p1 = src.Find(_T("\r\n"), 0); // tìm sự xuất hiện của chuỗi, bắt đầu từ 0
	des[0] = src.Mid(0, p1);

	p2 = src.Find(_T("\r\n"), p1 + 1);
	des[1] = src.Mid(p1 + 2, p2 - (p1 + 2)); // trả về chuỗi con bắt đầu từ p1+2 và có độ dài p2-(p1+2)

}

char* CServerDlg::ConvertToChar(const CString &s)// convert CString to char
{
	int nSize = s.GetLength();
	char *pAnsiString = new char[nSize + 1];
	memset(pAnsiString, 0, nSize + 1);
	wcstombs(pAnsiString, s, nSize + 1);
	return pAnsiString;
}

void CServerDlg::mSend(SOCKET sk, CString Command) // Hàm gửi dữ liệu thông qua socket được kết nối
{
	int Len = Command.GetLength(); // chiều dài của command
	Len += Len;	// Len=Len+Len;
	PBYTE sendBuff = new BYTE[1000];
	memset(sendBuff, 0, 1000); // set tất cả cá byte gửi đi có giá trị là 0
	memcpy(sendBuff, (PBYTE)(LPCTSTR)Command, Len); // copy cái command lưu vào cái sendBuff
	send(sk, (char*)&Len, sizeof(Len), 0); // Gửi len thông qua sk được kết nối
	send(sk, (char*)sendBuff, Len, 0);	// Gửi sendBuff
	delete sendBuff;
}

int CServerDlg::mRecv(SOCKET sk, CString &Command) // Nhận thông tin qua socket được kết nối
{
	PBYTE buffer = new BYTE[1000];
	memset(buffer, 0, 1000);
	recv(sk, (char*)&buffLength, sizeof(int), 0);
	recv(sk, (char*)buffer, buffLength, 0);
	TCHAR* ttc = (TCHAR*)buffer;
	Command = ttc;

	if (Command.GetLength() == 0)
		return -1;
	return 0;
	// Nếu nhận được return 0
	// Không nhận được return -1
}

void CServerDlg::OnBnClickedListen() // Khi nhấn nút "Listen"
{
	// TODO: Add your control notification handler code here

	UpdateData();

	// chuẩn bị dữ liệu
	ifstream myfile;
	string line;

	myfile.open("question.txt");
	if (!myfile.is_open()) {
		cout << "Unable to open file";
	}
	else {
		while (!myfile.eof()) {
			getline(myfile, line);
			CString temp(line.c_str());
			question.push_back(temp);
		}
	}
	myfile.close();

	ifstream _myfile;
	string _line;

	_myfile.open("answer.txt");
	if (!_myfile.is_open()) {
		cout << "Unable to open file";
	}
	else {
		while (!_myfile.eof()) {
			getline(_myfile, _line);
			CString _temp(_line.c_str());
			answer.push_back(_temp);
		}
	}
	_myfile.close();

	// creates a socket
	// AF_INET :  The internet Protocol version 4(IPv4) address family
	// SOCK_STREAM: Tranmistion control protocol TCP
	// Đối số thứ 3 là Protocol
	// Kết quả trả về là 1 socket
	sockServer = socket(AF_INET, SOCK_STREAM, 0);

	// sockaddr_in are used with IPv4
	//addressfamily
	serverAdd.sin_family = AF_INET;
	// port
	serverAdd.sin_port = htons(PORT);
	serverAdd.sin_addr.s_addr = htonl(INADDR_ANY);

	// Liên kết địa chỉ cục bộ với socket
	//serverAdd: a pointer to a sockaddr_in struct of the local address to asign to the socket
	bind(sockServer, (SOCKADDR*)&serverAdd, sizeof(serverAdd));

	// server chờ nhận kết nối từ clients
	// 5: chiều dài dòng nhận kết nối
	listen(sockServer, 5);


	// Cơ chế lập trình non-blocking sử dụng WSAAsyncSelect
	// WSAAsyncSelect sử dụng cơ chế xử lý thông điệp của Windows.
	// Khi window kiểm tra thấy có một kết nối đến server sẽ gửi một thông điệp để báo cho server biết để gọi hàm accept mà không phải chờ đợi.
	// Tương tự khi có dữ liệu gửi đến chương trình sẽ báo cho ứng dụng gọi hàm recv và nhận ngay dữ liệu.
	//Socket, HWND, Msg, event
	// FD_READ: Báo hiệu có dữ liệu gửi đến cho socketm sẵn sàng để gọi rev
	// FD_ACCEPT: Báo hiệu cho socket server (đã gọi hàm listen) có một kết nối từ phía client
	// FD_CLOSE: Báo hiệu đầu còn lại của kết nối đã đóng socket
	// Trong WSAAsyncSelect thì wParam là socket có sự kiện phát sinh, lParam là hai byte thấp cho biết dự kiện phát sinh, hai byte cao cho biết lỗi phát sinh nếu có
	// Nguyên tắc khai báo và sử lý sự kiện: 
	//+ ví dụ cần bắt WM_SOCKET thì #define WM_SOCKET WM_USER+1, sau đó gọi hàm WSAAsyncSelect(socket,m_hWnd,WM_SOCKET,FD_ACCEPT|FD_READ| FD_CLOSE); 
	// Cách bắt và sử lý sự kiện: Xây dựng hàm sử lý sự kiện SockMsg (xuống xem SockMsg)
	int err = WSAAsyncSelect(sockServer, m_hWnd, WM_SOCKET, FD_READ | FD_ACCEPT | FD_CLOSE);
	if (err)
		MessageBox((LPCTSTR)"Cant call WSAAsyncSelect");
	GetDlgItem(IDC_LISTEN)->EnableWindow(FALSE);
	number_Socket = 0;
	pSock = new Player[200];

}

void CServerDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnCancel();
}

void CServerDlg::OnEnChangeHistory()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}

LRESULT CServerDlg::SockMsg(WPARAM wParam, LPARAM lParam)
{

	if (WSAGETSELECTERROR(lParam))
	{
		// Display the error and close the socket
		// Nếu có lỗi cảu ra thì xuất lỗi và đóng socket
		closesocket(wParam);
	}
	switch (WSAGETSELECTEVENT(lParam))
	{
	case FD_ACCEPT:
	{
		// Có một kết nối đến, gọi hàm accept để tiếp nhận
		pSock[number_Socket].sockClient = accept(wParam, NULL, NULL);
		GetDlgItem(IDC_LISTEN)->EnableWindow(FALSE);
		break;
	}
	case FD_CLOSE:
	{
		// Đầu bên kia đã đóng socket 
		UpdateData();
		int post = -1;
		for (int i = 0; i < number_Socket; i++)
		{
			if (pSock[i].sockClient == wParam)
			{
				if (i < number_Socket)
					post = i;
			}
		}

		m_msgString += pSock[post].Name;
		m_msgString += _T(" đã tự động ngắt chương trình!! ID bị xóa khỏi Server!!  \r\n");
		closesocket(wParam);
		for (int j = post; j < number_Socket; j++)
		{
			pSock[post].sockClient = pSock[post + 1].sockClient;
			strcpy(pSock[post].Name, pSock[post + 1].Name);
			pSock[post].limit = pSock[post + 1].limit;
			pSock[post].scores = pSock[post + 1].scores;
			pSock[post].limitQuestion = pSock[post + 1].limitQuestion;

		}
		number_Socket--;
		UpdateData(FALSE);
		break;
	}

	case FD_READ:
	{
		CString temp; // temp là biến nhận thông điệp
		if (mRecv(wParam, temp) < 0)
			break;


		int post = -1; // p là biến tìm xác định vị trí socket sinh sự kiện
		for (int i = 0; i < number_Socket; i++) {
			//wParam là socket có sự kiện phát sinh
			if (pSock[i].sockClient == wParam) {
				if (i < number_Socket)
					post = i;
			}
		}

		Split(temp, strResult); // chia thông điệp thành hai phần để xử lý
		int flag = _ttoi(strResult[0]); // chỉ số đầu
		char* tem = ConvertToChar(strResult[1]);

		// Xử lí đăng kí, xử lí logout
		if (flag < 0)
		{
			//login
			if (flag == -1)
			{
				// Tiến hành kiểm tra có bị trùng tên hay không
				int t = 0; //t=0: không trùng tên, t=1: trùng tên
				if (number_Socket > 0)
				{
					for (int i = 0; i < number_Socket; i++)
					{
						// hàm so sánh hai chuỗi, giống trả về 0
						if (strcmp(tem, pSock[i].Name) == 0) //Trung ten user{
						{
							t = 1;
							break;
						}
					}
				}
				// Nếu không trùng tên
				if (t == 0)
				{
					// Copy tem vào namesocket
					strcpy(pSock[number_Socket].Name, tem);
					Command = _T("-1\r\n1\r\n");
					m_msgString += strResult[1] + _T(" đã đăng kí!\r\n");
					UpdateData(FALSE);
					number_Socket++;
				}

				// Nếu trùng
				else
					Command = _T("-1\r\n2\r\n");
			}
			//logout
			else if (flag == -2)
			{
				int post = -1;
				for (int i = 0; i < number_Socket; i++)
				{
					if (pSock[i].sockClient == wParam)
					{
						if (i < number_Socket)
							post = i;
					}
				}
				m_msgString += pSock[post].Name;
				m_msgString += _T(" logout!!\r\n") ;
				closesocket(wParam);
				for (int j = post; j < number_Socket; j++)
				{
					pSock[post].sockClient = pSock[post + 1].sockClient;
					strcpy(pSock[post].Name, pSock[post + 1].Name);
				}

				number_Socket--;

				UpdateData(FALSE);
			}

			// start
			else if (flag == -3)
			{
				srand(time(NULL));
				int a = rand() % 50 + 1;
				CString temp;
				temp.Format(_T("%d"), a);
				Command = temp + _T("\r\n") + question[a] + _T("\r\n");
			}

		}
		// Phần flag>=0, xử lý các câu hỏi
		else
		{
			int post = -1; // Kiểm tra thuộc socket nào
			for (int i = 0; i < number_Socket; i++)
			{
				if (pSock[i].sockClient == wParam)
				{
					if (i < number_Socket)
						post = i;
				}
			}
			// Người chơi trả lời đúng xử lí: 
			// Nếu câu hỏi =5 thì đã trả lời đủ
			// Nếu chưa đủ 5 câu thì tiếp tục gởi
			if (strResult[1] == answer[flag])
			{
				pSock[post].scores++; // trả lời đúng cộng điểm
				pSock[post].limit = 0;
			//	pSock[post].limitQuestion++;
				m_msgString += pSock[post].Name;
				m_msgString += _T(" trả lời ĐÚNG! \r\n");
				//Trả lời đủ câu
				if (pSock[post].limitQuestion == 5)
				{
					//Command=_T("-5\r\n\r\n");
					CString diem;
					diem.Format(_T("%d"), pSock[post].scores);
					Command = _T("-5\r\n") + diem + _T("\r\n");

				}

				else
				{
					// Gui cau hoi tiep theo
					srand((unsigned)time(NULL));
					R = rand() % 49 + 0;
					CString stringR;
					stringR.Format(_T("%d"), R);
					Command = stringR;
					Command += "\r\n";
					Command += question[R] + _T("\r\n");
					pSock[post].limitQuestion++; // So cau hoi da gui
				}

			}

			// Người chơi trả lời sai
			// Kiểm tra số điểm có <0
			// Kiểm tra đã trả lời đủ 5 câu chưa
			// Kiểm tra trả lời sai 3 lần chưa
			else
			{
				pSock[post].scores--;
				pSock[post].limit++;
				//pSock[post].limitQuestion++;
				m_msgString += pSock[post].Name;
				m_msgString += _T( " trả lời SAI! \r\n");
				//Trả lời sai 3 câu
				if (pSock[post].limit == 3)
				{
					//Command=_T("-4\r\n\r\n");
					m_msgString += pSock[post].Name;
					m_msgString += _T(" trả lời sai 3 lần, dùng cuộc chơi! ID bị xóa!!\r\n");
					Command = _T("-4\r\n");
					Command += _T("\r\n");
				}
				// Kiểm tra điểm <0 chưa
				else if (pSock[post].scores < 0)
				{
					//Command=_T("-6\r\n\r\n");
					m_msgString += pSock[post].Name;
					m_msgString += _T(" có số điểm <0, dừng cuộc chơi! ID bị xóa!! \r\n");
					Command = _T("-6\r\n");
					Command += _T("\r\n");
				}
				// Kiểm tra trả lời đủ 5 câu chưa
				else if (pSock[post].limitQuestion == 5)
				{
					// Command = _T("-5\r\n\r\n");

					CString diem;
					diem.Format(_T("%d"), pSock[post].scores);
					m_msgString += pSock[post].Name;
					m_msgString += _T(" đã hoàn thành lượt chơi với số điểm: ")+ diem+_T("\r\n");
					Command = _T("-5\r\n") + diem + _T("\r\n");
				}
				// gửi tiếp câu hỏi
				else
				{
					srand((unsigned)time(NULL));
					R = rand() % 49 + 0;
					CString stringR;
					stringR.Format(_T("%d"), R);
					Command = stringR;
					Command += "\r\n";
					Command += question[R] + _T("\r\n");
					pSock[post].limitQuestion++;
				}

			}
		}
		mSend(wParam, Command);
		UpdateData(FALSE);
		break;
	}

	}
	return 0;
}