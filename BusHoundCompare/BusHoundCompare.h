
// BusHoundCompare.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CBusHoundCompareApp: 
// �йش����ʵ�֣������ BusHoundCompare.cpp
//

class CBusHoundCompareApp : public CWinApp
{
public:
	CBusHoundCompareApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CBusHoundCompareApp theApp;