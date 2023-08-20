#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <argp.h>

#define buffer_size 50

const char *argp_program_version = "URL encoder v1.0";
const char *argp_program_bug_address = "<ghostmhbr@gmail.com>";
static char doc[] = "simple URL encoder";
static char args_doc[] = "input";
static struct argp_option options[] = {
    {.name = "output_file", .key = 'o', .arg = "FILE", .flags = 0, .doc = "output file instead of standard output. NOTE that if the output file exists, it will be overwritten"},
    {.name = "input_file", .key = 'i', .arg = "FILE", .flags = 0, .doc = "input file instead of standard input"},
    {.name = "escape_all", .key = 'a', .arg = 0, .flags = 0, .doc = "escape all characters including alphabet and digits"},
    {.name = "delimiter", .key = 'd', .arg = "STRING", .flags = 0, .doc = "using STRING as delimiter between each character"},
    {0}};

struct arguments
{
    char *arg;
    FILE *outputFile;
    FILE *inputFile;
    int escapeAll;
    char *del;
};

static error_t parse_opt(int key, char *arg, struct argp_state *state)
{
    struct arguments *arguments = state->input;
    switch (key)
    {
    case 'o':
        arguments->outputFile = fopen(arg, "w");
        if (!arguments->outputFile)
        {
            perror("[-] Error opening output file");
            exit(-1);
        }
        break;
    case 'i':
        arguments->inputFile = fopen(arg, "r");
        if (!arguments->inputFile)
        {
            perror("[-] Error opening input file");
            exit(-1);
        }
        break;
    case 'a':
        arguments->escapeAll = 1;
        break;

    case 'd':
        arguments->del = arg;
        break;
    case ARGP_KEY_ARG:
        if (state->arg_num > 1)
        {
            fprintf(stdout, "[-] Too many arguments\n");
            argp_usage(state);
        }
        arguments->arg = arg;
        break;

    case ARGP_KEY_END:
        if (state->arg_num < 1 && arguments->inputFile == NULL)
        {
            fprintf(stdout, "[-] Too few arguments\n");
            argp_usage(state);
        }
        break;
    default:
        ARGP_ERR_UNKNOWN;
        break;
    }
    return 0;
}

static struct argp argp = {
    options, parse_opt, args_doc, doc};

int isAlNum(char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9');
}

void writeOut(const char *const buffer, struct arguments args)
{
    int i = 0;
    char c = buffer[0];
    while (c > 0)
    {
        if (args.escapeAll)
        {
            fprintf(args.outputFile, "%%%x", (int)c);
        }
        else
        {
            if (isAlNum(c))
            {
                fprintf(args.outputFile, "%c", c);
            }
            else
            {
                fprintf(args.outputFile, "%%%x", (int)c);
            }
        }
        i++;
        c = buffer[i];
        if (c)
            printf("%s", args.del);
    }
}

int main(int argc, char **argv)
{
    struct arguments arguments;
    arguments.outputFile = stdout;
    arguments.inputFile = NULL;
    arguments.arg = NULL;
    arguments.escapeAll = 0;
    arguments.del = "";

    char buffer[buffer_size];
    argp_parse(&argp, argc, argv, 0, 0, &arguments);

    if (arguments.inputFile)
        while (fgets(buffer, buffer_size, arguments.inputFile))
            writeOut(buffer, arguments);
    else
        writeOut(arguments.arg, arguments);

    fprintf(arguments.outputFile, "\n");

    return 0;
}