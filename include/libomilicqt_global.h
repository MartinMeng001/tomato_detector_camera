#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(LIBOMILICQT_LIB)
#  define LIBOMILICQT_EXPORT Q_DECL_EXPORT
# else
#  define LIBOMILICQT_EXPORT Q_DECL_IMPORT
# endif
#else
# define LIBOMILICQT_EXPORT
#endif
