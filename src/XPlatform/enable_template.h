#pragma once

#ifdef _MSC_VER
#define TRISTEON_ENABLE_TEMPLATE typename = 
#elif defined(__GNUC__)
#define TRISTEON_ENABLE_TEMPLATE 
#endif