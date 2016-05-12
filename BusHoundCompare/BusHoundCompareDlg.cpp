
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

UINT  AFX_CDECL BusHoundDecodeThread(LPVOID lpParam)
{
	CBusHoundCompareDlg *  lpDlg = (CBusHoundCompareDlg *)lpParam;

	if (lpDlg)
	{
		return lpDlg->DecodeThread();
	}

	return 0;
}

UINT  AFX_CDECL BusHoundCompareThread(LPVOID lpParam)
{
	CBusHoundCompareDlg *  lpDlg = (CBusHoundCompareDlg *)lpParam;

	if (lpDlg)
	{
		return lpDlg->CompareThread();
	}

	return 0;
}

CBusHoundCompareDlg::CBusHoundCompareDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_BUSHOUNDCOMPARE_DIALOG, pParent),
	m_lpDecodeThread(NULL),
	m_lpCompareThread(NULL),
	m_lpSrcMapAddress(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CBusHoundCompareDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_DATAPATH, m_editDataPath);
	DDX_Control(pDX, IDC_LIST_SHOWSTATUS, m_listShowStatus);
	DDX_Control(pDX, IDC_EDIT_GRANULARITY, m_editGranularity);
	DDX_Control(pDX, IDC_EDIT_BLKUNITSIZE, m_editBlkUnitSize);
	DDX_Control(pDX, IDC_EDIT_FATUNITSIZE, m_editFATUnitSize);
}

BEGIN_MESSAGE_MAP(CBusHoundCompareDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CBusHoundCompareDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CBusHoundCompareDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BTN_SELECTPATH, &CBusHoundCompareDlg::OnBnClickedBtnSelectpath)
	ON_BN_CLICKED(IDC_BTN_COMPARE, &CBusHoundCompareDlg::OnBnClickedBtnCompare)
	ON_WM_CLOSE()
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
	InitialParam();
	DisplayWindowInfo();

	for (int i = 0; i < MAX_DMA_NUM; i++)
	{
		m_lpucSecotrData[i] = new BYTE[MAX_TRANS_SEC_NUM*SECTOR];
	}

	m_lpucSysArea = new BYTE[SYSTEM_AREA_SIZE];
	m_lpucDataArea = new BYTE[DATA_AREA_SIZE];

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// ��������: InitialParam
// ��������: ��ʼ������
void CBusHoundCompareDlg::InitialParam()
{
	m_bRun = FALSE;
	m_bEnd = FALSE;
	m_bCompareStart = FALSE;

	m_nDataStartPoint = 0;
	m_nDataLen = 0;
	m_strDataPath.Empty();
	m_Granularity = GetAllocationGranularity();
}

// ��������: DisplayWindowInfo
// ��������: ��ʾ��������Ϣ
void CBusHoundCompareDlg::DisplayWindowInfo()
{
	CString str;

	str.Format(_T("0x%X"), m_Granularity);
	m_editGranularity.SetWindowText(str);

	str.Format(_T("0x%X"), BLOCK_UNIT_SIZE);
	m_editBlkUnitSize.SetWindowText(str);

	str.Format(_T("0x%X"), FAT_MAX_UINT_SIZE);
	m_editFATUnitSize.SetWindowText(str);
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
	else
	{
		// ���������߳�
		CreateWorkThread();
	}
}

VOID CBusHoundCompareDlg::CreateWorkThread()
{
	SetRunFlag(TRUE);
	SetEndFlag(FALSE);

	// �������������ļ��߳�
	CreateDecodeThread();

	//�����Ƚ������߳�
	//CreateCompareThread();
}

