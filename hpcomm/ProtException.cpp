// ProtException.cpp: implementation of the CProtException class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "hpcomm.h"
#include "ProtException.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CProtException::CProtException(CProt::ERRCODE code)
{
	error = code;
}

CProtException::~CProtException()
{

}
