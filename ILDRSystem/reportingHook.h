/*内存泄露报告过滤器头文件，将不是"*.cpp"格式的报告过滤掉*/
#pragma once

#if defined(WIN32)
void setFilterDebugHook(void);
#endif 