# Multithreaded Web Scraper

Small C program that downloads multiple web pages in parallel. Each URL you pass on the command line is fetched by its own pthread and saved locally as `page_<index>.html` using libcurl.

- Parallel downloads with POSIX threads.
- Graceful logging for errors and non-200 HTTP responses.
- Writes HTML to numbered files in the working directory.

## Requirements
- GCC or Clang with C11 support.
- POSIX threads (usually `-lpthread`).
- libcurl with development headers (usually `-lcurl`).

## Build
From the `multithread_scraper` directory:
```sh
gcc -std=c11 -Wall -Wextra -pedantic main.c scraper.c -lcurl -lpthread -o scraper
```

## Usage
Run the compiled binary with one or more URLs:
```sh
./scraper https://example.com https://www.gnu.org
```

Each URL is fetched concurrently and written to `page_1.html`, `page_2.html`, etc. Download logs and HTTP warnings print to stdout/stderr.

## Notes
- Increase the timeout by adjusting `CURLOPT_TIMEOUT` in `scraper.c` if needed.
- Re-run with different URLs as often as you like; files are overwritten if names collide.
- Clean up downloaded files with `rm page_*.html`.
