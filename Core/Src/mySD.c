#include "mySD.h"
#include "tim.h"
#include "ff.h"  // 确保包含了 FatFs 头文件以使用 FRESULT 和 f_strerror()
#include "fatfs.h"
#include "lcd.h"        // 你用于 LCD 显示的头文件
#include "diskio.h"
#include <stdio.h>      // 为 snprintf 准备

// 创建并写入TXT示例
/**
创建并写入TXT示例
 */
int create_and_write_txt(void) {
    FATFS SDFatFS;     // 文件系统对象
    FIL MyFile;        // 文件对象
    FRESULT res;       // FATFS 返回结果
    UINT byteswritten; // 实际写入的字节数
    char buffer[] = "Hello World,By TangZiheng\n";

//    // 1. 挂载文件系统
//    res = f_mount(&SDFatFS, SDPath, 1);
//    if (res != FR_OK) {
//        // 挂载失败，打印错误信息到LCD
//        lcd_show_string(0, 0, 200, 16, 16, "Mount SD Failed!", RED);
//        return -1;
//    }


	res = f_mount(&SDFatFS, SDPath, 1);
	if (res != FR_OK) {
		char err_msg[64];

		// 方法 1：自己写映射（推荐）
		const char* errmsg;
		switch (res) {
			case FR_DISK_ERR: errmsg = "Disk error"; break;
			case FR_INT_ERR: errmsg = "Internal error"; break;
			case FR_NOT_READY: errmsg = "Not ready"; break;
			case FR_NO_FILESYSTEM: errmsg = "No filesystem"; break;
			case FR_INVALID_DRIVE: errmsg = "Invalid drive"; break;
			case FR_NOT_ENABLED: errmsg = "Not enabled"; break;
			case FR_NO_PATH: errmsg = "No path"; break;
			case FR_MKFS_ABORTED: errmsg = "Mkfs aborted"; break;
			default: errmsg = "Other error"; break;
		}

    snprintf(err_msg, sizeof(err_msg), "Mount failed: %s", errmsg);
    lcd_show_string(0, 0, 200, 16, 16, err_msg, RED);
    return -1;
}

    // 2. 创建或打�?TXT文件 (如果文件不存在则创建)
    res = f_open(&MyFile, "HELLO.TXT", FA_CREATE_ALWAYS | FA_WRITE);
    if (res != FR_OK) {
        // 打开失败，打印错误信息到LCD
        lcd_show_string(0, 0, 200, 16, 16, "Open File Failed!", RED);
        return -2;
    }

    // 3. 写入数据到文�?
    res = f_write(&MyFile, buffer, sizeof(buffer) - 1, &byteswritten);
    if (res != FR_OK || byteswritten == 0) {
        // 写入失败，打印错误信息到LCD
        lcd_show_string(0, 0, 200, 16, 16, "Write File Failed!", RED);
        f_close(&MyFile);
        return -3;
    }

    // 4. 关闭文件
    f_close(&MyFile);

    // 5. 卸载文件系统
    f_mount(NULL, SDPath, 1);

    // 成功完成�?有操作，打印成功信息到LCD
    //lcd_show_string(0, 0, 200, 16, 16, "File Write Success!", BLACK);
    return 0;
}


void read_and_append_txt(const char *filename, const char *append_text) {
    FATFS SDFatFS;     // 文件系统对象
    FIL MyFile;        // 文件对象
    FRESULT res;       // FATFS 返回结果
    UINT byteswritten; // 实际写入的字节数

    // 1. 挂载文件系统
    res = f_mount(&SDFatFS, SDPath, 1);
    if (res != FR_OK) {
        // 挂载失败，打印错误信息到LCD
        lcd_show_string(0, 0, 200, 16, 16, "Mount SD Failed!", RED);
        return;
    }

    // 2. 打开指定文件，追加写入模式 (FA_OPEN_APPEND | FA_WRITE)
    res = f_open(&MyFile, filename, FA_OPEN_APPEND | FA_WRITE);
    if (res != FR_OK) {
        // 打开失败，打印错误信息到LCD
        lcd_show_string(0, 0, 200, 16, 16, "Open File Failed!", RED);
        return;
    }

    // 3. 移动文件指针到文件末尾，并追加新字符串
    res = f_lseek(&MyFile, f_size(&MyFile));
    if (res != FR_OK) {
        // 移动文件指针失败，打印错误信息到LCD
        lcd_show_string(0, 0, 200, 16, 16, "Seek File Failed!", RED);
        f_close(&MyFile);
        return;
    }

    // 4. 写入追加的字符串
    res = f_write(&MyFile, append_text, strlen(append_text), &byteswritten);
    if (res != FR_OK || byteswritten == 0) {
        // 写入失败，打印错误信息到LCD
        lcd_show_string(0, 0, 200, 16, 16, "Write File Failed!", RED);
        f_close(&MyFile);
        return;
    }

    // 5. 关闭文件
    f_close(&MyFile);

    // 6. 卸载文件系统
    f_mount(NULL, SDPath, 1);

    // 成功完成所有操作，打印成功信息到LCD
    //lcd_show_string(30, 130, 200, 16, 16, "Append Success!", GREEN);
}


