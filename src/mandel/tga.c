/* TGA image file creation */

#include <string.h>
#include <stdio.h>              /* standard I/O */
#include <stdint.h>             /* C99 integer types */
#include <inttypes.h>           /* C99 integer format strings */

#include "tga.h"                /* TGA interface header */

bool
write_file(char *name, uint8_t *pic, uint32_t height, uint32_t width) {
    printf("writing %" PRIu32 " bytes into %s\n", (height * width * 3), name);

    // Variables used to place correct resolution into header[]
    uint32_t height_first;
    uint32_t height_second;
    uint32_t width_first;
    uint32_t width_second;

    uint32_t temp = 0;

//    printf("\nHEIGHT: %d\n", height);
//    printf("WIDTH: %d\n", width);

    // width_first calculation
    temp = width & 0xFF00;
    temp = temp >> 8;
    temp = temp & 0x00FF;
    width_first = temp;

    // width_second calculation
    temp = width & 0x00FF;
    width_second = temp;

    // height_first calculation
    temp = height & 0xFF00;
    temp = temp >> 8;
    temp = temp & 0x00FF;
    height_first = temp;

    // height_second calculation
    temp = height & 0x00FF;
    height_second = temp;

//    printf("Width_first = %x\n", width_first);
//    printf("Width_second = %x\n", width_second);
//
//    printf("Height_first = %x\n", height_first);
//    printf("Height_second = %x\n", height_second);

    uint8_t header[18] = {0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, width_second, width_first, height_second,
                          height_first, 0x18, 0x00};

    FILE *fp = fopen(name, "w");
    if (fp == NULL)
        return 0;

    fwrite(header, sizeof(uint8_t), 18, fp);
    fwrite(pic, sizeof(uint8_t), (height * width * 3), fp);

    fclose(fp);

    return true;
}
