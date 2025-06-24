#include "sdio.h"
#include "lcd.h"
#include "fatfs.h"
#include "sdio.h"

int create_and_write_txt(void);

void read_and_append_txt(const char *filename, const char *append_text);
void read_and_append_txt_line(const char *filename, const char *append_text);
int display_sd_card_files(void);
int create_data_file(int file_index);
int get_file_name_by_index(int index, char *filename);
int display_file_first_10_lines(int index,int start_line);
void buffered_append_txt_lines(const char *filename, char data[][16], int num_lines);
