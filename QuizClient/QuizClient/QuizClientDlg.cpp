
// QuizClientDlg.cpp : implementation file
//

#include "stdafx.h"
#define _CRT_NONSTDC_NO_WARNINGS
#include "QuizClient.h"
#include "QuizClientDlg.h"
#include "afxdialogex.h"

#include <CommClient.h>
#include "StartInitMessage.h"
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CQuizClientDlg dialog

#define WM_UPDATE_STATUS	(WM_USER + 100)
#define WM_UPDATE_PLAYERS	(WM_USER + 101)

CQuizClientDlg::CQuizClientDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CQuizClientDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CQuizClientDlg::~CQuizClientDlg()
{
	CommObserverManager::GetInstance()->UnregisterCommClientListener(this);
}

void CQuizClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_STATIC_PLAYER1_NAME, m_staticPlayer1Name);
	DDX_Text(pDX, IDC_STATIC_PLAYER1_NAME, m_strPlayer1);
	DDX_Control(pDX, IDC_STATIC_PLAYER1_SCORE, m_staticPlayer1Score);
	DDX_Text(pDX, IDC_STATIC_PLAYER1_SCORE, m_strPlayer1Score);
	DDX_Text(pDX, IDC_STATIC_TIME_LEFT, m_strTimeLeft);
	DDX_Control(pDX, IDC_STATIC_TIME_LEFT, m_staticTimeLeft);
	DDX_Control(pDX, IDC_STATIC_PLAYER2_NAME2, m_staticPlayer2Name);
	DDX_Text(pDX, IDC_STATIC_PLAYER2_NAME2, m_strPlayer2);
	DDX_Control(pDX, IDC_STATIC_PLAYER2_SCORE2, m_staticPlayer2Score);
	DDX_Text(pDX, IDC_STATIC_PLAYER2_SCORE2, m_strPlayer2Score);
	DDX_Control(pDX, IDC_STATIC_QUESTION, m_staticQuestion);
	DDX_Text(pDX, IDC_STATIC_QUESTION, m_strQuestion);
	DDX_Control(pDX, IDC_STATIC_VS, m_staticVS);
	DDX_Control(pDX, IDC_BUTTON_ANSWER1, m_buttonAnswer1);
	DDX_Control(pDX, IDC_BUTTON_ANSWER2, m_buttonAnswer2);
	DDX_Control(pDX, IDC_BUTTON_ANSWER3, m_buttonAnswer3);
	DDX_Control(pDX, IDC_BUTTON_ANSWER4, m_buttonAnswer4);
	DDX_Control(pDX, IDC_STATIC_STATUS, m_staticStatus);
	DDX_Text(pDX, IDC_STATIC_STATUS, m_strStatus);
	DDX_Control(pDX, IDC_EDIT_PLAYER_NAME, m_editPlayerName);
	DDX_Text(pDX, IDC_EDIT_PLAYER_NAME, m_strPlayerName);
	DDX_Control(pDX, IDC_EDIT_SERVER_NAME, m_editServerName);
	DDX_Text(pDX, IDC_EDIT_SERVER_NAME, m_strServerName);
	DDX_Control(pDX, IDC_BUTTON_CONNECT, m_buttonConnect);
}

void CQuizClientDlg::OnBnClickedButtonConnect()
{
	UpdateData(TRUE);

	m_buttonConnect.EnableWindow(FALSE);

	bool bResult = m_commClient.Connect(CT2A(m_strServerName.GetBuffer(0)).m_psz, CT2A(m_strPlayerName.GetBuffer(0)).m_psz);

	if (bResult)
	{
		SetApplicationStatus(eASWaitingForAnotherPlayerToConnect);
	}
	else
	{
		SetApplicationStatus(eASCouldNotConnectToServer);
	}
}

void CQuizClientDlg::OnBnClickedButtonAnswer(UINT nButtonID)
{
	int nButton = nButtonID - IDC_BUTTON_ANSWER1;
	if(m_nCorrectAnswer == nButton)
	{
		MessageBox(_T("You are right! The game does not continue with other questions"), NULL, MB_OK);
	}
	else
	{
		MessageBox(_T("Wrong! The game does not continue with other questions"), NULL, MB_OK);
	}
}

LRESULT CQuizClientDlg::UpdateStatusHandler(WPARAM wParam, LPARAM lParam)
{
	eApplicationStatus eAppStatus = static_cast<eApplicationStatus>(wParam);
	SetApplicationStatus(eAppStatus);

	return 0;
}

LRESULT CQuizClientDlg::UpdatePlayersHandler(WPARAM wParam, LPARAM lParam)
{
	EnableGameControls();

	std::unique_ptr<StartInitMessage> spStartInit(reinterpret_cast<StartInitMessage*>(wParam));
	
	m_strPlayer1 = m_strPlayerName;
	m_strPlayer2 = CString(spStartInit->GetOpponentName().c_str());

	std::vector<std::string> vectToken;
	boost::char_separator<char> sep("|");
	boost::tokenizer< boost::char_separator<char> > arrTokens(spStartInit->GetQuestions(), sep);
	for (auto it = arrTokens.begin(); it != arrTokens.end(); it++)
	{
		vectToken.push_back(*it);
	}

	m_strQuestion = CString(vectToken[1].c_str());
	m_buttonAnswer1.SetWindowText(CString(vectToken[2].c_str()));
	m_buttonAnswer2.SetWindowText(CString(vectToken[3].c_str()));
	m_buttonAnswer3.SetWindowText(CString(vectToken[4].c_str()));
	m_buttonAnswer4.SetWindowText(CString(vectToken[5].c_str()));
	m_nCorrectAnswer = std::atoi(vectToken[6].c_str());

	UpdateData(FALSE);

	return 0;
}

