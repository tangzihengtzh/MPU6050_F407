/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2022 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "fatfs.h"
#include "i2c.h"
#include "sdio.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "fsmc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lcd.h"
#include "MPU.h"
#include "string.h"

#include "lcd.h"   // 假设 lcd.h 是你用来控制 LCD 的头文件
#include "sdio.h"

//static int nums_tim_ex = -1;
static int nums_points = 0;
static int cur_rate = 0;
static int state = 0;
static char msg_tim[24];

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim == &htim2)
	{
		if(state == 2)
		{
			//nums_tim_ex++;
			sprintf(msg_tim, "rate:%u/s", nums_points);
			cur_rate = nums_points;
			lcd_show_string(120, 0, 200, 20, 12, msg_tim, BLUE);
			nums_points = 0;
		}
	}
}

/* ------------------- 常量区 ------------------- */
#define BUFF_SIZE          240
#define AVG_SAMPLE_N       10
#define ACC_SENS           8192.0f          // LSB / g  (±2 g)
#define G2MS2              9.80665f          // 1 g = 9.80665 m/s²
#define PIXEL_SCALE        20.0f             // 1 m/s² -> 20 像素，可按需调整
#define LCD_Y_ZERO         200               // 在哪一行画 0 轴
#define FILE_FLUSH_LINES   64

/* ------------------- 全局 / 静态变量 ------------------- */
static float  acc_ms2_buf[BUFF_SIZE];        // 环形缓冲：真实 m/s²
static char   txt_buffer[FILE_FLUSH_LINES][32];
static int    txt_idx  = 0;                  // 写文本缓冲计数
static int    buf_idx  = 0;                  // 环形缓冲索引
static size_t points_total = 0;              // 采到的总点数

/* ------------------- 将 16 bit 原始值转换为 m/s² ------------------- */
static inline float raw_to_ms2(int16_t raw)
{
    return (raw / ACC_SENS) * G2MS2;
}

/* ------------------- 显示比例与坐标 ------------------- */
#define LCD_Y_ZERO      200.0f     /* 0 g 基准线像素行。改位置只需改这一行   */
#define BASE_PIXEL_SCALE 10.0f     /* 1 m/s² → 10 px（之前是 20 px）         */
/* ---------- Y 轴相关：已使用 ---------- */
#define LCD_Y_ZERO        200.0f
#define BASE_PIXEL_SCALE   10.0f   /* 1 m/s² → 10 px */
static float g_y_mult      = 1.0f; /* 纵向倍率 */

/* ---------- X 轴抽样：新增 ---------- */
#define X_SKIP_DEFAULT      8      /* 显示分辨率：每 8 个采样点 → 1 个像素 */
static uint8_t  g_x_skip   = X_SKIP_DEFAULT;  /* 运行时可调 */
static uint16_t g_x_accum  = 0;               /* 跳过计数器 */


/* 可选：运行时纵向放大倍率（例如按键修改 0.5 / 1 / 2） */
//static float g_y_mult = 1.0f;      /* 默认 ×1，可在按键或菜单里动态调整      */


