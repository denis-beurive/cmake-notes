/**
 * @file Ce fichier implémente un outil intervenant dans le processus de construction des exécutables.
 *       Son rôle est de générer le fichier "version.h" (intégré dans les codes sources des exécutables).
 *
 * Si vous consultez le contenu du fichier "version.h", vous constatez que ce dernier inclut la "date de
 * la compilation".
 */

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

#define VERSION_LOGICIEL "1.0"

int main(const int argc, char *const argv[]) {
    if (3 != argc) {
        printf("Usage: %s <path to the src directory> <name of the output header file>\n", argv[0]);
        return 1;
    }
    const char *const path_src = argv[1];
    const char *const name_output = argv[2];
    const time_t t = time(NULL);
    const struct tm tm = *localtime(&t);

    if (0 != chdir(path_src)) {
        fprintf(stderr, "Cannot change the current directory to \"%s\"\n", path_src);
        return 1;
    }

    FILE *fd = fopen(name_output, "w");
    if (NULL == fd) {
        fprintf(stderr, "Cannot open the file \"%s\" for writing (path to \"src\": %s).\n", name_output, path_src);
        return 1;
    }
    fprintf(fd, "#ifndef VERSION_H\n");
    fprintf(fd, "#define VERSION_H\n");
    fprintf(fd, "#define DATE \"%d-%02d-%02d %02d:%02d:%02d %04ld (%s)\"\n",
            tm.tm_year + 1900,
            tm.tm_mon + 1,
            tm.tm_mday,
            tm.tm_hour,
            tm.tm_min,
            tm.tm_sec,
            tm.tm_gmtoff,
            tm.tm_zone);
    fprintf(fd, "#define SRC_PREFIX_LENGTH %lu\n", strlen(path_src));
    fprintf(fd, "#define VERSION_LOGICIEL \"%s\"\n", VERSION_LOGICIEL);

    fprintf(fd, "#endif\n");
    fclose(fd);
    return 0;
}
