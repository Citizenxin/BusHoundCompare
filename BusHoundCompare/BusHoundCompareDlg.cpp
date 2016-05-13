
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
	DDX_Control(pDX, IDC_PROGRESS_DECODE, m_progDecode);
}

BEGIN_MESSAGE_MAP(CBusHoundCompareDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CBusHoundCompareDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CBusHoundCompareDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BTN_SELECTPATH, &CBusHoundCompareDlg::OnBnClickedBtnSelectpath)
	ON_BN_CLICKED(IDC_BTN_COMPARE, &CBusHoundCompareDlg::OnBnClickedBtnCompare)
	ON_WM_CLOSE()
//	ON_WM_CREATE()
ON_WM_DROPFILES()
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
	m_progDecode.SetRange(0, 1000);
	m_progDecode.SetStep(1);

	for (int i = 0; i < MAX_DMA_NUM; i++)
	{
		m_lpucSecotrData[i] = new BYTE[MAX_TRANS_SEC_NUM*SECTOR];
	}

	m_lpucSysArea = new BYTE[SYSTEM_AREA_SIZE];
	m_lpucDataArea = new BYTE[DATA_AREA_SIZE];
	m_DataAreaMap = new vector<DWORD>;
	(*m_DataAreaMap).reserve(DATA_AREA_MAP_SIZE);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// ��������: InitialParam
// ��������: ��ʼ������
void CBusHoundCompareDlg::InitialParam()
{
	m_bRun = FALSE;
	m_bEnd = TRUE;
	m_bStop = TRUE;
	m_bCompareStart = FALSE;
	m_bStartWriteFlag = FALSE;

	m_nDataStartPoint = 0;
	m_nDataLen = 0;
	m_strDataPath.Empty();
	m_Granularity = GetAllocationGranularity();
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
		if(!GetRunFlag())
			CreateWorkThread();
	}
}

