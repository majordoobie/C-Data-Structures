#include <file_parser.h>
#include <wchar.h>

int main(int argc, char ** argv)
{
    wprintf(L"Please wait...\n");

    file_tracker_t * tracker = init_tracker();
    parse_args(argc, (const char **) argv, tracker);
    read_file(tracker);
    free_tracker(tracker);
}
