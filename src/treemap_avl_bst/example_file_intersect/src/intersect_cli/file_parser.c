#include <file_parser.h>
#include <intersect.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <wctype.h>

static void parse_file(intersect_t *intersect, const char *file);

/*!
 * @brief Main Intersect API will iterate through the
 * @param tracker
 */
void read_file(file_tracker_t *tracker) {
  intersect_t *intersect = create_intersect_dict();

  for (int i = 0; i < tracker->file_count; i++) {
    intersect->file_iteration = i + 1;
    parse_file(intersect, tracker->files[i]);

    if (intersect->file_iteration > 1) {
      intersect_reduce(intersect->dict, intersect->file_iteration);
    }
  }

  recurse_dict(intersect->dict, intersect_print, tracker);
  destroy_dict(intersect->dict);
  free(intersect);
}

void parse_file(intersect_t *intersect, const char *file) {
  freopen(NULL, "wb", stdout);
  setlocale(LC_ALL, "en_US.UTF-8");

  FILE *fp = fopen(file, "r");
  wchar_t c;

  int word_start = 0;
  int word_end = 0;
  int read;
  while ((c = fgetwc(fp))) {
    if (ferror(fp)) {
      break;
    } else if (feof(fp)) {
      read = word_end - word_start + 1;
      if (1 != read) {
        fseek(fp, word_start, SEEK_SET);
        wchar_t buffer[read];

        fgetws(buffer, read, fp);
        put_key_val_adv(
            intersect->dict,
            create_intersect_payload(buffer, intersect->file_iteration),
            intersect_adv_add, intersect);
        break;
      }
      break;
    }

    if (iswspace(c)) {
      read = word_end - word_start + 1;
      if (1 != read) {
        fseek(fp, word_start, SEEK_SET);
        wchar_t buffer[read];

        fgetws(buffer, read, fp);

        put_key_val_adv(
            intersect->dict,
            create_intersect_payload(buffer, intersect->file_iteration),
            intersect_adv_add, intersect);

        word_start = word_end + 1;
        word_end = word_start;
        fseek(fp, word_start, SEEK_SET);
      } else {
        word_end++;
        word_start = word_end;
      }
    } else {
      word_end++;
    }
  }
  fclose(fp);
}
