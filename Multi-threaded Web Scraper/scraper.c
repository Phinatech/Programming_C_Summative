#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "scraper.h"

/*
 * Thread function:
 *  - Initializes a CURL handle
 *  - Opens an output file "page_<index>.html"
 *  - Downloads the URL content into that file
 *  - Handles errors gracefully (CURL errors, file errors)
 */
void *fetch_url(void *arg)
{
    ThreadData *data = (ThreadData *)arg;
    CURL *curl_handle;
    CURLcode res;

    char filename[MAX_FILENAME_LENGTH];
    snprintf(filename, sizeof(filename), "page_%d.html", data->index);

    FILE *fp = fopen(filename, "w");
    if (!fp) {
        fprintf(stderr,
                "[Thread %d] Error: could not open file '%s' for writing.\n",
                data->index, filename);
        return NULL;
    }

    curl_handle = curl_easy_init();
    if (!curl_handle) {
        fprintf(stderr,
                "[Thread %d] Error: could not initialize CURL.\n",
                data->index);
        fclose(fp);
        return NULL;
    }

    /* Set URL to fetch */
    curl_easy_setopt(curl_handle, CURLOPT_URL, data->url);

    /* Set output file as the write target */
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, fp);

    /* Optional: set a timeout (seconds) */
    curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT, 30L);

    printf("[Thread %d] Fetching URL: %s -> %s\n",
           data->index, data->url, filename);

    res = curl_easy_perform(curl_handle);
    if (res != CURLE_OK) {
        fprintf(stderr,
                "[Thread %d] CURL error: %s\n",
                data->index, curl_easy_strerror(res));
    } else {
        long http_code = 0;
        curl_easy_getinfo(curl_handle, CURLINFO_RESPONSE_CODE, &http_code);
        if (http_code != 200) {
            fprintf(stderr,
                    "[Thread %d] Warning: HTTP response code %ld for %s\n",
                    data->index, http_code, data->url);
        } else {
            printf("[Thread %d] Successfully downloaded %s\n",
                   data->index, data->url);
        }
    }

    curl_easy_cleanup(curl_handle);
    fclose(fp);

    return NULL;
}