#pragma once
#ifdef _WIN32

#include <windows.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef DT_UNKNOWN
#define DT_UNKNOWN 0
#define DT_FIFO    1
#define DT_CHR     2
#define DT_DIR     4
#define DT_BLK     6
#define DT_REG     8
#define DT_LNK     10
#define DT_SOCK    12
#define DT_WHT     14
#endif

#ifndef S_IFLNK
#define S_IFLNK 0120000
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct dirent {
    char d_name[MAX_PATH];
    unsigned char d_type;
};

typedef struct DIR {
    HANDLE handle;
    WIN32_FIND_DATAA findData;
    struct dirent currentEntry;
    int cached;
} DIR;

inline DIR* opendir(const char* name) {
    char searchPath[MAX_PATH];
    snprintf(searchPath, sizeof(searchPath), "%s\\*", name);
    
    DIR* dir = (DIR*)malloc(sizeof(DIR));
    if (!dir) return NULL;

    dir->handle = FindFirstFileA(searchPath, &dir->findData);
    if (dir->handle == INVALID_HANDLE_VALUE) {
        free(dir);
        return NULL;
    }
    dir->cached = 1;
    return dir;
}

inline struct dirent* readdir(DIR* dirp) {
    if (!dirp) return NULL;
    if (dirp->cached) {
        dirp->cached = 0;
    } else {
        if (!FindNextFileA(dirp->handle, &dirp->findData)) {
            return NULL;
        }
    }
    strncpy_s(dirp->currentEntry.d_name, sizeof(dirp->currentEntry.d_name), dirp->findData.cFileName, _TRUNCATE);
    dirp->currentEntry.d_type = (dirp->findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? DT_DIR : DT_REG;
    return &dirp->currentEntry;
}

inline int closedir(DIR* dirp) {
    if (!dirp) return -1;
    if (dirp->handle != INVALID_HANDLE_VALUE) {
        FindClose(dirp->handle);
    }
    free(dirp);
    return 0;
}

inline int scandir(const char* dirp, struct dirent*** namelist,
                   int (*filter)(const struct dirent*),
                   int (*compar)(const struct dirent**, const struct dirent**)) {
    DIR* dir = opendir(dirp);
    if (!dir) return -1;

    struct dirent* entry;
    int count = 0;
    int capacity = 10;
    struct dirent** list = (struct dirent**)malloc(capacity * sizeof(struct dirent*));
    if (!list) {
        closedir(dir);
        return -1;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (filter && !filter(entry)) continue;

        struct dirent* entCopy = (struct dirent*)malloc(sizeof(struct dirent));
        if (!entCopy) continue;
        memcpy(entCopy, entry, sizeof(struct dirent));

        if (count >= capacity) {
            capacity *= 2;
            struct dirent** newList = (struct dirent**)realloc(list, capacity * sizeof(struct dirent*));
            if (!newList) {
                free(entCopy);
                break;
            }
            list = newList;
        }
        list[count++] = entCopy;
    }
    closedir(dir);

    if (compar && count > 1) {
        qsort(list, count, sizeof(struct dirent*), (int (*)(const void*, const void*))compar);
    }

    *namelist = list;
    return count;
}

#ifdef __cplusplus
}
#endif

#endif // _WIN32
