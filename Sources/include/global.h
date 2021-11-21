
#pragma once

#include <sstream>
#include <string>
#include <cassert>
#include <cstdint>

#ifndef loop
#define loop(a, v) for (uint32_t a = 0; a < static_cast<uint32_t>(v); a++)
#define loopi(v) loop(i, v)
#define loopj(v) loop(j, v)
#endif

void Log(const char *str);
void CheckError(const char* file, unsigned int line);

#define GL_CHECK_ERROR CheckError(__FILE__, __LINE__)

#define PV_MSG(msg)                             \
{                                               \
    std::stringstream __str;                    \
    __str << msg;                               \
    Log(__str.str().c_str());                   \
}

#define PV_INFO(expr)     { PV_MSG("[INFO] " << expr); }
#define PV_WARNING(expr)  { PV_MSG("[WARNING] " << expr); }
#define PV_ERROR(expr)    { PV_MSG("[ERROR] " << expr); }
#define PV_FATAL(expr)    { PV_ERROR(expr); assert(false); }

#ifdef _DEBUG
#define PV_ASSERT_MSG(expression, msg)    { if (!(expression)) { PV_ERROR(msg); assert(expression); } }
#define PV_ASSERT(expression)             PV_ASSERT_MSG(expression, #expression)
#else
#define PV_ASSERT(expression)
#define PV_ASSERT_MSG(expression, msg)
#endif
