
// BusHoundCompareDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"

#include <queue>
#include <vector>
#include "afxcmn.h"
#include <map>
using namespace std;

#define DATA_FILE_EX			(1)				// �½�������

#define SECTOR                  (0x200)         // ������С
#define MAX_TRANSFER_LEN        (0x10000)       // ��������䳤��(64K)
#define BLOCK_UNIT_SIZE			(0x400)			// �鵥Ԫ����
#define FAT_MAX_UINT_SIZE		(0x400)			// FAT��Ԥ�Ƶ�Ԫ����

#define CBW_MAX_LEN             (0x1E)			// Command Block Wrapper ����
#define SYSTEM_AREA_SIZE        (0x2000000)		// 32M ��ϵͳ��ʹ��
#define DATA_AREA_SIZE			(0x1000000)		// 16M ��������ʹ��
#define DATA_AREA_MAP_SIZE      (DATA_AREA_SIZE/SECTOR)		// ������ӳ���ļ����� 
#define MAX_TRANS_SEC_NUM       (0x80)			// �����������������

#define MAX_DMA_NUM				(0x10)			// DMA����
#define BYTE_STRING_LEN         (2)             // ÿ���ַ���ʾ ���ֽ� ��ռ����

#define CMD_PHASE_OFS_LEN		(0x12)			// ��λ����ռ����

#define CMD_BLK_CMDIDX			(0)				// �����������λ��
#define CMD_BLK_ADDRIDX			(2)				// ������е�ַ��ʼλ��
#define CMD_BLK_LENIDX			(7)				// ������д��䳤�ȿ�ʼλ��


#define SMALL_AREA_SIZE         (0x40000)                   // 256K ��С����ʹ��

UINT  AFX_CDECL BusHoundDecodeThread(LPVOID lpParam);

struct COMMAND_INFO
{
	DWORD dmaIdx;
	WORD sectorCnt;
	DWORD addr;
	BOOL direction;		// 0:IN/1:OUT
	TCHAR cmdPhaseOfs[CMD_PHASE_OFS_LEN+1];
};

// CBusHoundCompareDlg �Ի���
class CBusHoundCompareDlg : public CDialogEx
{
// ����
public:
	CBusHoundCompareDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_BUSHOUNDCOMPARE_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	CEdit m_editDataPath;
	afx_msg void OnBnClickedBtnSelectpath();
	afx_msg void OnBnClickedBtnCompare();
	afx_msg void OnClose();
	afx_msg void OnDropFiles(HDROP hDropInfo);
//	afx_msg int OnCreate(LPCREATESTRUCT);


public:
	CWinThread	*m_lpDecodeThread;

	UINT m_nCmdPhaseOfsPoint;		// ����״̬ƫ��λ��
	UINT m_nPhaseStartPoint;		// ״̬��ʼλ��
	UINT m_nDataStartPoint;			// ���ݿ�ʼλ��
	UINT m_nDataLen;				// ������ԭʼ�ļ�����ռ����

public:
    DWORD	DecodeThread();

private:
	// Դ�ļ���ز���
	CString m_strSrcPath;
	HANDLE  m_hSrcFileMap;
	DWORD m_dwSrcBlkSize;
	LPBYTE m_lpSrcMapAddress;

	// Ŀ���ļ���ز���
    CString m_strDstPath;
    HANDLE  m_hDstFileMap;
    DWORD m_dwDstBlkSize;
    LPBYTE m_lpDstMapAddress;
	DWORD m_BlkIdx;


	CMutex m_Mutex;
	UINT m_err;
	DWORD m_Granularity;
	
	BOOL m_bRun;		// �߳��������
	BOOL m_bEnd;		// �߳̽������
	BOOL m_bStop;		// �߳�ֹͣ���
	BOOL m_bCompareStart;	

	__int64 m_nSrcFileSize;	
    __int64 m_nDstFileSize;
	CString m_strResidualData;


	BYTE m_ucCmdData[CBW_MAX_LEN];
	BYTE *m_lpucSecotrData[MAX_DMA_NUM];
	DWORD m_DMAMask;								// DMA ����
	BYTE *m_lpucSysArea;							// 32M ��ϵͳ��ʹ��
	BYTE *m_lpucDataArea;							// 16M ��������ʹ��
	UINT m_DataFlag;

