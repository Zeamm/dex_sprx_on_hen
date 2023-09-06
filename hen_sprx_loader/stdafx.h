
#pragma once

#include <cellstatus.h>
#include <sys/prx.h>
#include <string.h>
#include <limits.h>
#include <np.h>
#include <fastmath.h>
#include <sys/sys_time.h>
#include <sys/time_util.h>
#include <sys/prx.h>
#include <sys/ppu_thread.h>
#include <sys/timer.h>
#include <sys/process.h>
#include <sysutil/sysutil_msgdialog.h>
#include <sysutil/sysutil_oskdialog.h>
#include <sysutil/sysutil_userinfo.h>
#include <cell/sysmodule.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netex/net.h>
#include <netex/errno.h>
#include <netex/libnetctl.h>
#include <netex/sockinfo.h>
#include <sys/socket.h>
#include <cell\pad.h>
#include <np.h>

CDECL_BEGIN
int _sys_printf(const char *format, ...);
int _sys_vsprintf(char *s, const char *format, va_list arg);
CDECL_END

#include "stuffs.hpp"


#pragma hdrstop