void read_and_append_txt_line(const char *filename, const char *append_text) {

	__HAL_TIM_DISABLE_IT(&htim2, TIM_IT_UPDATE);//禁用定时器中断

    FATFS SDFatFS;     // 文件系统对象
    FIL MyFile;        // 文件对象
    FRESULT res;       // FATFS 返回结果
    UINT byteswritten; // 实际写入的字节数
    char buffer[256];  // 用于存储追加的文本（包括换行符）

    // 1. 挂载文件系统
    res = f_mount(&SDFatFS, SDPath, 1);
    if (res != FR_OK) {
        // 挂载失败，打印错误信息到LCD
        lcd_show_string(0, 0, 200, 16, 16, "Mount SD Failed!", RED);
        return;
    }

    // 2. 打开指定文件，追加写入模式 (FA_OPEN_APPEND | FA_WRITE)
    res = f_open(&MyFile, filename, FA_OPEN_APPEND | FA_WRITE);
    if (res != FR_OK) {
        // 打开失败，打印错误信息到LCD
        lcd_show_string(0, 0, 200, 16, 16, "Open File Failed!", RED);
        return;
    }

    // 3. 追加文本内容并添加换行符
    snprintf(buffer, sizeof(buffer), "%s\n", append_text);

    // 4. 写入追加的字符串
    res = f_write(&MyFile, buffer, strlen(buffer), &byteswritten);
    if (res != FR_OK || byteswritten == 0) {
        // 写入失败，打印错误信息到LCD
        lcd_show_string(0, 0, 200, 16, 16, "Write File Failed!", RED);
        f_close(&MyFile);
        return;
    }

    // 5. 关闭文件
    f_close(&MyFile);

    // 6. 卸载文件系统
    f_mount(NULL, SDPath, 1);

    // 成功完成所有操作，打印成功信息到LCD
    //lcd_show_string(0, 0, 200, 16, 16, "Append Line Success!", GREEN);

    __HAL_TIM_ENABLE_IT(&htim2, TIM_IT_UPDATE);
}

// 显示SD卡中全部文件的函数
int display_sd_card_files(void) {
    FATFS fs;
    FILINFO fno;
    DIR dir;
    FRESULT res;
    int y_offset = 24;
    int file_count = 0;

    // 挂载SD卡文件系统
    res = f_mount(&fs, SDPath, 1);
    if (res != FR_OK) {
        lcd_show_string(0, y_offset, 200, 20, 12, "Mount SD Failed", BLACK);
        return 0;
    }

    // 打开根目录
    res = f_opendir(&dir, "/");
    if (res == FR_OK) {
        while (1) {
            res = f_readdir(&dir, &fno);  // 读取目录中的下一个条目
            if (res != FR_OK || fno.fname[0] == 0) {
                break;  // 读取失败或到达目录末尾
            }

            if (!(fno.fattrib & AM_DIR)) {  // 只显示文件，不显示目录
                lcd_show_string(0, y_offset, 200, 20, 12, fno.fname, BLACK);
                y_offset += 12;
                file_count++;
            }
        }
        f_closedir(&dir);
    } else {
        lcd_show_string(0, y_offset, 200, 20, 12, "Open Dir Failed", BLACK);
    }

    // 卸载SD卡文件系统
    f_mount(NULL, SDPath, 1);

    return file_count;  // 返回文件数量
}

// 创建指定名称的文件的函数
int create_data_file(int file_index) {
    FATFS fs;
    FIL file;
    FRESULT res;
    char filename[20];

    // 生成文件名，例如 "data1.txt", "data2.txt" 等
    snprintf(filename, sizeof(filename), "data%d.txt", file_index);

    // 挂载SD卡文件系统
    res = f_mount(&fs, SDPath, 1);
    if (res != FR_OK) {
        //printf("Mount SD Failed\n");
        return -1;
    }

    // 创建文件
    res = f_open(&file, filename, FA_CREATE_ALWAYS | FA_WRITE);
    if (res != FR_OK) {
        //printf("Create File Failed\n");
        f_mount(NULL, SDPath, 1);
        return -1;
    }

    // 关闭文件
    f_close(&file);

    // 卸载SD卡文件系统
    f_mount(NULL, SDPath, 1);

    //printf("File %s created successfully\n", filename);
    return 0;  // 返回0表示成功
}

