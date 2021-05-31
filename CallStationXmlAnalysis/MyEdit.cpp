// MyEdit.cpp : ʵ���ļ�
//
 
#include "stdafx.h"
#include "MyEdit.h"
#include "PublicFunDef.h"
#include <fstream> 
using namespace std;
// CMyEdit
 
IMPLEMENT_DYNAMIC(CMyEdit, CEdit)
 
CMyEdit::CMyEdit():m_nFlag(EDIT_NONE_JUDGE)
{
	
}
 
CMyEdit::~CMyEdit()
{
}
 
 
BEGIN_MESSAGE_MAP(CMyEdit, CEdit)
	ON_WM_DROPFILES()
END_MESSAGE_MAP()
 
 
// CMyEdit ��Ϣ�������
 
void CMyEdit::OnDropFiles(HDROP hDropInfo)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if(hDropInfo)
	{
		//��ק�ļ�������
		UINT nDrag = DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0);
 
		//ֻ��קһ���ļ�
		if (nDrag > 1 && (m_nFlag & EDIT_SIG_JUDGE))
		{
			nDrag = 1;
		}
 
		// ����ק���ļ����ļ���
		TCHAR* szItemPath = new TCHAR[MAX_PATH + 1]();
 
		CString strAllPath;
		for (UINT i = 0; i < nDrag; ++i)
		{
			memset(szItemPath, 0, sizeof(TCHAR) * (MAX_PATH + 1));
			DragQueryFile(hDropInfo, i, szItemPath, MAX_PATH);
			if (PathFileExists(szItemPath) == TRUE)
			{
				//���������ж�
				if ((m_nFlag & EDIT_DIR_JUDGE) && PathIsDirectoryA(szItemPath))
				{
					strAllPath += szItemPath;
				}
				else if ((m_nFlag & EDIT_FILE_JUDGE) && !PathIsDirectoryA(szItemPath))
				{
					strAllPath += szItemPath;
				}
				else if (m_nFlag == EDIT_NONE_JUDGE)
				{
					strAllPath += szItemPath;
				}
 
				//��һ����Ŀ��ʱ��Ӽ����
				if (nDrag > 1)
				{
					strAllPath += ";";
				}
			}
			
			//SetWindowText(strAllPath);
		}
		SetWindowText("");
		vector<CString> vecSplitPath;
		vecSplitPath = split(strAllPath, ";");
		ifstream myfile(vecSplitPath[0]);
		string temp;
		while (getline(myfile, temp))
		{
			CString strTemp = "";
			strTemp.Format("%s\r\n", temp.c_str());
			if (IncludeChinese(strTemp.GetBuffer()))
			{
				strTemp = transcoding2utf8(strTemp.GetBuffer()).c_str();
			}
			SetSel(GetWindowTextLength(), GetWindowTextLength());
			ReplaceSel(strTemp);
			strTemp.ReleaseBuffer();
		}
		myfile.close();
		delete[] szItemPath;
		szItemPath = nullptr;
		
	}
	CEdit::OnDropFiles(hDropInfo);
}