/**
 * TODO:
 *    1. read whole file and dump to hex
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define LINE_SIZE 1024

static const char usage[] = "glsl2cstr <shader>.<ext> <shader>_<ext>.h\n";

static int write_quote_line(FILE *file, const char *line)
{
        char str[LINE_SIZE];
        int len;

        len = snprintf(str, LINE_SIZE, "\"%s\"", line);
        fputs(str, file);
        fputs("\n", file);

        return len;
}

int main(int argc, char **argv)
{
        FILE *ifile, *ofile;
        char *input, *output, *s;
        char filename[80] = {0}, ext[20] = {0};
        int len, total_len = 0;
        char line[LINE_SIZE] = {0}, header_guard[LINE_SIZE] = {0};

        if (argc > 1) {
                input = argv[1];
        } else {
                fprintf(stderr, "%s", usage);
                return -1;
        }

        s = strchr(input, '.');
        if (!s) {
                fprintf(stderr, "Format of input file name is wrong!\n");
                return -1;
        }

        strncpy(filename, input, s - input);
        strcpy(ext, s + 1);

        if (argc > 2) {
                output = argv[2];
        } else {
                output = calloc(strlen(input) + 3, sizeof(char));
                if (!output) {
                        fprintf(stderr, "%s\n", "Out of memory");
                        return -1;
                }
                sprintf(output, "%s_%s.h", filename, ext);
        }

        ifile = fopen(input, "r");
        if (!ifile) {
                fprintf(stderr, "Cannot open %s for read\n", input);
                return -1;
        }

        ofile = fopen(output, "w");
        if (!ofile) {
                fprintf(stderr, "Cannot open %s for write\n", output);
                fclose(ifile);
                return -1;
        }

        /* Build header guard */
        snprintf(header_guard, LINE_SIZE, "GF_%s_%s", filename, ext);

        /* fprintf(stdout, "%s\n", header_guard); */

        /* Header guard is upper case string */
        s = header_guard;
        while ((*s = toupper(*s)))
                ++s;

        fprintf(stdout, "ok");

        snprintf(line, LINE_SIZE, "#ifndef %s\n", header_guard);
        fputs(line, ofile);
        snprintf(line, LINE_SIZE, "#define %s\n\n", header_guard);
        fputs(line, ofile);

        snprintf(line, LINE_SIZE, "const char %s_%s[] = {\n", filename, ext);
        fputs(line, ofile);

        while ((s = fgets(line, LINE_SIZE, ifile))) {
                while (*s) {
                        fprintf(ofile, "0x%x%s", *s, *(s + 1) == '\0' ? ",\n" : ", ");
                        s++;
                        total_len++;
                }
        }
        fprintf(ofile, "%s\n", "0x00");  /* NULL terminate string */

        fputs("};\n\n", ofile);

        snprintf(line, LINE_SIZE, "const int %s_%s_len = %d;\n\n", filename, ext, total_len);
        fputs(line, ofile);

        snprintf(line, LINE_SIZE, "#endif %s", header_guard);
        fputs(line, ofile);

        fclose(ifile);
        fclose(ofile);

        return 0;
}
