#include <stdio.h>
#include <stdint.h>
#include <string.h>

static uint32_t crctbl[256];

int main()
{
    uint32_t I[16];                 /* indexes */
    uint32_t e = 8;                 /* # of error bits */
    uint32_t i, j;
    uint64_t d;
    uint32_t crc;
    uint64_t ptrn = 0;              /* # of patterns */
    uint32_t fail = 0;              /* # of failures */
    uint32_t polynomial = 0x04C11DB7;  /* default polynomial for CRC32 */

    // Prompt and read polynomial
    char input[50];
    printf("Enter polynomial (default is 0x04C11DB7): ");
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = '\0';  // remove newline
    if(strlen(input) > 0) {
        sscanf(input, "%x", &polynomial);
    }

    for(j = 0; j < 256; j++){ /* generate table for CRC32 */
        crc = j << 24;
        for (i = 0; i < 8; i++) {
            if (crc & 0x80000000)
                crc = (crc << 1) ^ polynomial;
            else
                crc <<= 1;
        }
        crctbl[j] = crc;
    }

    for(i = 0; i < e; i++)      /* init I */
        I[i] = i;

    while(1){
        d = 0;                  /* set up pattern */
        for(i = 0; i < e; i++)
            d |= ((uint64_t)1 << (63-I[i]));

        crc = 0xFFFFFFFF;       /* initialize CRC32 value */
        for(j = 0; j < 8; j++){ /* generate crc */
            crc = (crc << 8) ^ crctbl[((crc >> 24) ^ (d >> (56 - j*8)) & 0xFF)];
        }
        crc = ~crc;             /* finalize CRC32 value */

        ptrn++;

        if (crc == 0) {         /* if failed */
            fail++;

            // Print binary representation of the pattern
            printf("Undetected Error: Pattern= %llu (", d);
            for (int b = 63; b >= 0; b--) {
                printf("%d", (d & ((uint64_t)1 << b)) ? 1 : 0);
            }
            printf("), CRC= %u\n", crc);
        }

        i = e - 1;              /* advance indexes */
        if (++I[i] < 64)        /*  to next permutation */
            continue;
        --I[i];
        j = 63;
        while (I[i--] >= j--) {
            if (i == 0)         /* show progress */
                printf("Progress: %u\n", I[0]);
            if (i == (uint32_t)(0-1))
                goto done0;
        }
        j = I[++i];
        while (i < e)
            I[i++] = ++j;
    }

done0:
    printf("Total Patterns: %llu, Total Failures: %u\n", ptrn, fail);
    return 0;
}

