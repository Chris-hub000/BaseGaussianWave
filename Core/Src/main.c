/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "adc.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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
//#define NUM_POINTS 1000  // 高斯波形点数
//#define A 300.0f         // 波形zui大幅zhi   //100
//#define MU 500.0f         // 波峰中心                  //50.0f
//#define SIGMA 100.0f      // 标准cha      //10
#define M_PI 3.141596

//增加突变
#define PROB_MUTATION 0.4f  //概率
#define MAX_SHIFT   30     //NUM_POINTS/4.0f  //100.0f
#define MAX_INTENSITY 0.5f
#define SIGMA_1 10         //NUM_POINTS/20.0f  //10.0f
#define SIGMA_2 5          //NUM_POINTS/25.0f  //80.0f

//#define AMPLITUDE 300.0f
//#define MEAN 200.0f
//#define NUM_POINTS_1 1000


float gaussian_waveform[NUM_POINTS];  // 高斯波形数据缓冲qu
float gaussian_waveform_2[NUM_POINTS];


// 生成高斯波形数据
//void Generate_Gaussian_Waveform(void)
//{
//    for (int i = 0; i < NUM_POINTS; i++)
//    {
//        float t = (float)i;
//        gaussian_waveform[i] = A * expf(-((t - MU) * (t - MU)) / (2.0f * SIGMA * SIGMA));
//    }
//}

//void Generate_Gaussian_Waveform(void)
//{
//    for (int i = 0; i < NUM_POINTS; i++)
//    {
//        float x = (float)i;
//        gaussian_waveform[i] = AMPLITUDE * expf(-0.5f * powf((x - MEAN) / SIGMA, 2));
//    }
//}




//背景噪声
float Generate_Gaussian_Noise(float sigma)
{
    // 使用 Box-Muller 变换生成高斯噪声
    float U1 = (float)rand() / (float)RAND_MAX;  // [0,1] 的均值分布
    float U2 = (float)rand() / (float)RAND_MAX;
    
    
	//标准正态分布（均值为0，标准差为1）
    float Z = sqrtf(-2.0f * logf(U1)) * cosf(2.0f * M_PI * U2);
		//生成噪声
    return sigma * Z;
}

//增加突变
// 生成高斯函数
float Generate_Gaussian(float x, float mean, float sigma, float amplitude)
{
    return amplitude * expf(-((x - mean) * (x - mean)) / (2.0f * sigma * sigma));
}


// 生成突变高斯信号
//void Add_Mutation(float mutation_mean, float mutation_amplitude, float mutation_sigma)
//{
//    for (int i = 0; i < NUM_POINTS; i++)
//    {
//        float x = (float)i;
//        float mutation_value = mutation_amplitude * expf(-0.5f * powf((x - mutation_mean) / mutation_sigma, 2));

					//叠加突变信号到原始高斯波形
//        gaussian_waveform[i] += mutation_value;
//    }
//}

