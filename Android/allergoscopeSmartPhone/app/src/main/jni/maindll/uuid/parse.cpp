/*
 * parse.c --- UUID parsing
 * 
 * Copyright (C) 1996, 1997 Theodore Ts'o.
 *
 * %Begin-Header%
 * This file may be redistributed under the terms of the GNU Public
 * License.
 * %End-Header%
 */

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "uuidP.h"

typedef unsigned short int WORD;
typedef unsigned int DWORD;

WORD convertToWord1(char *buff);

DWORD convertToDWord1(char *buff);

int uuid_parse(char *in, uuid_t uu)
{
    struct uuid uuid;
    int i;
    char *cp, buf[3];
    DWORD first;
    WORD second, third;

    if (strlen(in) != 36)
        return -1;
    for (i = 0, cp = in; i <= 36; i++, cp++)
    {
        if ((i == 8) || (i == 13) || (i == 18) ||
            (i == 23))
            if (*cp == '-')
                continue;
        if (i == 36)
            if (*cp == 0)
                continue;
        if (!isxdigit(*cp))
            return -1;
    }

    first = strtoul(in, NULL, 16);
    second = strtoul(in + 9, NULL, 16);
    third = strtoul(in + 14, NULL, 16);

    uuid.time_low = convertToDWord1((char *) &first);
    uuid.time_mid = convertToWord1((char *) &second);
    uuid.time_hi_and_version = convertToWord1((char *) &third);

    uuid.clock_seq = strtoul(in + 19, NULL, 16);
    cp = in + 24;
    buf[2] = 0;
    for (i = 0; i < 6; i++)
    {
        buf[0] = *cp++;
        buf[1] = *cp++;
        uuid.node[i] = strtoul(buf, NULL, 16);
    }

    uuid_pack(&uuid, uu);
    return 0;
}
