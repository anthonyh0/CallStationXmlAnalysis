
// CallStationXmlAnalysis.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CCallStationXmlAnalysisApp: 
// �йش����ʵ�֣������ CallStationXmlAnalysis.cpp
//

class CCallStationXmlAnalysisApp : public CWinApp
{
public:
	CCallStationXmlAnalysisApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CCallStationXmlAnalysisApp theApp;