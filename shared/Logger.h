#pragma once
#include <stdio.h>
#include <stdarg.h>


inline void Log(char *format, ...)
{
	va_list argptr;
	va_start(argptr, format);
	vfprintf(stdout, format, argptr);
	va_end(argptr);
}


inline void LogInDebugOnly(char *format, ...)
{
#ifdef _DEBUG
	va_list argptr;
	va_start(argptr, format);
	vfprintf(stdout, format, argptr);
	va_end(argptr);
#endif
}
