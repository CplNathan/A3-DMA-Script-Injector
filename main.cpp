#include "main.h"
#include "a3main.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <vector>
#include <iostream>

const char *argp_program_version =
    "V4.1";
const char *argp_program_bug_address =
    "https://www.unknowncheats.me/forum/arma-3-a/343712-external-eh-script-injector.html";

static char doc[] =
    "ArmA 3 Script Injector - https://github.com/CplNathan/A3-DMA-Script-Injector\n"
    "VMRead - https://github.com/Heep042/vmread"
    "\v"
    "./injector inject -s [SCRIPT] -i -r -f";

static char args_doc[] =
    "-s ~/script.txt\n-s ~/script.txt -r -i -f";

static struct argp_option options[] = {
    {"noinfistar", 'i', 0, OPTION_ARG_OPTIONAL, "Kills anti-cheat threads", 1},
    {"restorehook", 'r', 0, OPTION_ARG_OPTIONAL, "Restores script hook after inject", 1},
    {"filepatching", 'f', 0, OPTION_ARG_OPTIONAL, "Enables file patching", 1},
    {"scriptsource", 's', "FILE", 0, "Script source location", 1},
    {0}};

uint64_t restoreTime = 30;

// ----- Beep Boop ----- //
uint64_t g_armaBase;
uint64_t g_battleyeBase;

std::string g_scriptData;

uint64_t g_NetworkManager;
uint64_t g_NetworkClient;
uint64_t g_NetworkCallback;

static error_t parse_opt(int key, char *arg, struct argp_state *state)
{
    struct arguments *arguments = static_cast<struct arguments *>(state->input);

    switch (key)
    {
    case 'i':
        arguments->noinfistar = true;
        break;
    case 'r':
        arguments->restorehook = true;
        break;
    case 'f':
        arguments->filepatching = true;
        break;
    case 's':
        arguments->sourcescript = arg;
        break;

    case ARGP_KEY_ARG:
        if (state->arg_num >= 0)
            /* Too many arguments. */
            argp_usage(state);

        arguments->args[state->arg_num] = arg;

        break;

    case ARGP_KEY_END:
        if (state->arg_num < 0)
            /* Not enough arguments. */
            argp_usage(state);

        if (!arguments->sourcescript[0])
            /* No script dir provided. */
            argp_usage(state);
        break;

    default:
        return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

static struct argp argp = {options, parse_opt, args_doc, doc};

int main(int argc, char **argv)
{
    std::string scriptdir;

    struct arguments arguments;

    /* Default values. */
    arguments.restorehook = false;
    arguments.noinfistar = false;
    arguments.filepatching = false;
    arguments.sourcescript = (char *)"";

    error_t argsparse = argp_parse(&argp, argc, argv, 0, 0, &arguments);

    if (argsparse == 0) // 0 = No Error
    {
        FILE *scriptfile = fopen(arguments.sourcescript, "r");
        std::string scriptcontent;

        if (scriptfile)
        {
            fseek(scriptfile, 0, SEEK_END);
            long fsize = ftell(scriptfile);
            fseek(scriptfile, 0, SEEK_SET);

            scriptcontent.resize(fsize + 1);
            fread(&scriptcontent[0], 1, fsize, scriptfile);

            fclose(scriptfile);
        }

        printf("Script Location: %s (%s)\nRestore Hook: %s\nFilepatching: %s\nKill Infistar: %s\n", arguments.sourcescript, scriptfile ? "Found" : strerror(errno), arguments.restorehook ? "Yes" : "No", arguments.filepatching ? "Enabled" : "Disabled", arguments.noinfistar ? "Yes" : "No");

        if (scriptfile)
            a3::init(arguments, scriptcontent);
    }

    return 0;
}