/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_FSMC_Init();
  MX_I2C1_Init();
  MX_USART1_Init();
  MX_SDIO_SD_Init();
  MX_FATFS_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start_IT(&htim2);
	lcd_init();
	mpu_init();
	//lcd_show_string(30, 50, 200, 16, 16, "starting", RED);

	while(create_and_write_txt() != 0)
	{
		lcd_show_string(40, 120, 200, 20, 32, "NO FAT32", RED);
		static int timer_value = 0;
		static int old_timer_value = 0;
		static int time_show_0 = 0;
		old_timer_value = timer_value;
		timer_value = __HAL_TIM_GET_COUNTER(&htim2);
		if(old_timer_value > timer_value)
		{
			time_show_0 ++;
			char msg_time_show_0[24];
			sprintf(msg_time_show_0,"waiting:%d",time_show_0);
			lcd_show_string(40, 160, 200, 20, 32, msg_time_show_0, RED);
		}
	}


	int data_list[240];
	float data_source_g[240];
	int data_list_diff[240];
	data_list[0]=0;
	unsigned int idx = 0;
	lcd_draw_line(0, 200, 239, 200,0x07FF);



  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1) {

//		while (1) {
//
//			//KEY_TEST
//			int ans_key_W =read_key_state('W');
//			char MSG_KEY_W[32];
//			sprintf(MSG_KEY_W, "KEY_W:%d", ans_key_W);
//			lcd_show_string(0, 0, 200, 20, 16, MSG_KEY_W, RED);
//
//			int ans_key_A =read_key_state('A');
//			char MSG_KEY_A[32];
//			sprintf(MSG_KEY_A, "KEY_A:%d", ans_key_A);
//			lcd_show_string(0, 20, 200, 20, 16, MSG_KEY_A, RED);
//
//			int ans_key_S =read_key_state('S');
//			char MSG_KEY_S[32];
//			sprintf(MSG_KEY_S, "KEY_S:%d", ans_key_S);
//			lcd_show_string(0, 40, 200, 20, 16, MSG_KEY_S, RED);
//
//			int ans_key_D =read_key_state('D');
//			char MSG_KEY_D[32];
//			sprintf(MSG_KEY_D, "KEY_D:%d", ans_key_D);
//			lcd_show_string(0, 60, 200, 20, 16, MSG_KEY_D, RED);
//
//		}
		int is_fir = 1;
		static char cur_dir = 'x';
		char data_path[24];

		while(state == 0)
		{
			lcd_show_string(0, 0, 200, 20, 16, ">>>----<<<          ", BLACK);
			lcd_show_string(0, 20, 200, 20, 16, "MAIN MENU", BLUE);
			lcd_show_string(0, 40, 200, 20, 16, "[A:START]", BLACK);
			if(is_fir == 1)
			{
				lcd_show_string(0, 140, 200, 20, 16, "CUR DIR:X  ", 0xB9E0);
				is_fir = 0;
			}
			lcd_show_string(0, 60, 200, 20, 16, "[S:CHANGE DIR Y]", BLACK);
			lcd_show_string(0, 80, 200, 20, 16, "[W:CHANGE DIR Z]", BLACK);
			lcd_show_string(0, 100, 200, 20, 16, "[D:CHANGE DIR X]", BLACK);
			lcd_show_string(0, 120, 200, 20, 16, ">>>----<<<", BLACK);

			lcd_show_string(120, 240, 200, 20, 16, "---KEY SET---", BLACK);
			lcd_show_string(120, 260, 200, 20, 16, "        oW", BLACK);
			lcd_show_string(120, 285, 200, 20, 16, "oRST oA oS oD", BLACK);
			if(read_key_state('A') == 0)
			{
				while(read_key_state('A') == 0)
				{

				}
				lcd_fill(0, 0, 239, 319,0xFFFF);
				state = 1;
			}

			if(read_key_state('S') == 0)
			{
				lcd_show_string(0, 140, 200, 20, 16, " CUR DIR:Y  ", 0x045F);
				cur_dir = 'y';
				data_path[4]='y';
			}

			if(read_key_state('W') == 0)
			{
				lcd_show_string(0, 140, 200, 20, 16, "  CUR DIR:Z ", 0x05E0);
				cur_dir = 'z';
				data_path[4]='z';
			}

			if(read_key_state('D') == 0)
			{
				lcd_show_string(0, 140, 200, 20, 16, "CUR DIR:X  ", 0xB9E0);
				cur_dir = 'x';
				data_path[4]='x';
			}
		}
		static int sel_file = 2;
		while(state == 1)
		{
			int is_fr_s1 = 1;
			int is_SD = 1;
			if(is_fr_s1 == 1)
			{
				is_SD = create_and_write_txt();
				is_fr_s1 = 0;
			}
			if(is_SD != 0)
			{
				char MSG_SD[32];
				sprintf(MSG_SD, "SD_FAT32_WRONG:%d", is_SD);  // 显示两位小数
				lcd_show_string(0, 0, 200, 20, 16, MSG_SD, RED);

				lcd_fill(0, 20, 239, 319,0xFFFF);
				state = 0;
			}
			else
			{
				lcd_show_string(0, 0, 200, 20, 12, "W/S:SELECT FILE[A:OK][D:VIEW]", RED);
				lcd_show_string(120, 240, 200, 20, 16, "---KEY SET---", BLACK);
				lcd_show_string(120, 260, 200, 20, 16, "        oW", BLACK);
				lcd_show_string(120, 285, 200, 20, 16, "oRST oA oS oD", BLACK);
				int nums_file = display_sd_card_files();
				//state = 2;
				lcd_show_string(200, sel_file*12, 200, 20, 12, "<<<", BLUE);
				if(read_key_state('W') == 0)
				{
					if(sel_file > 1)
					{
						sel_file -= 1;
						lcd_fill(200, 12, 239, 319,0xFFFF);
						lcd_show_string(200, sel_file*12, 200, 20, 12, "<<<", BLUE);
						if(sel_file == 1)
						{
							lcd_fill(200, 12, 239, 319,0xFFFF);
							lcd_show_string(0, sel_file*12, 200, 20, 12, "NEW FILE", RED);
							lcd_show_string(200, sel_file*12, 200, 20, 12, "<<<", BLUE);
						}
					}
					while(read_key_state('W') == 0)
					{

					}
				}
				if(read_key_state('S') == 0)
				{
					if(sel_file < nums_file+1)
					{
						sel_file += 1;
						lcd_fill(200, 12, 239, 319,0xFFFF);
						lcd_show_string(200, sel_file*12, 200, 20, 12, "<<<", BLUE);
					}
					while(read_key_state('S') == 0)
					{

					}
				}
				if(read_key_state('A') == 0)
				{
					while(read_key_state('A') == 0)
					{

					}
					if(sel_file == 1)
					{
						int isOK_new_file = create_data_file(nums_file+1);
						if(isOK_new_file != 0)
						{
							lcd_fill(0, 12, 239, 319,0xFFFF);
							lcd_show_string(0, 0, 200, 20, 12, "File Create Failed", RED);
							lcd_show_string(0, 12, 200, 20, 12, "D:EXIT", RED);
							while(read_key_state('D') == 1)
							{

							}
						}
						else
						{
							snprintf(data_path, sizeof(data_path), "data%d.txt", nums_file+1);
							lcd_fill(0, 0, 239, 319,0xFFFF);
							state = 3;
						}
					}
					else
					{
						//char sel_file_name[24];
						int test_open_file = get_file_name_by_index(sel_file-2, data_path);
						//lcd_show_string(0, 12, 200, 20, 12, data_path, RED);
						lcd_fill(0, 0, 239, 319,0xFFFF);
						//lcd_show_string(0, 12, 200, 20, 12, sel_file_name, RED);
						state = 3;
					}
				}
				if(read_key_state('D') == 0)
				{
					while(read_key_state('D') == 0)
					{

					}
					lcd_fill(0, 0, 239, 319,0xFFFF);
					state = 4;
				}

			}
		}

		static int wave_show = 0;
		static unsigned int disp_rate = 0;
		if(state == 3)
		{
			lcd_show_string(0, 0, 200, 20, 12, data_path, BLACK);
			lcd_show_string(0, 12, 200, 20, 12, "[A:HIGH SPEED NO WAVE SHOW]", BLUE);
			lcd_show_string(0, 24, 200, 20, 12, "[S:LOW SPEED WITH WAVE SHOW]", RED);
			lcd_show_string(0, 36, 200, 20, 12, "[D:EXIT]", BLACK);
			if(read_key_state('A') == 0)
			{
				while(read_key_state('A') == 0)
				{

				}
				lcd_fill(0, 0, 239, 319,0xFFFF);
				wave_show = 0;
				state = 2;
			}

			if(read_key_state('S') == 0)
			{
				while(read_key_state('S') == 0)
				{

				}
				lcd_fill(0, 0, 239, 319,0xFFFF);
				wave_show = 1;
				state = 2;
			}

			if(read_key_state('D') == 0)
			{
				while(read_key_state('D') == 0)
				{

				}
				lcd_fill(0, 0, 239, 319,0xFFFF);
				state = 1;
			}
		}



		if(state == 4)
		{

			static int start_line = 0;
			display_file_first_10_lines(sel_file-2,start_line);
			lcd_show_string(0, 0, 200, 20, 12, "[D:EXIT][S:NETX PAGE][W:LAST PAGE]", RED);

//			lcd_show_string(120, 240, 200, 20, 16, "---KEY SET---", BLACK);
//			lcd_show_string(120, 260, 200, 20, 16, "        oW", BLACK);
//			lcd_show_string(120, 285, 200, 20, 16, "oRST oA oS oD", BLACK);

			char title[]="PAGE:    ";
			snprintf(title, sizeof(title), "PAGE:%d", start_line/24+1);
			lcd_show_string(0, 12, 200, 20, 12, title, RED);

			if(read_key_state('D') == 0)
			{
				while(read_key_state('D') == 0)
				{

				}
				start_line = 0;
				lcd_fill(0, 0, 239, 319,0xFFFF);
				state = 1;
			}
			if(read_key_state('S') == 0)
			{
				while(read_key_state('S') == 0)
				{

				}
				lcd_fill(0, 0, 239, 319,0xFFFF);
				start_line += 24;
			}
			if(read_key_state('W') == 0)
			{
				while(read_key_state('W') == 0)
				{

				}
				lcd_fill(0, 0, 239, 319,0xFFFF);
				if(start_line >= 24)
				{
					start_line -= 24;
				}
			}
		}

	    // 定义一个函数指针类型
	    typedef int16_t (*MPU_Read_Accel_Func)(void);

	    MPU_Read_Accel_Func read_accel_func;

	    if (cur_dir == 'x')
	    {
	        read_accel_func = MPU_Read_Accel_X;
	    }
	    else if (cur_dir == 'y')
	    {
	        read_accel_func = MPU_Read_Accel_Y;
	    }
	    else if (cur_dir == 'z')
	    {
	        read_accel_func = MPU_Read_Accel_Z;
	    }


	    char data_buffer[64][16];  // 缓存，用于存储 64 次加速度数据
	    int buffer_index = 0;     // 当前缓存索引

		float base_bis = 0;
		float cru_bis = 1.0;
		int is_fir_s2 = 1;

//		while(state == 2)
//		{
//		    idx = (idx + 1) % 240;
//		    int sum = 0;
//		    int16_t A = 0;  // 加�?�度值，根据方向动�?�获�??
//
//		    // 执行循环并读取加速度数据
//		    for (int j = 0; j < 60; j++)
//		    {
//		        A = read_accel_func();  // 调用函数指针，读取加速度数据
//		        sum += (int)(A);
//		    }
//		    float acc = sum *(1.0/ 30.0);
//
//
//		    char acc_show[16];
//		    float accel_in_g = acc * (1.0/16384.0);
//		    nums_points++;
//
//		    data_list[idx] = (int)(1000 * accel_in_g);
//		    if(idx > 3)
//		    {
//		        data_list_diff[idx] = ((data_list[idx] - data_list[idx - 1])+data_list_diff[idx-1]+data_list_diff[idx-2])*(1.0/3.0);
//		    }
////
//		    // 根据 cur_dir 显示不同的轴信息
//
//
//
//			if(cur_dir == 'x')
//			{
//				sprintf(acc_show, "X: %.4f *  \n", (data_list_diff[idx]) * cur_rate *(1.0/1000));
//			}
//			else if(cur_dir == 'y')
//			{
//				sprintf(acc_show, "Y: %.4f *  \n", (data_list_diff[idx]) * cur_rate*(1.0/1000));
//			}
//			else if(cur_dir == 'z')
//			{
//				sprintf(acc_show, "Z: %.4f *  \n", (data_list_diff[idx]) * cur_rate*(1.0/1000));
//			}
//
//
//		    // 将 acc_show 编写到数据缓冲区
//		    strncpy(data_buffer[buffer_index], acc_show, sizeof(acc_show));
//		    buffer_index++;
//		    // 当数据缓冲区满 64 条数据后，调用文件写入函数
//		    if (buffer_index >= 64)
//		    {
//		        buffered_append_txt_lines(data_path, data_buffer, 64);
//		        buffer_index = 0;  // 重置缓冲区指针
//		    }
//
//
//
//		    if(idx > 10 && idx < 230 && wave_show == 1)
//		    {
//		        lcd_draw_line(idx - 1, 200 - data_list_diff[idx - 1], idx, 200 - data_list_diff[idx], 0x001F);
//		    }
//		    else
//		    {
//		        if(idx == 235)
//		        {
//		            lcd_fill(0, 0, 239, 319, 0xFFFF);
//				    if(wave_show == 1)
//				    {
//						lcd_show_string(0, 0, 200, 16, 16, data_path, BLUE);
//						lcd_show_string(0, 20, 200, 16, 16, acc_show, BLUE);
//				    }
//
//		            for(int k = 0; k < 6; k++)
//		            {
//		                lcd_draw_line(5, 170 + 10 * k, 234, 170 + 10 * k, 0x07FF);
//		            }
//		            lcd_draw_line(5, 170, 5, 220, 0x07FF);
//		            lcd_draw_line(234, 170, 234, 220, 0x07FF);
//		        }
//		    }
//
//		    if(wave_show == 1)
//		    {
//		    	disp_rate++;
//		    	if(disp_rate % 20 == 0)
//		    	{
//				    lcd_show_string(0, 0, 200, 16, 16, data_path, BLUE);
//				    lcd_show_string(0, 20, 200, 16, 16, acc_show, BLUE);
//		    	}
//		    }
//
//		}



//		/* ------------------- 主循环 ------------------- */
//		while (state == 2)
//		{
//		    /* ===== 1. 采样并求平均 ===== */
//		    int32_t sum_raw = 0;
//		    for (int j = 0; j < AVG_SAMPLE_N; ++j)
//		        sum_raw += (int32_t)read_accel_func();   // 读 X/Y/Z 中选定方向的原始计数
//
//		    float acc_ms2 = raw_to_ms2(sum_raw / (float)AVG_SAMPLE_N);
//
//		    /* ===== 2. 存入环形缓冲 ===== */
//		    buf_idx = (buf_idx + 1) % BUFF_SIZE;
//		    acc_ms2_buf[buf_idx] = acc_ms2;
//		    ++points_total;
//
//		    /* ===== 3. 生成要显示 / 记录的字符串 ===== */
//		    char line[32];
//		    snprintf(line, sizeof(line), "%c: %+7.3f m/s^2\r\n", cur_dir, acc_ms2);
//
//		    strncpy(txt_buffer[txt_idx], line, sizeof(line));
//		    if (++txt_idx >= FILE_FLUSH_LINES)
//		    {
//		        buffered_append_txt_lines(data_path, txt_buffer, FILE_FLUSH_LINES);
//		        txt_idx = 0;
//		    }
//
//
//		    /* ===== 4. 绘波形（点对点连线） ===== */
//		    if (wave_show && points_total > 1)
//		    {
//		        /* 像素比例：物理量×(px / m/s²)*倍率 */
//		        const float px_per_ms2 = BASE_PIXEL_SCALE * g_y_mult;
//
//		        /* 计算上一点 / 当前点的像素坐标 */
//		        int16_t x_prev = (buf_idx == 0) ? BUFF_SIZE - 1 : buf_idx - 1;
//		        int16_t y_prev = (int16_t)(LCD_Y_ZERO - acc_ms2_buf[x_prev] * px_per_ms2);
//		        int16_t y_now  = (int16_t)(LCD_Y_ZERO - acc_ms2       * px_per_ms2);
//
//		        /* 限幅防止越界 */
//		        if (y_prev < 0)   y_prev = 0;
//		        if (y_prev > 319) y_prev = 319;
//		        if (y_now  < 0)   y_now  = 0;
//		        if (y_now  > 319) y_now  = 319;
//
//		        lcd_draw_line(x_prev, y_prev, buf_idx, y_now, 0x001F);
//		    }
//
//
//		    /* ===== 5. 每一圈清屏、重画网格与标题 ===== */
//		    if (buf_idx == BUFF_SIZE - 5)
//		    {
//		        lcd_fill(0, 0, 239, 319, 0xFFFF);                 // 背景白
//		        if (wave_show)
//		        {
//		            lcd_show_string(0, 0, 240, 16, 16, data_path, BLUE);
//		            lcd_show_string(0, 20, 240, 16, 16, line,     BLUE);
//		        }
//
//		        /* 画网格（示例 5 条水平线） */
//		        for (int k = 0; k < 6; ++k)
//		        {
//		            int y = LCD_Y_ZERO - 25 + 10 * k;
//		            lcd_draw_line(5, y, 234, y, 0x07FF);
//		        }
//		        lcd_draw_line(5,   LCD_Y_ZERO-25, 5,   LCD_Y_ZERO+25, 0x07FF);
//		        lcd_draw_line(234, LCD_Y_ZERO-25, 234, LCD_Y_ZERO+25, 0x07FF);
//		    }
//
//		    /* ===== 6. 周期性刷新文字 ===== */
//		    static uint32_t disp_cnt = 0;
//		    if (wave_show && (++disp_cnt % 20 == 0))
//		    {
//		        lcd_show_string(0, 0, 240, 16, 16, data_path, BLUE);
//		        lcd_show_string(0, 20, 240, 16, 16, line,     BLUE);
//		    }
//		    nums_points++;//这是记录采样率的变量，别删了，采样率显示函数在时钟中断里
//		}

		/* ------------------- 主循环 ------------------- */
		while (state == 2)
		{
		    /* ===== 1. 采样并求平均 ===== */
		    int32_t sum_raw = 0;
		    for (int j = 0; j < AVG_SAMPLE_N; ++j)
		        sum_raw += (int32_t)read_accel_func();          // 读选定轴原始计数

		    float acc_ms2 = raw_to_ms2(sum_raw / (float)AVG_SAMPLE_N);

		    /* --------------------------------------------------
		     * X 轴抽样限速：
		     *   只有当累积计数 >= g_x_skip 时，才前进 1 个像素并存入环形缓冲
		     * -------------------------------------------------- */
		    if (++g_x_accum >= g_x_skip)    /* 到达抽样间隔 */
		    {
		        g_x_accum = 0;              /* 计数清零 */

		        /* ===== 2. 存入环形缓冲 ===== */
		        buf_idx = (buf_idx + 1) % BUFF_SIZE;
		        acc_ms2_buf[buf_idx] = acc_ms2;
		        ++points_total;

		        /* ===== 3. 生成要显示 / 记录的字符串 ===== */
		        char line[32];
		        snprintf(line, sizeof(line),
		                 "%c: %+7.3f m/s^2\r\n", cur_dir, acc_ms2);

		        strncpy(txt_buffer[txt_idx], line, sizeof(line));
		        if (++txt_idx >= FILE_FLUSH_LINES)
		        {
		            buffered_append_txt_lines(data_path,
		                                      txt_buffer,
		                                      FILE_FLUSH_LINES);
		            txt_idx = 0;
		        }

		        /* ===== 4. 绘波形（点对点连线） ===== */
		        if (wave_show && points_total > 1)
		        {
		            const float px_per_ms2 = BASE_PIXEL_SCALE * g_y_mult;

		            int16_t x_prev = (buf_idx == 0) ? (BUFF_SIZE - 1)
		                                            : (buf_idx - 1);
		            int16_t y_prev = (int16_t)(LCD_Y_ZERO
		                               - acc_ms2_buf[x_prev] * px_per_ms2);
		            int16_t y_now  = (int16_t)(LCD_Y_ZERO
		                               - acc_ms2          * px_per_ms2);

		            /* 防越界 */
		            y_prev = (y_prev < 0)   ? 0 : (y_prev > 319 ? 319 : y_prev);
		            y_now  = (y_now  < 0)   ? 0 : (y_now  > 319 ? 319 : y_now);

		            lcd_draw_line(x_prev, y_prev, buf_idx, y_now, 0x001F);
		        }

		        /* ===== 5. 每一圈清屏、重画网格与标题 ===== */
		        if (buf_idx == (BUFF_SIZE - 5))
		        {
		            lcd_fill(0, 0, 239, 319, 0xFFFF);  // 背景白
		            if (wave_show)
		            {
		                lcd_show_string(0, 0, 240, 16, 16, data_path, BLUE);
		                lcd_show_string(0, 20, 240, 16, 16, line,     BLUE);
		            }

		            /* 画 5 条水平网格线 */
		            for (int k = 0; k < 6; ++k)
		            {
		                int y = LCD_Y_ZERO - 25 + 10 * k;
		                lcd_draw_line(5, y, 234, y, 0x07FF);
		            }
		            lcd_draw_line(5,   LCD_Y_ZERO - 25,
		                          5,   LCD_Y_ZERO + 25, 0x07FF);
		            lcd_draw_line(234, LCD_Y_ZERO - 25,
		                          234, LCD_Y_ZERO + 25, 0x07FF);
		        }
		    } /* (end of X-skip block) */

		    /* ===== 6. 周期性刷新文字 ===== */
		    static uint32_t disp_cnt = 0;
		    if (wave_show && (++disp_cnt % 20 == 0))
		    {
		        lcd_show_string(0, 0, 240, 16, 16, data_path, BLUE);
		        /* 显示最近一次 line 字符串 */
		        lcd_show_string(0, 20, 240, 16, 16,
		                        txt_buffer[(txt_idx ? txt_idx : FILE_FLUSH_LINES) - 1],
		                        BLUE);
		    }

		    nums_points++;   /* 统计原始采样速率（时钟中断里显示） */
		}



    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	}
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1) {
	}
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