/*ICommClientObserver implementation*/
void CQuizClientDlg::OnClientConnected(bool bSuccess)
{
	if (!bSuccess)
	{
		PostMessage(WM_UPDATE_STATUS, static_cast<WPARAM>(eASCouldNotConnectToServer));
	}
}

void CQuizClientDlg::OnStartInitReceived(std::shared_ptr<CommMessageBase> spStartInit)
{
	PostMessage(WM_UPDATE_STATUS, static_cast<WPARAM>(eASWaitingForSendingResponse));

	StartInitMessage* pStartInit = new StartInitMessage(*(static_cast<StartInitMessage*>(spStartInit.get())));
	PostMessage(WM_UPDATE_PLAYERS, reinterpret_cast<WPARAM>(pStartInit));
}

void CQuizClientDlg::OnQuestionReceived(std::shared_ptr<CommMessageBase> spQuestion)
{

}

void CQuizClientDlg::OnSocketClosed()
{
	PostMessage(WM_UPDATE_STATUS, static_cast<WPARAM>(eASSessionDisconnected));
}

BEGIN_MESSAGE_MAP(CQuizClientDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_CONNECT, &CQuizClientDlg::OnBnClickedButtonConnect)
	ON_MESSAGE(WM_UPDATE_STATUS, &CQuizClientDlg::UpdateStatusHandler)
	ON_MESSAGE(WM_UPDATE_PLAYERS, &CQuizClientDlg::UpdatePlayersHandler)
	ON_COMMAND_RANGE(IDC_BUTTON_ANSWER1, IDC_BUTTON_ANSWER4, &CQuizClientDlg::OnBnClickedButtonAnswer)
END_MESSAGE_MAP()


// CQuizClientDlg message handlers

BOOL CQuizClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	m_strServerName = "localhost";
	m_strPlayerName = "Mihnea";
	UpdateData(FALSE);

	DisableGameControls();
	CommObserverManager::GetInstance()->RegisterCommClientListener(this);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CQuizClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CQuizClientDlg::OnPaint()
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
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CQuizClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

bool CQuizClientDlg::SetApplicationStatus(eApplicationStatus applicationStatus)
{
	switch (applicationStatus)
	{
		case eASWaitingForAnotherPlayerToConnect: m_strStatus = _T("Waiting for another player to connect to the Quiz server"); break;
		case eASWaitingForReceivingQuestion:  m_strStatus = _T("Connected to game. Waiting for Question "); break;
		case eASWaitingForSendingResponse:	  m_strStatus = _T("Waiting for sending answer to Question "); break;
		case eASCouldNotConnectToServer:
		{
			m_strStatus = _T("Could not connect to the Quiz server"); 
			m_buttonConnect.EnableWindow(TRUE);
		}
		break;
		case eASSessionDisconnected:
		{
			m_strStatus = _T("Session was disconnected"); 
			m_buttonConnect.EnableWindow(TRUE);
		}
		break;
		default: ASSERT(false); return false;
	}

	UpdateData(FALSE);
	return true;
}

void CQuizClientDlg::DisableGameControls()
{
	m_staticPlayer1Name.ShowWindow(SW_HIDE);
	m_staticPlayer1Score.ShowWindow(SW_HIDE);
	m_staticPlayer2Name.ShowWindow(SW_HIDE);
	m_staticPlayer2Score.ShowWindow(SW_HIDE);
	m_staticQuestion.ShowWindow(SW_HIDE);
	m_staticVS.ShowWindow(SW_HIDE);
	m_staticTimeLeft.ShowWindow(SW_HIDE);
	m_buttonAnswer1.ShowWindow(SW_HIDE);
	m_buttonAnswer2.ShowWindow(SW_HIDE);
	m_buttonAnswer3.ShowWindow(SW_HIDE);
	m_buttonAnswer4.ShowWindow(SW_HIDE);
}

void CQuizClientDlg::EnableGameControls()
{
	m_staticPlayer1Name.ShowWindow(SW_SHOW);
	m_staticPlayer1Score.ShowWindow(SW_SHOW);
	m_staticPlayer2Name.ShowWindow(SW_SHOW);
	m_staticPlayer2Score.ShowWindow(SW_SHOW);
	m_staticQuestion.ShowWindow(SW_SHOW);
	m_staticVS.ShowWindow(SW_SHOW);
	m_staticTimeLeft.ShowWindow(SW_HIDE);
	m_buttonAnswer1.ShowWindow(SW_SHOW);
	m_buttonAnswer2.ShowWindow(SW_SHOW);
	m_buttonAnswer3.ShowWindow(SW_SHOW);
	m_buttonAnswer4.ShowWindow(SW_SHOW);
}



