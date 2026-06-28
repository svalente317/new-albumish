/*
 * mp3_duration.c
 *
 * Get the duration of an MP3 file using libmpg123 without decoding the
 * entire file.  The strategy is:
 *
 *   1. Open the file with mpg123_open().
 *   2. Read just enough frames to let libmpg123 parse the first valid header
 *      (mpg123_scan() is intentionally NOT called – that would decode every
 *      frame).
 *   3. If a Xing/Info/VBRI tag is present libmpg123 fills in the frame-count
 *      automatically; for a CBR file the frame count is estimated from the
 *      file size and the bit-rate found in the first header.
 *   4. Convert frame count → seconds using the samples-per-frame and
 *      sample-rate values from the parsed header.
 *
 * Compile:
 *   gcc -O2 -Wall -o mp3_duration mp3_duration.c -lmpg123
 *
 * Usage:
 *   ./mp3_duration <file.mp3>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <mpg123.h>

/*
 * Return the file size in bytes, or -1 on error.
 */
static off_t file_size(const char *path)
{
    struct stat st;
    if (stat(path, &st) != 0)
        return -1;
    return st.st_size;
}

/*
 * Attempt to obtain duration without a full scan.
 *
 * Returns duration in seconds (>= 0.0) on success, or < 0.0 on error.
 */
double mp3_duration_fast(const char *path)
{
    mpg123_handle *mh = NULL;
    int err;
    double duration = -1.0;

    /* ------------------------------------------------------------------ */
    /* 1. Initialise library and open file                                 */
    /* ------------------------------------------------------------------ */
    if (mpg123_init() != MPG123_OK)
        return -1.0;

    mh = mpg123_new(NULL, &err);
    if (!mh) {
        fprintf(stderr, "mpg123_new: %s\n", mpg123_plain_strerror(err));
        goto cleanup;
    }

    /*
     * Do NOT call mpg123_scan() – that forces a full file traversal.
     * MPG123_SKIP_ID3V2 avoids spending time on large artwork tags.
     */
    mpg123_param(mh, MPG123_ADD_FLAGS, MPG123_SKIP_ID3V2, 0);

    if (mpg123_open(mh, path) != MPG123_OK) {
        fprintf(stderr, "mpg123_open: %s\n", mpg123_strerror(mh));
        goto cleanup;
    }

    /* ------------------------------------------------------------------ */
    /* 2. Parse the first valid frame header                               */
    /*    mpg123_getformat() pumps just enough data to find the header.    */
    /* ------------------------------------------------------------------ */
    long rate = 0;
    int channels = 0, encoding = 0;
    if (mpg123_getformat(mh, &rate, &channels, &encoding) != MPG123_OK) {
        fprintf(stderr, "mpg123_getformat: %s\n", mpg123_strerror(mh));
        goto cleanup;
    }

    /* ------------------------------------------------------------------ */
    /* 3. Try to get the frame count from the mpg123 index / Xing tag     */
    /*                                                                     */
    /*    mpg123_length() returns the number of PCM *samples* if the      */
    /*    library already knows it (from a Xing/Info/VBRI header).        */
    /*    We use that when available; otherwise we fall back to a CBR     */
    /*    estimate derived from the file size and bit-rate.               */
    /* ------------------------------------------------------------------ */
    off_t pcm_samples = mpg123_length(mh);

    if (pcm_samples > 0) {
        /* Lossless path: Xing/VBR header or accurate frame index */
        duration = (double)pcm_samples / (double)rate;
    } else {
        /*
         * CBR fallback: estimate from file size.
         *
         * bit-rate (bps) comes from mpg123_info().  Frame size and
         * samples-per-frame are implicit from the MPEG version and layer.
         *
         * For MPEG-1 Layer III: 1152 samples/frame
         * For MPEG-2/2.5 Layer III:  576 samples/frame
         *
         * Duration ≈ file_size_bits / bit_rate
         * (We subtract an estimate for the ID3 tag size to be tidy, but
         *  even without that the error is usually < 0.1 s.)
         */
        struct mpg123_frameinfo fi;
        memset(&fi, 0, sizeof(fi));

        if (mpg123_info(mh, &fi) != MPG123_OK || fi.bitrate <= 0) {
            fprintf(stderr, "Cannot determine bit-rate; giving up.\n");
            goto cleanup;
        }

        off_t fsize = file_size(path);
        if (fsize <= 0) {
            fprintf(stderr, "Cannot stat file.\n");
            goto cleanup;
        }

        /* Skip over the ID3v2 tag that sits at the start of the file */
        off_t id3v2_size = 0;
        mpg123_id3v2 *v2 = NULL;
        mpg123_id3v1 *v1 = NULL;
        if (mpg123_id3(mh, &v1, &v2) == MPG123_OK && v2) {
            /*
             * libmpg123 doesn't expose the raw tag length directly, but
             * we can compute it: the tag occupies everything before the
             * first sync word.  A reasonable upper bound is to use the
             * current file position after getformat(), which has already
             * skipped the tag.
             */
            off_t pos = mpg123_tell(mh);
            if (pos > 0)
                id3v2_size = pos;
        }

        off_t audio_bytes = fsize - id3v2_size;
        if (audio_bytes <= 0)
            audio_bytes = fsize;

        /* duration = audio_bytes * 8 / bitrate_bps */
        duration = ((double)audio_bytes * 8.0) / ((double)fi.bitrate * 1000.0);
    }

cleanup:
    if (mh) {
        mpg123_close(mh);
        mpg123_delete(mh);
    }
    mpg123_exit();
    return duration;
}

/* ------------------------------------------------------------------ */
/* main                                                                */
/* ------------------------------------------------------------------ */
int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <file.mp3>\n", argv[0]);
        return 1;
    }

    double dur = mp3_duration_fast(argv[1]);
    if (dur < 0.0) {
        fprintf(stderr, "Failed to determine duration.\n");
        return 1;
    }

    int h  = (int)(dur / 3600);
    int m  = (int)((dur - h * 3600) / 60);
    double s = dur - h * 3600 - m * 60;

    printf("Duration: %.3f seconds", dur);
    if (h > 0)
        printf("  (%d:%02d:%05.2f)\n", h, m, s);
    else
        printf("  (%d:%05.2f)\n", m, s);

    return 0;
}
