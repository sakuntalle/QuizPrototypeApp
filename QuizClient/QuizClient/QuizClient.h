
// QuizClient.h : main header file for the PROJECT_NAME application
//

#pragma once

#include "stdafx.h"
#include "resource.h"		// main symbols


// CQuizClientApp:
// See QuizClient.cpp for the implementation of this class
//

class CQuizClientApp : public CWinApp
{
public:
	CQuizClientApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CQuizClientApp theApp;