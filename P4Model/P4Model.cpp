// P4Model.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "P4Model.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Ψһ��Ӧ�ó������

CWinApp theApp;

using namespace std;

class CPBlaze4Model gPBlaze4Model;
int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	HMODULE hModule = ::GetModuleHandle(NULL);

	if (hModule != NULL)
	{
		// ��ʼ�� MFC ����ʧ��ʱ��ʾ����
		if (!AfxWinInit(hModule, NULL, ::GetCommandLine(), 0))
		{
			// TODO: ���Ĵ�������Է���������Ҫ
			_tprintf(_T("����: MFC ��ʼ��ʧ��\n"));
			nRetCode = 1;
		}
		else
		{
			// TODO: �ڴ˴�ΪӦ�ó������Ϊ��д���롣
			gPBlaze4Model.m_fnStartUp(argc, argv);
		}
	}
	else
	{
		// TODO: ���Ĵ�������Է���������Ҫ
		_tprintf(_T("����: GetModuleHandle ʧ��\n"));
		nRetCode = 1;
	}

	return nRetCode;
}

/*---------------------------------------------------------------------------*/
//
//
void CPBlaze4Model::m_fnStartUp(int argc, TCHAR* argv[])
{
	std::cout << "PBlaze4 Start Run..." << std::endl;

	return;
}


CPBlaze4Model::CPBlaze4Model()
{

}

CPBlaze4Model::~CPBlaze4Model()
{

}

