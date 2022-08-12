#include "stdafx.h"
#include "Project.h"
#include "atlpath.h"
#include <cmath>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CWinApp theApp;

using namespace std;

CString GetHomeDirectory()
{
	TCHAR sFilename[_MAX_PATH];
	TCHAR sDrive[_MAX_DRIVE];
	TCHAR sDir[_MAX_DIR];
	TCHAR sFname[_MAX_FNAME];
	TCHAR sExt[_MAX_EXT];
	GetModuleFileName(AfxGetInstanceHandle(), sFilename, _MAX_PATH);
	_tsplitpath(sFilename, sDrive, sDir, sFname, sExt);

	CString rVal(CString(sDrive) + CString(sDir));
	int nLen = rVal.GetLength();
	if (rVal.GetAt(nLen - 1) != _T('\\'))
		rVal += _T("\\");

	return rVal;
};

extern double CalculationError(/*int argc, _TCHAR* argv[]*/)
{
	int nRetCode = 0;

	HMODULE hModule = ::GetModuleHandle(NULL);

	if (hModule != NULL)
	{
		if (!AfxWinInit(hModule, NULL, ::GetCommandLine(), 0))
		{
			_tprintf(_T("´íÎó:  MFC ³õÊ¼»¯Ê§°Ü\n"));
			nRetCode = 1;
		}
		else
		{
			// TODO:
		}
	}
	else
	{
		// TODO:
		_tprintf(_T("ERROR:  GetModuleHandle Fail\n"));
		nRetCode = 1;
	}


	//Hydrodynamic model operation
	CProject  project;
	project.ReadAndRunModel();


	//Calculate the hydrodynamic simulation error
	double ErrorAverage = 0;
	extern int nRow; extern double StageCompare[10000], CalculationResult[10000];
	for (int i = 0; i < nRow; i++)
	{
		ErrorAverage += abs(CalculationResult[i] - StageCompare[i]);
	}
	ErrorAverage /= nRow;

	return ErrorAverage;
}
