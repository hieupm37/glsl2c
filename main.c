/**
 * main.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#define MAX_SIZE 1024
#define PATH_SEPERATOR '/'

static const char usage[] = "glsl2c path_to_file\n";

/* Find last appearance of ch in str */
static char * lastch(char *str, char ch)
{
        int len = strlen(str);

        while (len > 0) {
                if (str[len - 1] == ch)
                        return (str + len - 1);
                --len;
        }

        return 0;
}

int main(int argc, char **argv)
{
        FILE *ifile, *ofile;
        char *buf;
        int buf_size, i;
        char *input, output[MAX_SIZE];
        char name[MAX_SIZE], *it, *last_sep;
        char header_guard[MAX_SIZE], line[MAX_SIZE];

        if (argc > 1) {
                input = argv[1];
        } else {
                fprintf(stderr, "%s", usage);
                return -1;
        }

        last_sep = lastch(input, PATH_SEPERATOR);
        if (last_sep)
                strcpy(name, last_sep + 1);
        else
                strcpy(name, input);
        /* Replace all . to _ and make sure all characters in name are lower case*/
        it = name;
        while (*it) {
                if (*it == '.')
                        *it = '_';
                else
                        *it = tolower(*it);
                ++it;
        }

        /* Build path of output file from path of input file */
        if (last_sep) {
                strcpy(output, input);
                last_sep = output + (last_sep - input);
                /* Note: last_sep now belongs to output */
                snprintf(last_sep + 1, MAX_SIZE - (last_sep - output + 1),
                         "%s.h", name);
        } else {
                snprintf(output, MAX_SIZE, "%s.h", name);
        }

        ifile = fopen(input, "r");
        if (!ifile) {
                fprintf(stderr, "Cannot open %s for read\n", input);
                return -1;
        }

        /* Read input file to buf */
        fseek(ifile, 0L, SEEK_END);
        buf_size = ftell(ifile);
        rewind(ifile);
        buf = calloc(buf_size, sizeof(char));
        if (!buf) {
                fprintf(stderr, "Out of memory\n");
                fclose(ifile);
                return -1;
        }
        fread(buf, sizeof(char), buf_size, ifile);
        fclose(ifile);  /* We don't need ifile anymore, close it */

        ofile = fopen(output, "w");
        if (!ofile) {
                fprintf(stderr, "Cannot open %s for write\n", output);
                return -1;
        }

        /* Build header guard */
        snprintf(header_guard, MAX_SIZE, "GF_%s_H", name);
        it = header_guard;
        while ((*it = toupper(*it)))
                ++it;

        snprintf(line, MAX_SIZE, "#ifndef %s\n", header_guard);
        fputs(line, ofile);
        snprintf(line, MAX_SIZE, "#define %s\n\n", header_guard);
        fputs(line, ofile);

        snprintf(line, MAX_SIZE, "const char %s[] = {\n", name);
        fputs(line, ofile);

        for (i = 0; i < buf_size; ++i)
                fprintf(ofile, "%s0x%02x%s%s",
                        (i % 13 == 0) ? "    " : " ",
                        buf[i],
                        (i < buf_size - 1) ? "," : "",
                        ((i + 1) % 13 == 0 || (i == buf_size - 1)) ? "\n" : "");

        fputs("};\n\n", ofile);

        snprintf(line, MAX_SIZE, "const int %s_len = %d;\n\n", name, buf_size);
        fputs(line, ofile);

        snprintf(line, MAX_SIZE, "#endif %s", header_guard);
        fputs(line, ofile);

        fclose(ofile);

        return 0;
}
