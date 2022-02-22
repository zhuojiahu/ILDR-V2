/*标志内存检测报告：setdebugnew.h
注意，只有在cpp中添加setdebugnew.h，而且有new操作的那些函数或者类被调用，才会进行内存泄露跟踪*/
#ifndef SET_DEBUG_NEW_H
#define SET_DEBUG_NEW_H

#ifdef _DEBUG
#define DEBUG_CLIENTBLOCK   new( _CLIENT_BLOCK, __FILE__, __LINE__)
#else
#define DEBUG_CLIENTBLOCK
#endif

#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

#ifdef _DEBUG
#define new DEBUG_CLIENTBLOCK
#endif

#endif