HANDLE CBusHoundCompareDlg::CreateUserFileMapping(CString strPath, __int64 &fileSize)
{
	// �����ļ�����
	HANDLE hFile = CreateFile(strPath,
		GENERIC_READ | GENERIC_WRITE,
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
	HANDLE hFileMap = CreateFileMapping(hFile,
		NULL,
		PAGE_READWRITE,
		0,
		0,
		NULL
	);
	if (INVALID_HANDLE_VALUE == hFileMap)
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

	return hFileMap;
}

// ��������: GetAllocationGranularity
// ��������: ��ȡϵͳ��������
DWORD CBusHoundCompareDlg::GetAllocationGranularity()
{
	// �õ�ϵͳ��������
	SYSTEM_INFO SysInfo;
	GetSystemInfo(&SysInfo);
	return SysInfo.dwAllocationGranularity;
}

// ��������: GetMappingBlkSize
// ��������: ��ȡӳ����С
// �������: fileSize(�ļ���С)
DWORD CBusHoundCompareDlg::GetMappingBlkSize(__int64 fileSize)
{
	return (fileSize < BLOCK_UNIT_SIZE * m_Granularity) ? ((DWORD)fileSize) : (BLOCK_UNIT_SIZE * m_Granularity);
}

// ��������: CreateDecodeThread
// ��������: �������������ļ��߳�
DWORD CBusHoundCompareDlg::CreateDecodeThread()
{
	DestroyDecodeThread();

	m_lpDecodeThread = AfxBeginThread(BusHoundDecodeThread,				//AFX_THREADPROC pfnThreadProc,
		(LPVOID)this,						//LPVOID pParam,
		THREAD_PRIORITY_NORMAL,    		    //int nPriority = THREAD_PRIORITY_NORMAL,
		NULL,								//UINT nStackSize = 0,
		0,					                //DWORD dwCreateFlags = 0, ������ֱ�������߳�
		NULL								//LPSECURITY_ATTRIBUTES lpSecurityAttrs = NULL 
	);

	return (DWORD)m_lpDecodeThread;
}

void CBusHoundCompareDlg::DestroyDecodeThread()
{
	if (m_lpDecodeThread)
	{
		Sleep(100);

		m_lpDecodeThread = NULL;
	}
}

// ��������: MappingVirtualMemory
// ��������: ӳ�������ڴ�
// �������:
// �������:
// ����ֵ  :
BOOL CBusHoundCompareDlg::MappingVirtualMemory()
{
	return FALSE;
}

// ��������: CreateCompareThread
// ��������: �����Ƚ������߳�
// �������:
// �������:
// ����ֵ  :
DWORD CBusHoundCompareDlg::CreateCompareThread()
{
	DestroyCompareThread();

	m_lpCompareThread = AfxBeginThread(BusHoundCompareThread,				//AFX_THREADPROC pfnThreadProc,
		(LPVOID)this,						//LPVOID pParam,
		THREAD_PRIORITY_NORMAL,    		    //int nPriority = THREAD_PRIORITY_NORMAL,
		NULL,								//UINT nStackSize = 0,
		0,					                //DWORD dwCreateFlags = 0, ������ֱ�������߳�
		NULL								//LPSECURITY_ATTRIBUTES lpSecurityAttrs = NULL 
	);

	return (DWORD)m_lpCompareThread;
}

void CBusHoundCompareDlg::DestroyCompareThread()
{
	if (m_lpCompareThread)
	{
		Sleep(100);

		m_lpCompareThread = NULL;
	}
}

BOOL CBusHoundCompareDlg::SetErrCode(UINT uErr)
{
	if (m_Mutex.Lock())
	{
		m_err = uErr;
		m_Mutex.Unlock();
		return TRUE;
	}

	return FALSE;
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

BOOL CBusHoundCompareDlg::GetRunFlag()
{
	BOOL  bRet = FALSE;

	if (m_Mutex.Lock())
	{
		bRet = m_bRun;
		m_Mutex.Unlock();
	}

	return bRet;

}

BOOL CBusHoundCompareDlg::SetRunFlag(BOOL  runFlag)
{
	if (m_Mutex.Lock())
	{
		m_bRun = runFlag;
		m_Mutex.Unlock();
		return TRUE;
	}

	return FALSE;
}

BOOL CBusHoundCompareDlg::GetEndFlag()
{
	BOOL  bRet = FALSE;

	if (m_Mutex.Lock())
	{
		bRet = m_bEnd;
		m_Mutex.Unlock();
	}

	return bRet;
}

BOOL CBusHoundCompareDlg::SetEndFlag(BOOL  endFlag)
{
	if (m_Mutex.Lock())
	{
		m_bEnd = endFlag;
		m_Mutex.Unlock();
		return TRUE;
	}

	return FALSE;
}

BOOL CBusHoundCompareDlg::GetCompareStartFlag()
{
	BOOL  bRet = FALSE;

	if (m_Mutex.Lock())
	{
		bRet = m_bCompareStart;
		m_Mutex.Unlock();
	}

	return bRet;
}

BOOL CBusHoundCompareDlg::SetCompareStartFlag(BOOL  startFlag)
{
	if (m_Mutex.Lock())
	{
		m_bCompareStart = startFlag;
		m_Mutex.Unlock();
		return TRUE;
	}

	return FALSE;
}

UINT CBusHoundCompareDlg::GetDataFlag()
{
	UINT  uiRet = 0;

	if (m_Mutex.Lock())
	{
		uiRet = m_DataFlag;
		m_Mutex.Unlock();
	}

	return uiRet;
}

BOOL CBusHoundCompareDlg::SetDataFlag(UINT  dataFlag)
{
	if (m_Mutex.Lock())
	{
		m_DataFlag &= dataFlag;
		m_Mutex.Unlock();
		return TRUE;
	}

	return FALSE;
}

// ��������: AddDisplay
// ��������: ��ʾ��Ϣ
BOOL CBusHoundCompareDlg::AddDisplay(LPCTSTR str)
{
	if (m_Mutex.Lock())
	{
		m_listShowStatus.AddString(str);
		m_Mutex.Unlock();
		return TRUE;
	}

	return FALSE;
}

BOOL CBusHoundCompareDlg::CreateMapAddr(HANDLE hFileMap, __int64 &fileOffset, DWORD blkSize, LPBYTE &mapAddr)
{
	DistroyMapAddr(mapAddr);

	// ӳ����ͼ
	mapAddr = (LPBYTE)MapViewOfFile(hFileMap,
		FILE_MAP_READ,
		(DWORD)(fileOffset >> 32),
		(DWORD)(fileOffset & 0xFFFFFFFF),
		blkSize);
	if (NULL == mapAddr)
	{
		return FALSE;
	}
	return TRUE;
}

BOOL CBusHoundCompareDlg::DistroyMapAddr(LPBYTE &mapAddr)
{
	// �����ļ�ӳ��
	if (mapAddr)
	{
		UnmapViewOfFile(mapAddr);
	}

	return TRUE;
}

// ��������: GetDataOffset
// ��������: ������������ʼ�����λ��(ֻ����һ���鵥Ԫ��С,δ�ҵ��򷵻�ʧ��)
// �������: fileOffset(�ļ�ƫ��), blkOffset(����ƫ��)
BOOL CBusHoundCompareDlg::GetDataOffset(__int64 &fileOffset, UINT &blkOffset)
{
	if (!CreateMapAddr(m_hSrcFileMap, fileOffset, m_dwBlkSize, m_lpSrcMapAddress))
		return FALSE;

	while (blkOffset < m_dwBlkSize)
	{
		CString strLine = FindLine(m_lpSrcMapAddress, blkOffset, m_dwBlkSize);
		if (m_nDataStartPoint)
		{
			int offset = strLine.Find(_T("----"), m_nDataStartPoint);

			if (m_nDataStartPoint == offset)
			{
				offset = strLine.Find(_T(" "), m_nDataStartPoint);
				m_nDataLen = offset - m_nDataStartPoint;
			}

			else
			{
				m_nDataStartPoint = 0;
				m_nPhaseStartPoint = 0;
				m_nCmdPhaseOfsPoint = 0;
			}

		}
		else
		{
			int offset = strLine.Find(_T(" Data "));

			if (-1 != offset)
			{
				m_nDataStartPoint = offset + 1;

				// ��ȡ״̬��ƫ��λ��
				int phaseOffset = strLine.Find(_T("Phase"));
				int cmdPhaseOfs = strLine.Find(_T("Cmd.Phase.Ofs(rep)"));
				if (-1 != phaseOffset)
				{
					m_nCmdPhaseOfsPoint = cmdPhaseOfs;
					m_nPhaseStartPoint = phaseOffset;
				}
				else
				{
					m_nDataStartPoint = 0;
					m_nPhaseStartPoint = 0;
					m_nCmdPhaseOfsPoint = 0;
				}
			}

		}

		if (m_nDataLen)
			return TRUE;

	}

	return FALSE;
}

DWORD   CBusHoundCompareDlg::DecodeThread()
{
	__int64 qwFileOffset = 0;
	UINT    uiBlkOffset = 0;

	m_strResidualData.Empty();

	AddDisplay(_T("�������ݿ�ʼ!"));
	// �����ļ�ӳ�䲢��ȡ�ļ�����
	m_hSrcFileMap = CreateUserFileMapping(m_strDataPath, m_nSrcFileSize);

	// ����ӳ����С
	m_dwBlkSize = GetMappingBlkSize(m_nSrcFileSize);

	// ���������ļ���������ƫ��
	if (!GetDataOffset(qwFileOffset, uiBlkOffset))
	{
		AddDisplay(_T("��������ʧ��!"));
		return FALSE;
	}

	CString strLine;
	CString strData;
	UINT dmaIdx = 0;
	UINT dataIdx = 0;
	UINT cbwIdx = 0;
	UINT phaseType = 0;  // 0:����״̬/1:����״̬/2:����״̬
	COMMAND_INFO cmdInfo;
	BOOL bStartWriteFlag = FALSE;
	BOOL bOutOfRange = FALSE;

	m_DataAreaMap.clear();

	while (GetRunFlag())
	{
		//SetCompareStartFlag(TRUE); ��ʱ�ر�

		// ��ȡ�������
		while (uiBlkOffset < m_dwBlkSize)
		{
			// �ж�Խ������
			if ((qwFileOffset + uiBlkOffset) >= m_nSrcFileSize)
			{
				bOutOfRange = TRUE;
				break;
			}

			strLine = FindLine(m_lpSrcMapAddress, uiBlkOffset, m_dwBlkSize);       // 5ms

			int cmdIdx = strLine.Find(_T("CMD"), m_nPhaseStartPoint);
			int inIdx = strLine.Find(_T("IN"), m_nPhaseStartPoint);
			int outIdx = strLine.Find(_T("OUT"), m_nPhaseStartPoint);
			int spaceIdx = strLine.Find(_T(" "), m_nPhaseStartPoint);

			if ((cmdIdx == m_nPhaseStartPoint) || ((spaceIdx == m_nPhaseStartPoint) && (1 == phaseType)))
			{
				// ������������������
				if ((cmdIdx == m_nPhaseStartPoint))
					cbwIdx = 0;

				dataIdx = 0;
				phaseType = 1;
				// ��ȡ����
				strData = strLine.Mid(m_nDataStartPoint, m_nDataLen);
				strData.TrimRight();
					

				while (strData.GetLength())
				{
					strData.TrimLeft();
					m_ucCmdData[cbwIdx++] = StringToByte(strData);
					strData = strData.Mid(BYTE_STRING_LEN);
				}
				if ((m_ucCmdData[0] != 0x28) && (m_ucCmdData[0] != 0x2a))
					continue;
				else
				{
					CString strPhaseOfs = strLine.Mid(m_nCmdPhaseOfsPoint, CMD_PHASE_OFS_LEN);
					strPhaseOfs.Trim();

					if (-1 != strPhaseOfs.Find(_T("2270.1.0")))
					{
						strPhaseOfs.Trim();
					}

					_tcscpy_s(cmdInfo.cmdPhaseOfs, strPhaseOfs);
					//memcpy(cmdInfo.cmdPhaseOfs, strPhaseOfs, strPhaseOfs.GetLength() + 1);
					cmdInfo.addr = ReverseDWORD(*((DWORD *)&m_ucCmdData[2]));
					cmdInfo.sectorCnt = ReverseWORD(*((WORD *)&m_ucCmdData[7]));
					cmdInfo.dmaIdx = dmaIdx++;
					cmdInfo.direction = (m_ucCmdData[0] == 0x2a);
					m_CommandInfo.push(cmdInfo);

					if(cmdInfo.direction)
						bStartWriteFlag = TRUE;
					//ASSERT(m_CommandInfo.size() < 2);
					continue;
				}
			}
			else if (((inIdx == m_nPhaseStartPoint) || (outIdx == m_nPhaseStartPoint)) || ((spaceIdx == m_nPhaseStartPoint) && (2 == phaseType)))
			{
				// �����������ݿ��������
				if((inIdx == m_nPhaseStartPoint) || (outIdx == m_nPhaseStartPoint))
					dataIdx = 0;

				phaseType = 2;
				cbwIdx = 0;

				// δ��ʼд֮ǰ���Ƚ�����
				if (!bStartWriteFlag)
				{
					if (!m_CommandInfo.empty())
					{
						m_CommandInfo.pop();
						dmaIdx = 0;
					}
					continue;
				}
					
						
					

				// ��ȡ����
				if (!m_CommandInfo.empty())
				{
					
					strData = strLine.Mid(m_nDataStartPoint, m_nDataLen);    // 6ms
					strData.TrimRight();


					while (strData.GetLength())
					{
						strData.TrimLeft();
						m_lpucSecotrData[m_CommandInfo.front().dmaIdx][dataIdx++] = StringToByte(strData);
						strData = strData.Mid(BYTE_STRING_LEN);
					}

					//ASSERT(dataIdx <= (UINT)m_CommandInfo.front().sectorCnt*SECTOR);
					if (dataIdx == m_CommandInfo.front().sectorCnt*SECTOR)
					{
						if (m_CommandInfo.front().direction)
						{
							if (m_CommandInfo.front().addr < (SYSTEM_AREA_SIZE / SECTOR))
							{
								memcpy(&m_lpucSysArea[m_CommandInfo.front().addr*SECTOR], m_lpucSecotrData[m_CommandInfo.front().dmaIdx], m_CommandInfo.front().sectorCnt*SECTOR);
							}
							else
							{
								memcpy(&m_lpucDataArea[m_DataAreaMap.size()*SECTOR], m_lpucSecotrData[m_CommandInfo.front().dmaIdx], m_CommandInfo.front().sectorCnt*SECTOR);
								for (int i = 0; i < m_CommandInfo.front().sectorCnt; i++)
								{
									m_DataAreaMap.insert(make_pair(m_CommandInfo.front().addr, m_DataAreaMap.size()));
								}
								
							}
							dmaIdx--;
							m_CommandInfo.pop();
						}
						else
						{
							if (bStartWriteFlag)
							{
								if (m_CommandInfo.front().addr < (SYSTEM_AREA_SIZE / SECTOR))
								{
									if (0 != memcmp(&m_lpucSysArea[m_CommandInfo.front().addr*SECTOR], m_lpucSecotrData[m_CommandInfo.front().dmaIdx], m_CommandInfo.front().sectorCnt*SECTOR))
									{
										CString strShow;
										strShow.Format(_T("Error Address: 0x%8X, Error Phase Offset: %s"), m_CommandInfo.front().addr, m_CommandInfo.front().cmdPhaseOfs);
										AddDisplay(strShow);
									}
								}
								else
								{
									for (int i = 0; i < m_CommandInfo.front().sectorCnt; i++)
									{
										if (0 != memcmp(&m_lpucDataArea[m_DataAreaMap[m_CommandInfo.front().addr+i]*SECTOR], &m_lpucSecotrData[m_CommandInfo.front().dmaIdx][i*SECTOR], SECTOR))
										{
											CString strShow;
											strShow.Format(_T("Error Address: 0x%8X, Error Phase Offset: %s"), m_CommandInfo.front().addr, m_CommandInfo.front().cmdPhaseOfs);
											AddDisplay(strShow);
										}
									}
								}
							}
							dmaIdx--;
							m_CommandInfo.pop();
						}
					}
				}
			}
			else
			{
				dataIdx = 0;
				cbwIdx = 0;
				phaseType = 0;
			}
		}
		// �ж�Խ��������
		if (bOutOfRange)
			break;

		qwFileOffset += m_dwBlkSize;

		
		if (qwFileOffset > m_nSrcFileSize)
			break;

		// ����ǰ������ԭ����ӳ��
		if (!CreateMapAddr(m_hSrcFileMap, qwFileOffset, m_dwBlkSize, m_lpSrcMapAddress))
			return FALSE;

		uiBlkOffset = 0;

	}
	DistroyMapAddr(m_lpSrcMapAddress);
	//SetRunFlag(FALSE);
	SetEndFlag(TRUE);
	AddDisplay(_T("�������ݽ���!"));
	return TRUE;
}

DWORD   CBusHoundCompareDlg::CompareThread()
{
	while (GetRunFlag())
	{

		if (GetCompareStartFlag())
		{
			AddDisplay(_T("�Ƚ����ݿ�ʼ!"));

		}
		else
		{

		}
	}

	AddDisplay(_T("�Ƚ����ݽ���!"));
	return TRUE;
}

CString  CBusHoundCompareDlg::FindLine(LPBYTE  pByte, UINT & uiIndex, UINT uiLen)
{
	CString    strRet;
	char       szChar[2] = "0";
	char  & ch = szChar[0];

	// ������һ�β����޽�β���ַ���
	if (!m_strResidualData.IsEmpty())
	{
		strRet = m_strResidualData;
		m_strResidualData.Empty();
	}
	for (UINT i = uiIndex; i < uiLen; i++)
	{
		ch = pByte[i];
		if ((0x0d != ch) && (0x0a != ch))
		{

			strRet += szChar;

			if (i == uiLen - 1)
			{
				uiIndex = uiLen;
				m_strResidualData = strRet;
				strRet = "";
				break;
			}
			continue;
		}
		else
		{
			uiIndex = i + 1;

			if (strRet.GetLength() > 0)
				break;
			else
				continue;
		}
	}
	return strRet;
}

BYTE  CBusHoundCompareDlg::StringToByte(CString strChar)
{
	BYTE  bRet = 0;
	//int iLen = strChar.GetLength();
	ASSERT(strChar.GetLength() >= BYTE_STRING_LEN);
	strChar.MakeUpper();

	for (int i = 0; i < BYTE_STRING_LEN; i++)
	{
		TCHAR ch = strChar.GetAt(i);
		if ((ch <= _T('9')) && (ch >= _T('0')))
		{
			bRet <<= 4;
			bRet |= (ch - _T('0'));
		}
		else if ((ch <= _T('F')) && (ch >= _T('A')))
		{
			bRet <<= 4;
			bRet |= (ch - _T('A') + 10);
		}
	}
	return bRet;
}

DWORD CBusHoundCompareDlg::ReverseDWORD(DWORD InData)
{
	BYTE   da1 = (BYTE)(InData);
	BYTE   da2 = (BYTE)(InData >> 8);
	BYTE   da3 = (BYTE)(InData >> 16);
	BYTE   da4 = (BYTE)(InData >> 24);

	return ((((DWORD)da1) << 24) | (((DWORD)da2) << 16) | (((DWORD)da3) << 8) | (((DWORD)da4)));
}

WORD CBusHoundCompareDlg::ReverseWORD(WORD InData)
{
	return  ((InData >> 8) | (InData << 8));
}

void CBusHoundCompareDlg::OnClose()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	for (int i = 0; i < MAX_DMA_NUM; i++)
	{
		if (NULL != m_lpucSecotrData[i])
		{
			delete[] m_lpucSecotrData[i];
			m_lpucSecotrData[i] = NULL;
		}
	}

	if (NULL != m_lpucSysArea)
	{
		delete[] m_lpucSysArea;
		m_lpucSysArea = NULL;
	}

	if (NULL != m_lpucDataArea)
	{
		delete[] m_lpucDataArea;
		m_lpucDataArea = NULL;
	}

	CDialogEx::OnClose();
}
