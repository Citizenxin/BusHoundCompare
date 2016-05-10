
// BusHoundCompareDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "BusHoundCompare.h"
#include "BusHoundCompareDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CBusHoundCompareDlg �Ի���



CBusHoundCompareDlg::CBusHoundCompareDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_BUSHOUNDCOMPARE_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CBusHoundCompareDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_DATAPATH, m_editDataPath);
}

BEGIN_MESSAGE_MAP(CBusHoundCompareDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CBusHoundCompareDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CBusHoundCompareDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BTN_SELECTPATH, &CBusHoundCompareDlg::OnBnClickedBtnSelectpath)
	ON_BN_CLICKED(IDC_BTN_COMPARE, &CBusHoundCompareDlg::OnBnClickedBtnCompare)
END_MESSAGE_MAP()


// CBusHoundCompareDlg ��Ϣ�������

BOOL CBusHoundCompareDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	m_strDataPath.Empty();

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CBusHoundCompareDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CBusHoundCompareDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

CString CBusHoundCompareDlg::GetCurrentPath()
{
	TCHAR szPath[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, szPath, MAX_PATH);
	TCHAR drive[MAX_PATH], dir[MAX_PATH], fname[MAX_PATH], ext[MAX_PATH];
	_tsplitpath_s(szPath, drive, dir, fname, ext);
	_tcscpy_s(szPath, MAX_PATH - 1, drive);
	_tcscat_s(szPath, MAX_PATH - 1, dir);
	CString strPath = szPath;
	return strPath;
}

void CBusHoundCompareDlg::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//CDialogEx::OnOK();
}


void CBusHoundCompareDlg::OnBnClickedCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CDialogEx::OnCancel();
}


void CBusHoundCompareDlg::OnBnClickedBtnSelectpath()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CFileDialog dlg(TRUE, 
		NULL, 
		NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("All Files (*.*)|*.*||"),
		NULL);

	if (dlg.DoModal() == IDOK)
	{
		m_strDataPath = dlg.GetPathName();
		m_editDataPath.SetWindowText(m_strDataPath);
	}
}


void CBusHoundCompareDlg::OnBnClickedBtnCompare()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (m_strDataPath.IsEmpty())
		MessageBox(_T("����ѡȡBusHound�����ļ�!"), _T("����"), MB_ICONWARNING | MB_OK);
	do
	{
		// ӳ�������ļ�
		if (MappingDataFile())
		{
			// �������������ļ��߳�
			CreateDecodeThread();
		}	
		else
			break;

		// ӳ�������ڴ�
		if (MappingVirtualMemory())
		{
			//�����Ƚ������߳�
			CreateCompareThread();
		}
					
	} while (false);
	
	//CompareData();
	//CFile tmpFile;
	//tmpFile.Open(_T("tmpData.bin"), CFile::modeCreate | CFile::modeReadWrite);

}

BOOL CBusHoundCompareDlg::CompareData()
{
	m_hSrcFileMap = CreateUserFileMapping(m_strDataPath, m_nSrcFileSize);

	// �õ�ϵͳ��������
	SYSTEM_INFO SysInfo;
	GetSystemInfo(&SysInfo);
	DWORD dwGran = SysInfo.dwAllocationGranularity;

	// ƫ�Ƶ�ַ 
	__int64 qwFileOffset = 0;
	// ���С(����Ϊ1024������)
	DWORD dwBlockBytes = 0x400 * dwGran;
	if (m_nSrcFileSize < 0x400 * dwGran)
		dwBlockBytes = (DWORD)m_nSrcFileSize;
	/*
	while (m_nSrcFileSize > 0)
	{
		// ӳ����ͼ
		LPBYTE lpbMapAddress = (LPBYTE)MapViewOfFile(m_hSrcFileMap, 
			FILE_MAP_READ,
			(DWORD)(qwFileOffset >> 32), 
			(DWORD)(qwFileOffset & 0xFFFFFFFF),
			dwBlockBytes);
		if (NULL == lpbMapAddress)
		{
			TRACE("ӳ���ļ�ӳ��ʧ��,�������:%drn", GetLastError());
			return FALSE;
		}
		// ��ӳ�����ͼ���з���
		for (DWORD i = 0; i < dwBlockBytes; i++)
			BYTE temp = *(lpbMapAddress + i);
		// �����ļ�ӳ��
		UnmapViewOfFile(lpbMapAddress);
		// ��������
		qwFileOffset += dwBlockBytes;
		m_nSrcFileSize -= dwBlockBytes;
	}
	*/

	// �ر��ļ�ӳ�������
	CloseHandle(m_hSrcFileMap);


	return TRUE;
}

HANDLE CBusHoundCompareDlg::CreateUserFileMapping(CString strPath, __int64 &fileSize)
{
	// �����ļ�����
	HANDLE hFile = CreateFile(strPath,
		GENERIC_READ,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		MessageBox(_T("�����ļ�����ʧ��!"));
		return INVALID_HANDLE_VALUE;
	}

	// �����ļ�ӳ�����
	HANDLE hSrcFileMap = CreateFileMapping(hFile,
		NULL,
		PAGE_READONLY,
		0,
		0,
		NULL
	);
	if (INVALID_HANDLE_VALUE == hSrcFileMap)
	{
		MessageBox(_T("�����ļ�����ʧ��!"));
		return INVALID_HANDLE_VALUE;
	}

	// �õ��ļ��ߴ�
	DWORD dwFileSizeHigh;
	fileSize = GetFileSize(hFile, &dwFileSizeHigh);
	fileSize |= (((__int64)dwFileSizeHigh) << 32);

	// �ر��ļ�����
	CloseHandle(hFile);

	return hSrcFileMap;
}

DWORD CBusHoundCompareDlg::GetMappingBlkSize(__int64 srcFileSize)
{
	// �õ�ϵͳ��������
	SYSTEM_INFO SysInfo;
	GetSystemInfo(&SysInfo);
	DWORD dwGran = SysInfo.dwAllocationGranularity;

	if (srcFileSize < BLOCK_UNIT_SIZE * dwGran)
		return((DWORD)srcFileSize);
	else
		return(BLOCK_UNIT_SIZE * dwGran);
}

// ��������: MappingDataFile
// ��������: ӳ�������ļ�
BOOL CBusHoundCompareDlg::MappingDataFile()
{
	// �����ļ�ӳ�䲢��ȡ�ļ�����
	m_hSrcFileMap = CreateUserFileMapping(m_strDataPath, m_nSrcFileSize);

	// ����ӳ����С
	m_dwBlkSize = GetMappingBlkSize(m_nSrcFileSize);

	return TRUE;
}

// ��������: CreateDecodeThread
// ��������: �������������ļ��߳�
VOID CBusHoundCompareDlg::CreateDecodeThread()
{

}

// ��������: MappingVirtualMemory
// ��������: ӳ�������ڴ�
BOOL CBusHoundCompareDlg::MappingVirtualMemory()
{
	return FALSE;
}

// ��������: CreateCompareThread
// ��������: �����Ƚ������߳�
VOID CBusHoundCompareDlg::CreateCompareThread()
{

}

VOID CBusHoundCompareDlg::SetErrCode(UINT uErr)
{
	if (m_Mutex.Lock())
	{
		m_err = uErr;
		m_Mutex.Unlock();
	}
}

UINT CBusHoundCompareDlg::GetErrCode()
{
	UINT uErr = 0;
	if (m_Mutex.Lock())
	{
		uErr = m_err;
		m_Mutex.Unlock();
	}
	return uErr;
}