
// searchMapDlg.h: 头文件
//

#pragma once


// CsearchMapDlg 对话框
class CsearchMapDlg : public CDialogEx
{
// 构造
public:
	CsearchMapDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SEARCHMAP_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	CToolTipCtrl m_tipBtnCtr;
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	int RePaint();
	afx_msg void OnAs32777();
	afx_msg void On232775();
	afx_msg void On232776();
	afx_msg void OnAscsdtdr32780();
	afx_msg void OnAscsdtdr32781();
	afx_msg void OnAscsdtdr32792();
	afx_msg void OnAscsdtdr32793();
	afx_msg void OnAscsdr32784();
	afx_msg void OnAscsdr32785();
	afx_msg void OnAscsdr32786();
	afx_msg void OnAscssrdr32787();
	afx_msg void OnAscssrdr32788();
	afx_msg void OnAscssrdr32789();
	afx_msg void OnAscssr32794();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnAscs32796();
};
