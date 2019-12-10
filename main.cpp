/* -*- C++ -*-
 * File: 4channels.cpp
 * Copyright 2008-2019 LibRaw LLC (info@libraw.org)
 * Created: Mon Feb 09, 2009
 *
 * LibRaw sample
 * Generates 4 TIFF file from RAW data, one file per channel
 *

LibRaw is free software; you can redistribute it and/or modify
it under the terms of the one of two licenses as you choose:

1. GNU LESSER GENERAL PUBLIC LICENSE version 2.1
   (See file LICENSE.LGPL provided in LibRaw distribution archive for details).

2. COMMON DEVELOPMENT AND DISTRIBUTION LICENSE (CDDL) Version 1.0
   (See file LICENSE.CDDL provided in LibRaw distribution archive for details).


 */
#include <stdio.h>
#include <string.h>
#include <math.h>

#ifndef WIN32

#include <netinet/in.h>

#else
#include <winsock2.h>
#endif

#include "libraw/libraw.h"

#ifdef WIN32
#define snprintf _snprintf
#endif

int main(int ac, char *av[]) {
    int i, ret;
    int autoscale = 0, black_subtraction = 1, use_gamma = 1;
    char outfn[1024];

    LibRaw RawProcessor;

#define P1 RawProcessor.imgdata.idata
#define S RawProcessor.imgdata.sizes
#define C RawProcessor.imgdata.color
#define T RawProcessor.imgdata.thumbnail
#define P2 RawProcessor.imgdata.other
#define OUT RawProcessor.imgdata.params

    OUT.output_bps = 16;
    OUT.output_tiff = 1;
    OUT.user_flip = 0;
    OUT.no_auto_bright = 1;
    OUT.half_size = 1;

    i = 1;
    if (use_gamma)
        OUT.gamm[0] = OUT.gamm[1] = 1;

    int c;
    printf("Processing file %s\n", av[i]);
    if ((ret = RawProcessor.open_file(av[i])) != LIBRAW_SUCCESS) {
        fprintf(stderr, "Cannot open %s: %s\n", av[i], libraw_strerror(ret));
        return -1; // no recycle b/c open file will recycle itself
    }
    if (P1.is_foveon) {
        printf("Cannot process Foveon image %s\n", av[i]);
        return -1;
    }
    if ((ret = RawProcessor.unpack()) != LIBRAW_SUCCESS) {
        fprintf(stderr, "Cannot unpack %s: %s\n", av[i], libraw_strerror(ret));
        return -1;
    }
    RawProcessor.raw2image();
//        RawProcessor.subtract_black();


    // hack to make dcraw tiff writer happy
    int isrgb = (P1.colors == 4 ? 0 : 1);
    P1.colors = 1;
    S.width = S.iwidth;
    S.height = S.iheight;

    for (int layer = 0; layer < 4; layer++) {
        if (layer > 0) {
            for (int rc = 0; rc < S.iheight * S.iwidth; rc++)
                RawProcessor.imgdata.image[rc][0] = RawProcessor.imgdata.image[rc][layer];
        }
        char lname[8];
        if (isrgb) {
            snprintf(lname, 7, "%c", ((char *) ("RGBG"))[layer]);
            if (layer == 3)
                strcat(lname, "2");
        } else
            snprintf(lname, 7, "%c", ((char *) ("GCMY"))[layer]);

        if (OUT.shot_select)
            snprintf(outfn, sizeof(outfn), "%s-%d.%s.tiff", av[i], OUT.shot_select, lname);
        else
            snprintf(outfn, sizeof(outfn), "%s.%s.tiff", av[i], lname);

        printf("Writing file %s\n", outfn);
        if (LIBRAW_SUCCESS != (ret = RawProcessor.dcraw_ppm_tiff_writer(outfn)))
            fprintf(stderr, "Cannot write %s: %s\n", outfn, libraw_strerror(ret));
    }
    return 0;
}