VOID CBusHoundCompareDlg::CreateWorkThread()
{
	SetRunFlag(TRUE);
	SetEndFlag(FALSE);
	SetStopFlag(FALSE);

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

BOOL CBusHoundCompareDlg::GetStopFlag()
{
	BOOL  bRet = FALSE;

	if (m_Mutex.Lock())
	{
		bRet = m_bStop;
		m_Mutex.Unlock();
	}

	return bRet;

}

BOOL CBusHoundCompareDlg::SetStopFlag(BOOL  stopFlag)
{
	if (m_Mutex.Lock())
	{
		m_bStop = stopFlag;
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

BOOL CBusHoundCompareDlg::GetDMAIdxMask(DWORD dmaIdx)
{
	BOOL  bRet = TRUE;		// Ĭ�ϱ�ռ��

	if (m_Mutex.Lock())
	{
		bRet = m_DMAMask&((DWORD)1<< dmaIdx);
		m_Mutex.Unlock();
	}

	return bRet;
}

BOOL CBusHoundCompareDlg::SetDMAIdxMask(DWORD dmaIdx, BOOL maskFlag)
{
	if (m_Mutex.Lock())
	{
		if(maskFlag)
			m_DMAMask |= ((DWORD)1 << dmaIdx);
		else
			m_DMAMask &= ~((DWORD)1 << dmaIdx);
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

			if (EOF != offset)
			{
				m_nDataStartPoint = offset + 1;

				// ��ȡ״̬��ƫ��λ��
				int phaseOffset = strLine.Find(_T("Phase"));
				int cmdPhaseOfs = strLine.Find(_T("Cmd.Phase.Ofs(rep)"));
				if (EOF != phaseOffset)
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
	
	m_DataIdx = 0;
	m_CBWIdx = 0;
	BOOL bOutOfRange = FALSE;

	m_PhaseType = 0;
	m_DmaIdx = 0;

	__int64 stepSize = m_nSrcFileSize / 1000;
	WORD progPos = 0;

	while (GetRunFlag())
	{
		//SetCompareStartFlag(TRUE); ��ʱ�ر�

		// ��ȡ�������
		while (uiBlkOffset < m_dwBlkSize)
		{
			if (((qwFileOffset + uiBlkOffset) / stepSize) > progPos)
			{
				m_progDecode.StepIt();
				progPos++;
			}
			if (GetStopFlag())
			{
				SetRunFlag(FALSE);
				break;
			}

			// �ж�Խ������
			if ((qwFileOffset + uiBlkOffset) >= m_nSrcFileSize)
			{
				bOutOfRange = TRUE;
				break;
			}

			strLine = FindLine(m_lpSrcMapAddress, uiBlkOffset, m_dwBlkSize);


			if (!m_strResidualData.IsEmpty())
				continue;

			int cmdIdx = strLine.Find(_T("CMD"), m_nPhaseStartPoint);
			int inIdx = strLine.Find(_T("IN"), m_nPhaseStartPoint);
			int outIdx = strLine.Find(_T("OUT"), m_nPhaseStartPoint);
			int spaceIdx = strLine.Find(_T(" "), m_nPhaseStartPoint);

			if ((cmdIdx == m_nPhaseStartPoint) || ((spaceIdx == m_nPhaseStartPoint) && (1 == m_PhaseType)))
			{
				// ������������������
				if ((cmdIdx == m_nPhaseStartPoint))
					m_CBWIdx = 0;

				CommandDecodeFlow(strLine);

			}
			else if (((inIdx == m_nPhaseStartPoint) || (outIdx == m_nPhaseStartPoint)) || ((spaceIdx == m_nPhaseStartPoint) && (2 == m_PhaseType)))
			{
				// �����������ݿ��������
				if((inIdx == m_nPhaseStartPoint) || (outIdx == m_nPhaseStartPoint))
					m_DataIdx = 0;

				DataDecodeFlow(strLine);
			}
			else
			{
				m_DataIdx = 0;
				m_CBWIdx = 0;
				m_PhaseType = 0;
			}
		}
		// �ж�Խ��������
		if (bOutOfRange)
			break;

		qwFileOffset += m_dwBlkSize;


		if (qwFileOffset > m_nSrcFileSize)
		{
			SetRunFlag(FALSE);
			break;
		}

		// �ļ���β���Ȳ���Խ��
		if ((m_nSrcFileSize - qwFileOffset) < m_dwBlkSize)
		{
			m_dwBlkSize = (DWORD)(m_nSrcFileSize - qwFileOffset);
		}

		// ����ǰ������ԭ����ӳ��
		if (!CreateMapAddr(m_hSrcFileMap, qwFileOffset, m_dwBlkSize, m_lpSrcMapAddress))
			break;

		uiBlkOffset = 0;

	}
	DistroyMapAddr(m_lpSrcMapAddress);

	if (!GetStopFlag())
	{
		m_progDecode.SetPos(1000);
		AddDisplay(_T("�������ݽ���!"));
	}	
	SetEndFlag(TRUE);
	return TRUE;
}

// ��������: CommandDecodeFlow
// ��������: �����������
// �������: strLine(�����ַ�����)
BOOL CBusHoundCompareDlg::CommandDecodeFlow(CString &strLine)
{
	CString strData;
	COMMAND_INFO cmdInfo;

	m_DataIdx = 0;		// �����������
	m_PhaseType = 1;	// ״̬���Ϊ����

	// ��ȡ�����ַ���
	strData = strLine.Mid(m_nDataStartPoint, m_nDataLen);
	strData.TrimRight();

	// �����ַ���ת�����ַ�����
	while (strData.GetLength())
	{
		strData.TrimLeft();
		m_ucCmdData[m_CBWIdx++] = StringToByte(strData);
		strData = strData.Mid(BYTE_STRING_LEN);
	}

	// ɸѡ����(0x28:����/0x2A:д��)
	if ((m_ucCmdData[CMD_BLK_CMDIDX] != 0x28) && (m_ucCmdData[CMD_BLK_CMDIDX] != 0x2a))
	{
		return FALSE;
	}
	else
	{
		// ��ȡ����״̬ƫ��
		CString strCmdPhaseOfs = strLine.Mid(m_nCmdPhaseOfsPoint, CMD_PHASE_OFS_LEN);
		
		
		UINT nextIdx = (m_DmaIdx++) % MAX_DMA_NUM;
		if (!GetDMAIdxMask(nextIdx))
		{
			cmdInfo.dmaIdx = nextIdx;
			SetDMAIdxMask(cmdInfo.dmaIdx, TRUE);
		}
		else
			SetRunFlag(FALSE);		// �����߳�

		// ��ȡ�����ַ���
		cmdInfo.addr = ReverseDWORD(*((DWORD *)&m_ucCmdData[CMD_BLK_ADDRIDX]));
		cmdInfo.sectorCnt = ReverseWORD(*((WORD *)&m_ucCmdData[CMD_BLK_LENIDX]));
		cmdInfo.direction = (m_ucCmdData[CMD_BLK_CMDIDX] == 0x2a);
		_tcscpy_s(cmdInfo.cmdPhaseOfs, strCmdPhaseOfs);

		// ����д����
		if (cmdInfo.direction)
			m_bStartWriteFlag = TRUE;
		if (!m_bStartWriteFlag)
		{
			SetDMAIdxMask(cmdInfo.dmaIdx, FALSE);			
		}

		m_CommandInfo.push(cmdInfo);
	}

	return TRUE;
}

// ��������: DataDecodeFlow
// ��������: ���ݽ�������
// �������: strLine(�����ַ�����)
BOOL CBusHoundCompareDlg::DataDecodeFlow(CString &strLine)
{
	CString strData;

	m_CBWIdx = 0;			// �����������
	m_PhaseType = 2;		// ״̬���Ϊ����
		   
	if (!ExistedWriteFlag())
	{
		return FALSE;
	}
	

	DWORD addr = m_CommandInfo.front().addr;
	WORD secCnt = m_CommandInfo.front().sectorCnt;
	UINT dmaIdx = m_CommandInfo.front().dmaIdx;
	TCHAR *cmdPhaseOfs = m_CommandInfo.front().cmdPhaseOfs;

	if (!m_CommandInfo.empty())
	{
		// ��ȡ�����ַ���
		strData = strLine.Mid(m_nDataStartPoint, m_nDataLen);
		strData.TrimRight();

		// �����ַ���ת��Ϊ�ַ�����
		while (strData.GetLength())
		{
			strData.TrimLeft();
			m_lpucSecotrData[m_CommandInfo.front().dmaIdx][m_DataIdx++] = StringToByte(strData);
			strData = strData.Mid(BYTE_STRING_LEN);
		}

		if (m_DataIdx == m_CommandInfo.front().sectorCnt*SECTOR)
		{
			if (m_CommandInfo.front().direction)
			{
				// д������
				if (!PseudoWriteData(addr, secCnt, dmaIdx))
				{
					return FALSE;
				}
			}
			else
			{
				// ��������
				if (!PseudoReadData(addr, secCnt, dmaIdx, cmdPhaseOfs))
				{
					return FALSE;
				}
			}
		}
	}

	return TRUE;
}

// ��������: ExistedWriteFlag
// ��������: �ж��Ƿ��ѷ���д����
BOOL CBusHoundCompareDlg::ExistedWriteFlag()
{
	if (!m_bStartWriteFlag)
	{
		// ����������
		while (!m_CommandInfo.empty())
		{
			m_CommandInfo.pop();
		}
		//m_DmaIdx = 0;				// DMA �������
		return FALSE;
	}
	return TRUE;
}

// ��������: PseudoWriteData
// ��������: ģ�� USB д�����ݲ���
// �������: addr(д���ַ), secCnt(д��������), dmaIdx(DMA ����)
BOOL CBusHoundCompareDlg::PseudoWriteData(DWORD addr, WORD secCnt, DWORD dmaIdx )
{
	// ���ݵ�ַ����Դ�(��ʱ��Ϊǰ˳�� 32M ��ӳ�� 16M)
	if (addr < (SYSTEM_AREA_SIZE / SECTOR))
	{
		// д��˳��ϵͳ��
		memcpy(&m_lpucSysArea[addr*SECTOR], m_lpucSecotrData[dmaIdx], secCnt*SECTOR);
	}
	else
	{
		for (int i = 0; i < secCnt; i++)
		{
			UINT secIdx;
			vector<DWORD>::iterator result = find((*m_DataAreaMap).begin(), (*m_DataAreaMap).end(), addr + i);
			if (result == (*m_DataAreaMap).end())	// δ�ҵ�
				secIdx = (*m_DataAreaMap).size();
			else
				secIdx = *result;

			if (secIdx >= DATA_AREA_MAP_SIZE)
				return FALSE;
			else
				(*m_DataAreaMap).push_back(addr + i);

			// д��ӳ��������
			memcpy(&m_lpucDataArea[secIdx*SECTOR], &m_lpucSecotrData[dmaIdx][i*SECTOR], SECTOR);
		}
	}
	//m_DmaIdx--;	
	SetDMAIdxMask(dmaIdx, FALSE);
	m_CommandInfo.pop();

	return TRUE;
}

// ��������: PseudoReadData
// ��������: ģ�� USB �������ݲ���
// �������: addr(д���ַ), secCnt(д��������), dmaIdx(DMA ����)
BOOL CBusHoundCompareDlg::PseudoReadData(DWORD addr, WORD secCnt, DWORD dmaIdx, TCHAR *cmdPhaseOfs)
{
	// ���ݵ�ַ����Դ�(��ʱ��Ϊǰ˳�� 32M ��ӳ�� 16M)
	if (m_CommandInfo.front().addr < (SYSTEM_AREA_SIZE / SECTOR))
	{
		if (0 != memcmp(&m_lpucSysArea[addr*SECTOR], m_lpucSecotrData[dmaIdx], secCnt*SECTOR))
		{
			ShowErrInfo(addr, cmdPhaseOfs);
		}
	}
	else
	{
		for (int i = 0; i < secCnt; i++)
		{
			UINT secIdx;
			vector<DWORD>::iterator result = find((*m_DataAreaMap).begin(), (*m_DataAreaMap).end(), addr + i);
			if (result == (*m_DataAreaMap).end())	// δ�ҵ�
				return FALSE;
			else
				secIdx = distance((*m_DataAreaMap).begin(), result);

			if (0 != memcmp(&m_lpucDataArea[secIdx*SECTOR], &m_lpucSecotrData[dmaIdx][i*SECTOR], SECTOR))
			{
				ShowErrInfo(addr, cmdPhaseOfs);
			}
		}
	}

	//m_DmaIdx--;
	SetDMAIdxMask(dmaIdx, FALSE);
	m_CommandInfo.pop();

	return TRUE;
}

void CBusHoundCompareDlg::ShowErrInfo(DWORD addr, TCHAR *cmdPhaseOfs)
{
	CString strShow;
	strShow.Format(_T("Error Address: 0x%-10X, Error Phase Offset: %-31s"), addr, cmdPhaseOfs);
	AddDisplay(strShow);
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

BYTE  CBusHoundCompareDlg::StringToByte(CString &strChar)
{
	BYTE  bRet = 0;
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

	// �����߳�
	if (SetStopFlag(TRUE))
	{
		while (!GetEndFlag())
		{
			Sleep(10);
		}
	}

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

	if (NULL != m_DataAreaMap)
	{
		delete m_DataAreaMap;
		m_DataAreaMap = NULL;
	}
	CDialogEx::OnClose();
}

void CBusHoundCompareDlg::OnDropFiles(HDROP hDropInfo)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	DragQueryFile(hDropInfo, NULL, m_strDataPath.GetBufferSetLength(MAX_PATH), MAX_PATH);
	m_editDataPath.SetWindowText(m_strDataPath);
	//m_editDataPath.UpdateWindow();
	DragFinish(hDropInfo);


	CDialogEx::OnDropFiles(hDropInfo);
}
