#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <pthread.h>
#include "scraper.h"

/*
 * Multi-threaded Web Scraper
 *
 * Usage:
 *   ./scraper <url1> <url2> ... <urlN>
 *
 * Each URL is fetched in parallel using a separate POSIX thread (pthreads),
 * and saved to a file named "page_<index>.html".
 */
int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr,
                "Usage: %s <url1> <url2> ... <urlN>\n"
                "Example: %s https://example.com https://www.google.com\n",
                argv[0], argv[0]);
        return EXIT_FAILURE;
    }

    int url_count = argc - 1;

    /* Initialize libcurl globally (thread-safe for separate easy handles) */
    if (curl_global_init(CURL_GLOBAL_DEFAULT) != 0) {
        fprintf(stderr, "Error: curl_global_init failed.\n");
        return EXIT_FAILURE;
    }

    pthread_t *threads = malloc(url_count * sizeof(pthread_t));
    ThreadData *thread_data = malloc(url_count * sizeof(ThreadData));

    if (!threads || !thread_data) {
        fprintf(stderr, "Error: memory allocation failed.\n");
        free(threads);
        free(thread_data);
        curl_global_cleanup();
        return EXIT_FAILURE;
    }

    /* Create one thread per URL */
    for (int i = 0; i < url_count; i++) {
        /* Copy URL into thread-specific data */
        strncpy(thread_data[i].url, argv[i + 1], MAX_URL_LENGTH - 1);
        thread_data[i].url[MAX_URL_LENGTH - 1] = '\0';
        thread_data[i].index = i + 1;  /* start from 1 for nicer filenames */

        int rc = pthread_create(&threads[i], NULL, fetch_url, &thread_data[i]);
        if (rc != 0) {
            fprintf(stderr,
                    "Error: pthread_create failed for URL %s (code %d)\n",
                    thread_data[i].url, rc);
        }
    }

    /* Wait for all threads to finish */
    for (int i = 0; i < url_count; i++) {
        pthread_join(threads[i], NULL);
    }

    free(threads);
    free(thread_data);

    curl_global_cleanup();

    printf("All downloads attempted. Check 'page_*.html' files.\n");
    return EXIT_SUCCESS;
}