// 生成带突变的高斯波形
void Generate_Gaussian_With_Mutation(void)
{
	
//	
//	//生成主高斯信号
//    Generate_Gaussian_Waveform();

	//    //添加不同类型的突变
//    //突变1：偏左侧的突变
//    Add_Mutation(150.0f, 0.8f * AMPLITUDE, 30.0f);
//	
	
	
    float mean1 = 100.0f ;  //主高斯波形均值
    float amplitude1 = 100.0f;

	
			

//		// 生成双峰叠加波形
//		for (int i = 0; i < NUM_POINTS; i++)
//		{
//			float t = (float)i;
//			float gaussian1 = Generate_Gaussian(t, mean1, SIGMA_1, amplitude1);
//			float gaussian2 = Generate_Gaussian(t, mean2, SIGMA_2, amplitude2);
//			gaussian_waveform[i] = gaussian1 + gaussian2;
//			if(i>(mean2-30) && i<(mean2+30)) {gaussian_waveform[i] = gaussian1 + gaussian2;}
//			else	{gaussian_waveform[i] = gaussian1 ;}  // 两个波形叠加
//		}
		
		 for (int i = 0; i < NUM_POINTS; i++)
        {
            float t = (float)i;
            gaussian_waveform_2[i] = Generate_Gaussian(t, mean1, SIGMA_1, amplitude1);
        }
		
    // 随机决定是否发生突变（多峰）
			float shift = ((float)rand() / (float)RAND_MAX) * MAX_SHIFT - (MAX_SHIFT / 2.0f);  // 偏移位置
			
			if ((float)rand() / (float)RAND_MAX < PROB_MUTATION)
			{
			
			float amplitude2 = amplitude1 * (((float)rand() / (float)RAND_MAX) * MAX_INTENSITY);  // 第二个高斯 强度
			float mean2 = mean1 - shift;  // 第二个高斯的中心

			// 生成双峰叠加波形
			for (int i = 0; i < NUM_POINTS; i++)
			{
					float t = (float)i;
					float gaussian1 = Generate_Gaussian(t, mean1, SIGMA_1, amplitude1);
					float gaussian2 = Generate_Gaussian(t, mean2, SIGMA_2, amplitude2);
					gaussian_waveform[i] = gaussian1 + gaussian2;  // 两个波形叠加
			}
			}else if((float)rand() / (float)RAND_MAX > PROB_MUTATION+0.1f)
			{
			
				float amplitude2 = amplitude1 * (((float)rand() / (float)RAND_MAX) * MAX_INTENSITY);  // 第二个高斯 强度
				float mean2 = mean1 + shift;  // 第二个高斯的中心

				// 生成双峰叠加波形
				for (int i = 0; i < NUM_POINTS; i++)
				{
						float t = (float)i;
						float gaussian1 = Generate_Gaussian(t, mean1, SIGMA_1, amplitude1);
						float gaussian2 = Generate_Gaussian(t, mean2, SIGMA_2, amplitude2);
						gaussian_waveform[i] = gaussian1 + gaussian2;  // 两个波形叠加
				}
			}
			else
			{
					// 生成单个高斯波形
					for (int i = 0; i < NUM_POINTS; i++)
					{
							float t = (float)i;
							gaussian_waveform[i] = Generate_Gaussian(t, mean1, SIGMA_1, amplitude1);
					}
			}
}




//重定向（但是用不了，编译器问题？）
int fputc(int ch, FILE *f)
{
 
  HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xffff);
 
  return ch;
 
}
void m_printf(uint8_t *buf,char len)
{
	HAL_UART_Transmit(&huart1,&buf[0],len,0xffff);
	
}
void Send_Data_To_Vofa(float data1, float data2)
{
    char TxBuffer[64];  // 字符串缓冲区

    // 格式化数据，使用逗号分隔，方便Vofa+ 解析
    sprintf(TxBuffer, "%.2f,%.2f\n", data1, data2);

	//发送格式化后的字符串数据
    m_printf((uint8_t *)TxBuffer, strlen(TxBuffer));
}

void Send_Data_To_Vofa_1(float data1)
{
    char TxBuffer[64];  // 字符串缓冲区

	//格式化数据，使用逗号分隔，方便vofa+解析
    sprintf(TxBuffer, "%.2f\n", data1);

	//发送格式化后的字符串数据
    m_printf((uint8_t *)TxBuffer, strlen(TxBuffer));
}

//float gaussian(float x, float mu, float sigma) {
//    return (1.0 / (sigma * sqrt(2.0 * M_PI))) * exp(-0.5 * pow((x - mu) / sigma, 2));
//}
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
  MX_DMA_Init();
  MX_USART1_UART_Init();
  MX_TIM2_Init();
  MX_ADC1_Init();
  /* USER CODE BEGIN 2 */
	
	
  /* USER CODE BEGIN SysInit */
		 // 生成高斯波形数据
//	Generate_Gaussian_Waveform();
	Generate_Gaussian_With_Mutation();

		//启动定时器中断
	HAL_TIM_Base_Start_IT(&htim2);
	

//	uint8_t buf[10] = "10,20\r\n";

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
//		m_printf(&buf[0],strlen((char *)buf));

		
		//直接生成高斯波形，会有数据出
//		for (int i = 0; i < NUM_POINTS; i++) {
//			float x = x_min + i * step;
//			float y = gaussian(x, MU, SIGMA);
//			Send_Data_To_Vofa_1(y); // 输出高斯曲线的点
//	}
		
		
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
  RCC_OscInitStruct.PLL.PLLQ = 4;
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
  while (1)
  {
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
