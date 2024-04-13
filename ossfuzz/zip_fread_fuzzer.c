#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <zip.h>

#ifdef __cplusplus
extern "C"
#endif
    int
    LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    zip_t *archive;
    zip_error_t error;
    zip_source_t *source;
    zip_file_t *file;
    zip_int64_t num_entries, bytes_read;
    zip_uint64_t bufsize = 4096;
    void *buf;

    // Create zip source from memory
    source = zip_source_buffer_create(Data, Size, 0, &error);
    if (source == NULL) {
        zip_error_fini(&error);
        return 0;
    }

    // Open zip archive from source
    archive = zip_open_from_source(source, 0, &error);
    if (archive == NULL) {
        zip_source_free(source);
        zip_error_fini(&error);
        return 0;
    }

    // Get number of entries in the zip archive
    num_entries = zip_get_num_entries(archive, 0);
    if (num_entries < 0) {
        zip_close(archive);
        return 0;
    }

    // Allocate buffer for reading data
    buf = malloc(bufsize);
    if (buf == NULL) {
        zip_close(archive);
        return 0;
    }

    // Iterate through all entries in the archive and fuzz zip_fread
    for (zip_int64_t i = 0; i < num_entries; i++) {
        file = zip_fopen_index(archive, i, 0);
        if (file == NULL) {
            continue;
        }

        // Read the contents of the file
        do {
            bytes_read = zip_fread(file, buf, bufsize);
        } while (bytes_read > 0);

        // Close the file
        zip_fclose(file);
    }

    // Free the buffer and close the archive
    free(buf);
    zip_close(archive);

    return 0;
}