// 获取文件列表中指定索引的文件名
int get_file_name_by_index(int index, char *filename) {
	__HAL_TIM_DISABLE_IT(&htim2, TIM_IT_UPDATE);//禁用定时器中断

	//lcd_show_string(0, 0, 200, 20, 12, "get_file_name_by_index", RED);

    FATFS fs;
    FILINFO fno;
    DIR dir;
    FRESULT res;
    int file_count = 0;

    // 挂载SD卡文件系统
    res = f_mount(&fs, SDPath, 1);
    if (res != FR_OK) {
        printf("Mount SD Failed\n");
        return -1;
    }

    // 打开根目录
    res = f_opendir(&dir, "/");
    if (res == FR_OK) {
        while (1) {
            res = f_readdir(&dir, &fno);  // 读取目录中的下一个条目
            if (res != FR_OK || fno.fname[0] == 0) {
                break;  // 读取失败或到达目录末尾
            }

            if (!(fno.fattrib & AM_DIR)) {  // 只处理文件，不处理目录
                if (file_count == index) {
                    strncpy(filename, fno.fname, 256);  // 将文件名保存到输出参数中
                    f_closedir(&dir);
                    f_mount(NULL, SDPath, 1);
                    return 0;  // 返回0表示成功
                }
                file_count++;
            }
        }
        f_closedir(&dir);
    } else {
        printf("Open Dir Failed\n");
    }

    // 卸载SD卡文件系统
    f_mount(NULL, SDPath, 1);

    __HAL_TIM_ENABLE_IT(&htim2, TIM_IT_UPDATE);
    return -1;  // 未找到指定索引的文件
}

// 读取指定索引文件的前10行并显示在LCD屏幕上
int display_file_first_10_lines(int index, int start_line) {
    FATFS fs;
    FIL file;
    FRESULT res;
    char filename[256];
    char line[128];
    int y_offset = 24;
    int current_line = 0;

    // 获取文件名
    if (get_file_name_by_index(index, filename) != 0) {
        lcd_show_string(0, y_offset, 200, 20, 12, "Get File Name Failed", RED);
        return -1;
    }

    // 挂载SD卡文件系统
    res = f_mount(&fs, SDPath, 1);
    if (res != FR_OK) {
        lcd_show_string(0, y_offset, 200, 20, 12, "Mount SD Failed", RED);
        return -1;
    }

    // 打开文件
    res = f_open(&file, filename, FA_READ);
    if (res != FR_OK) {
        lcd_show_string(0, y_offset, 200, 20, 12, "Open File Failed", RED);
        f_mount(NULL, SDPath, 1);
        return -1;
    }

    // 跳过起始行数
    while (current_line < start_line && f_gets(line, sizeof(line), &file)) {
        current_line++;
    }

    // 读取指定行数并显示在LCD上
    int lines_displayed = 0;
    while (lines_displayed < 24 && f_gets(line, sizeof(line), &file)) {
        lcd_show_string(0, y_offset, 200, 20, 12, line, BLACK);
        y_offset += 12;
        lines_displayed++;
    }

    // 关闭文件
    f_close(&file);

    // 卸载SD卡文件系统
    f_mount(NULL, SDPath, 1);

    return 0;  // 返回0表示成功
}


void buffered_append_txt_lines(const char *filename, char data[][16], int num_lines) {

    __HAL_TIM_DISABLE_IT(&htim2, TIM_IT_UPDATE);  // 禁用定时器中断

    FATFS SDFatFS;     // 文件系统对象
    FIL MyFile;        // 文件对象
    FRESULT res;       // FATFS 返回结果
    UINT byteswritten; // 实际写入的字节数

    // 1. 挂载文件系统
    res = f_mount(&SDFatFS, SDPath, 1);
    if (res != FR_OK) {
        // 挂载失败，打印错误信息到LCD
        lcd_show_string(0, 0, 200, 16, 16, "Mount SD Failed!", RED);
        return;
    }

    // 2. 打开指定文件，追加写入模式 (FA_OPEN_APPEND | FA_WRITE)
    res = f_open(&MyFile, filename, FA_OPEN_APPEND | FA_WRITE);
    if (res != FR_OK) {
        // 打开失败，打印错误信息到LCD
        lcd_show_string(0, 0, 200, 16, 16, "Open File Failed!", RED);
        return;
    }

    // 3. 遍历数据并写入文件
    for (int i = 0; i < num_lines; i++) {
        res = f_write(&MyFile, data[i], strlen(data[i]), &byteswritten);
        if (res != FR_OK || byteswritten == 0) {
            // 写入失败，打印错误信息到LCD
            lcd_show_string(0, 0, 200, 16, 16, "Write File Failed!", RED);
            f_close(&MyFile);
            return;
        }
    }

    // 4. 关闭文件
    f_close(&MyFile);

    // 5. 卸载文件系统
    f_mount(NULL, SDPath, 1);

    // 成功完成所有操作，打印成功信息到LCD
    //lcd_show_string(0, 0, 200, 16, 16, "Append Lines Success!", GREEN);

    __HAL_TIM_ENABLE_IT(&htim2, TIM_IT_UPDATE);
}

