/*
 * Loger.h
 *
 *  Created on: Jul 14, 2014
 *      Author: alex
 */

#ifndef LOGER_H_
#define LOGER_H_

EXPORTNIX void PrintF(const char* pMessage, ...);


#endif /* LOGER_H_ */

#ifndef TRACE
#ifndef NDEBUG
#define TRACE(...) PrintF(__VA_ARGS__)
#else
#define TRACE(...)
#endif
#endif
