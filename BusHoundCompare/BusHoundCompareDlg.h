
// BusHoundCompareDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"

#define BLOCK_UNIT_SIZE			(0x400)			// �鵥Ԫ����
#define FAT_MAX_UINT_SIZE		(0x400)			// FAT��Ԥ�Ƶ�Ԫ����        
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

private:
	CString m_strDataPath;
	HANDLE  m_hSrcFileMap;
	CMutex m_Mutex;
	UINT m_err;
	DWORD m_dwBlkSize;

	__int64 m_nSrcFileSize;
private:
	CString GetCurrentPath();
	BOOL CompareData();
	VOID SetErrCode(UINT uErr);
	UINT GetErrCode();
	HANDLE CreateUserFileMapping(CString strPath, __int64 &fileSize);

	BOOL MappingDataFile();
	DWORD GetMappingBlkSize(__int64 srcFileSize);
	VOID CreateDecodeThread();
	BOOL MappingVirtualMemory();
	VOID CreateCompareThread();

};
