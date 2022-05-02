#include <arg_parser.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

static bool file_exists(const char * file_path);
file_tracker_t * init_tracker()
{
    file_tracker_t * tracker = calloc(1, sizeof(* tracker));
    tracker->print_all = false;
    return tracker;
}
void free_tracker(file_tracker_t * tracker)
{
    for (int i = 0; i < tracker->file_count; i++)
    {
        free(tracker->files[i]);
    }
    free(tracker->files);
    free(tracker);
}

/*!
 * @brief Parse the command line arguments and set the appropriate options in the
 * arg_parse_args_t struct. It will also add the file path array to the struct
 * @param argc[in] argc  Arg count
 * @param argv[in] argv  Array of string array
 * @param args[in] args  Pointer to arg_parse_args_t struct
 * @return 0 if parsing was successful, -1 if parsing was not
 */
int8_t parse_args(int argc, const char ** argv, file_tracker_t * args)
{
    int getopt_result = getopt(argc, (char *const *) argv, ":a");
    while (-1 != getopt_result)
    {
        switch(getopt_result)
        {
            case 'a':
                args->print_all = true;
                break;
            default:
                fprintf(stderr, "Usage: %s [OPTION]... [FILES]...\n", argv[0]);
                return -1;
        }
        getopt_result = getopt(argc, (char *const *) argv, "a");
    }

    if (argc == optind)
    {
        fprintf(stderr, "No file paths passed in\n");
        return -1;
    }


    int file_count = argc - optind;
    args->size = file_count;
    args->files = malloc(sizeof(* args->files) * file_count);
    int file_path_index = 0;
    for (int index = optind; index < argc; index++)
    {
        if (file_exists(argv[index]))
        {
            args->files[file_path_index] = malloc(sizeof(char) * strlen(argv[index]) + 1);
            strncpy(args->files[file_path_index], argv[index], strlen(argv[index]) + 1);
            args->files[file_path_index][strlen(argv[index])] = '\0';
            file_path_index++;
            args->file_count++;
        }
        else
        {
            fprintf(stderr, "Could not find or open file %s\nSkipping...\n", argv[index]);
        }
    }
    return 0;
} // parse_args()

static bool file_exists(const char * file_path)
{
    struct stat stats;
    if (stat(file_path, &stats) == -1)
    {
        return false;
    }
        return S_ISREG(stats.st_mode);
}
