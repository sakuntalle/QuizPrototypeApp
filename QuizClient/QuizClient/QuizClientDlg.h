
// QuizClientDlg.h : header file
//

#pragma once
#include "afxwin.h"

#include <CommClient.h>


// CQuizClientDlg dialog
class CQuizClientDlg : public CDialogEx, public ICommClientObserver
{
	enum eApplicationStatus
	{
		eASWaitingForAnotherPlayerToConnect,
		eASCouldNotConnectToServer,
		eASWaitingForReceivingQuestion,
		eASWaitingForSendingResponse,
		eASSessionDisconnected
	};

// Construction
public:
	CQuizClientDlg(CWnd* pParent = NULL);	// standard constructor
	~CQuizClientDlg();

// Dialog Data
	enum { IDD = IDD_QUIZCLIENT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	afx_msg void OnBnClickedButtonConnect();
	afx_msg void OnBnClickedButtonAnswer(UINT nButtonID);
	
	afx_msg LRESULT UpdateStatusHandler(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT UpdatePlayersHandler(WPARAM wParam, LPARAM lParam);

	/*ICommClientObserver implementation*/
	virtual void OnClientConnected(bool bSuccess);
	virtual void OnStartInitReceived(std::shared_ptr<CommMessageBase> spStartInit);
	virtual void OnQuestionReceived(std::shared_ptr<CommMessageBase> spQuestion);
	virtual void OnSocketClosed();

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

private: 

	bool SetApplicationStatus(eApplicationStatus applicationStatus);
	void DisableGameControls();
	void EnableGameControls();

	// dialog control variables
	CStatic m_staticPlayer1Name;
	CStatic m_staticPlayer1Score;
	CStatic m_staticPlayer2Name;
	CStatic m_staticPlayer2Score;
	CStatic m_staticQuestion;
	CStatic m_staticVS;
	CStatic m_staticTimeLeft;
	CButton m_buttonAnswer1;
	CButton m_buttonAnswer2;
	CButton m_buttonAnswer3;
	CButton m_buttonAnswer4;
	CStatic m_staticStatus;
	CEdit m_editPlayerName;
	CEdit m_editServerName;
	CButton m_buttonConnect;

	// dialog value variables
	CString m_strPlayer1;
	CString m_strPlayer1Score;

	CString m_strPlayer2;
	CString m_strPlayer2Score;
	CString m_strQuestion;
	CString m_strTimeLeft;
	CString m_strStatus;
	CString m_strPlayerName;
	CString m_strServerName;

	CommClient m_commClient;
	int		m_nCorrectAnswer;
	
};
