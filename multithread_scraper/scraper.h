#ifndef SCRAPER_H
#define SCRAPER_H

#include <pthread.h>

/* Maximum lengths for URLs and output filenames */
#define MAX_URL_LENGTH      1024
#define MAX_FILENAME_LENGTH 256

/* Data passed to each thread */
typedef struct {
    char url[MAX_URL_LENGTH];
    int index;  /* index of this URL (used to name the output file) */
} ThreadData;

/*
 * Thread routine:
 *  - Takes a ThreadData* as argument
 *  - Downloads the HTML content of the URL
 *  - Saves it to "page_<index>.html"
 */
void *fetch_url(void *arg);

#endif /* SCRAPER_H */