    BYTE *m_lpSmallSecArea;                         // ��С����ʹ��

	UINT m_PhaseType;							// 0:����״̬/1:����״̬/2:����״̬
	UINT m_DmaIdx;								// DMA ����λ��
	UINT m_DataIdx;								// ��������λ��
	UINT m_CBWIdx;								// ��������λ��

	BOOL m_bStartWriteFlag;						// �Ƿ���д����

    vector<WORD> *m_lpSmallAreaMap;             // ����ӳ�䷶Χ 32M
    vector<WORD> *m_lpDstFileMap;               // 

	vector<DWORD> *m_DataAreaMap;
	queue<COMMAND_INFO> m_CommandInfo;


    WORD    m_SmallAreaIdx;
    WORD    m_DstFileIdx;
    map<DWORD, WORD>    m_SmallAreaMap;
    map<DWORD, WORD>    m_DstFileMap;
    map<DWORD, WORD>    m_DstSecMap;

private:
	CString GetCurrentPath();

	BOOL SetErrCode(UINT uErr);
	UINT GetErrCode();
	BOOL GetRunFlag();
	BOOL SetRunFlag(BOOL  runFlag);
	BOOL GetEndFlag();
	BOOL SetEndFlag(BOOL  endFlag);
	BOOL GetStopFlag();
	BOOL SetStopFlag(BOOL  stopFlag);
	BOOL GetCompareStartFlag();
	BOOL SetCompareStartFlag(BOOL  startFlag);
	UINT GetDataFlag();
	BOOL SetDataFlag(UINT  dataFlag);
	BOOL GetDMAIdxMask(DWORD dmaIdx);
	BOOL SetDMAIdxMask(DWORD dmaIdx, BOOL maskFlag);

	HANDLE CreateUserFileMapping(CString strPath, __int64 &fileSize);

	DWORD GetMappingBlkSize(__int64 fileSize);
	DWORD CreateDecodeThread();
	void DestroyDecodeThread();
	BOOL MappingVirtualMemory();
	VOID CreateWorkThread();
	DWORD GetAllocationGranularity();
	void InitialParam();

	BOOL	CreateMapAddr(HANDLE hFileMap, __int64 &fileOffset, DWORD blkSize, LPBYTE &mapAddr);
	BOOL	DistroyMapAddr(LPBYTE &mapAddr);
	BOOL	GetDataOffset(__int64 fileOffset, UINT blkOffset);
	void	GetDataStartPoint(CString &strLine);
	void	CheckDataStartPoint(CString &strLine);
    BOOL    GetDstFileSize(CString &strLine);
	BOOL	AddDisplay(LPCTSTR str);

	CString  FindLine(LPBYTE  pByte, UINT & uiIndex, UINT uiLen);
	BYTE  StringToByte(CString &strChar);
	DWORD ReverseDWORD(DWORD InData);
	WORD ReverseWORD(WORD InData);

	BOOL CommandDecodeFlow(CString &strLine);
	BOOL DataDecodeFlow(CString &strLine);
	BOOL ExistedWriteFlag();
    BOOL PseudoWriteData(DWORD addr, WORD secCnt, DWORD dmaIdx);
    BOOL PseudoWriteData_Ex(DWORD addr, WORD secCnt, DWORD dmaIdx, TCHAR *cmdPhaseOfs);
    BOOL PseudoReadData(DWORD addr, WORD secCnt, DWORD dmaIdx, TCHAR *cmdPhaseOfs);
    BOOL PseudoReadData_Ex(DWORD addr, WORD secCnt, DWORD dmaIdx, TCHAR *cmdPhaseOfs);
	void ShowErrInfo(DWORD addr, TCHAR *cmdPhaseOfs);
	void ShowMissInfo(DWORD addr, TCHAR *cmdPhaseOfs);
    void ShowOverflowInfo(DWORD addr, TCHAR *cmdPhaseOfs);

	BOOL    GetFileAttribute();
	BOOL    CreateDstFile();
	BOOL	AdjustFileMap(WORD idx, __int64 &qwFileOffset);
    BOOL    AddNewMapData(DWORD addr, WORD secCnt, DWORD dmaIdx);

public:
	CListBox m_listShowStatus;	
	CProgressCtrl m_progDecode;
};
