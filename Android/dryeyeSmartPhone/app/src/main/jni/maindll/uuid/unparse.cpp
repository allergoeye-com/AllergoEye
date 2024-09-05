/*
 * unparse.c -- convert a UUID to string
 * 
 * Copyright (C) 1996, 1997 Theodore Ts'o.
 *
 * %Begin-Header%
 * This file may be redistributed under the terms of the GNU Public
 * License.
 * %End-Header%
 */

#include <stdio.h>

#include "uuidP.h"

typedef unsigned short int WORD;
typedef unsigned int DWORD;

union regDW {
    struct {
	unsigned char x1;
	unsigned char x2;
	unsigned char x3;
	unsigned char x4;
    };
    DWORD y;
};

union regW {
    struct {
	unsigned char x1;
	unsigned char x2;
    };
    WORD y;
};

WORD convertToWord1(char *buff)
{
	union regW {
    struct {
	unsigned char x1;
	unsigned char x2;
    };
    WORD y;
} rw;
	WORD dw = 0;
	rw.x1 = buff[1];
	rw.x2 = buff[0];
	dw = rw.y;
	return dw;
}

DWORD convertToDWord1(char *buff)
{
	union regDW {
    struct {
	unsigned char x1;
	unsigned char x2;
	unsigned char x3;
	unsigned char x4;
    };
    DWORD y;
} rw;
	DWORD dw = 0;
	rw.x1 = buff[3];
	rw.x2 = buff[2];
	rw.x3 = buff[1];
	rw.x4 = buff[0];
	dw = rw.y;
	return dw;
}


WORD convertToWord(char *buff)
{
	WORD dw = 0;
	dw = buff[0];
	dw <<= 8;
	dw |= buff[1];
	return dw;
}

DWORD convertToDWord(char *buff)
{
	DWORD dw = 0;
	for (int i = 0; i <= 3; ++i) 
	{
		dw |= buff[i];
		if (i != 3)
			dw <<= 8;
	}
	return dw;
}

void uuid_unparse(uuid_t uu, char *out)
{
	struct uuid uuid;
	uuid_unpack(uu, &uuid);
	DWORD first = convertToDWord1((char *)&uuid.time_low);
	WORD second = convertToWord1((char *)&uuid.time_mid);
	WORD third = convertToWord1((char *)&uuid.time_hi_and_version);
	sprintf(out,
		"%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
		first, second, third,
		
		uuid.clock_seq >> 8, uuid.clock_seq & 0xFF,		
		uuid.node[0], uuid.node[1], uuid.node[2],
		uuid.node[3], uuid.node[4], uuid.node[5]);
}

