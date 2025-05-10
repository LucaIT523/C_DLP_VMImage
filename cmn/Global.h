#pragma once

#define GD_LOG_LEVEL_ALL		(0XFFFFFFFF)
#define GD_LOG_LEVEL_BASE		(0x00000001)
#define GD_LOG_LEVEL_NORMAL		(GD_LOG_LEVEL_BASE<<0)
#define GD_LOG_LEVEL_ERROR		(GD_LOG_LEVEL_BASE<<1)
#define GD_LOG_LEVEL_WARN		(GD_LOG_LEVEL_BASE<<2)

#define GD_LOG_AREA_ALL			(0XFFFFFFFF)
#define GD_LOG_AREA_BASE		(0x00000001)
#define GD_LOG_AREA_S_INIT		(GD_LOG_LEVEL_BASE<<0)
#define GD_LOG_AREA_S_FILE		(GD_LOG_LEVEL_BASE<<1)
#define GD_LOG_AREA_S_PROC		(GD_LOG_LEVEL_BASE<<2)
#define GD_LOG_AREA_S_PRINT		(GD_LOG_LEVEL_BASE<<3)
#define GD_LOG_AREA_S_CHROME	(GD_LOG_LEVEL_BASE<<4)
#define GD_LOG_AREA_C_INIT		(GD_LOG_LEVEL_BASE<<5)
#define GD_LOG_AREA_C_HOOK		(GD_LOG_LEVEL_BASE<<6)
#define GD_LOG_AREA_LAUNCHER	(GD_LOG_LEVEL_BASE<<7)
#define GD_LOG_AREA_SERVICE		(GD_LOG_LEVEL_BASE<<8)
#define GD_LOG_AREA_AGENT		(GD_LOG_LEVEL_BASE<<9)
#define GD_LOG_AREA_VD			(GD_LOG_LEVEL_BASE<<10)

void LogPrintW(unsigned int p_nLogLevel, unsigned int p_nLogArea, const wchar_t* p_wszFmt, ...);
void LogPrintA(unsigned int p_nLogLevel, unsigned int p_nLogArea, const char* p_wszFmt, ...);
