// Log.cpp: implementation of the CLog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Log.h"

CLog *CLog::instance_ = 0;

#ifdef TURN_LOGGING_ON
  const bool logging = true;
#else
  const bool logging = false;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLog::
CLog()
{
    if (logging || getenv("HPCOMM_SERIAL") != 0)
        serialName_ = "HPComm-s.log";

    if (logging || getenv("HPCOMM_PROGRAM") != 0)
        programName_ = "HPComm-p.log";
}


CLog::
~CLog()
{
}


bool CLog::
Serial(const char *fmt, ...)
{
    va_list ap;
    char buff[1000];
    
    if (instance_ == 0)
        instance_ = new CLog;

    if (instance_->serialName_.IsEmpty())
        return true;

    // Log the message
    if (fmt != 0)
    {
        va_start(ap, fmt);
        vsprintf(buff, fmt, ap);
        va_end(ap);

        return instance_->writeToFile(instance_->serialName_, buff);
    }        

    return true;
}


bool CLog::
Program(const char *fmt, ...)
{
    va_list ap;
    char buff[1000];
    
    if (instance_ == 0)
        instance_ = new CLog;

    if (instance_->programName_.IsEmpty())
        return true;

    // Log the message
    if (fmt != 0)
    {
        va_start(ap, fmt);
        vsprintf(buff, fmt, ap);
        va_end(ap);

        return instance_->writeToFile(instance_->programName_, buff);
    }        

    return true;
}


bool CLog::
writeToFile(const CString &name, const CString &msg)
{
#ifdef _DEBUG
    TRACE("%s\n", msg);
#endif

    FILE *logfile;
    if ((logfile = fopen(name, "a+")) == 0)
    {
        CString msg = "Can't open log file '" + name + "'.";
        AfxMessageBox(msg, MB_ICONERROR, 0);

        return false;
    }

    SYSTEMTIME st;
    GetLocalTime(&st);

    fprintf(logfile, "%4d%02d%02d %02d:%02d:%02d.%03d %s\n", 
                st.wYear, st.wMonth, st.wDay,
                st.wHour, st.wMinute, st.wSecond, st.wMilliseconds,
                msg);

    fclose(logfile);

    return true;
}
