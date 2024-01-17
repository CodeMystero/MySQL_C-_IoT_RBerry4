/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define TEMP 												0
#define HUMI 												1
						/* Command set 8*/
#define SHT2x_ADDR									(0x40 << 1) //0b0100 0000 -> 7bit form
#define SHT2x_HOLD_MASTER_T 				0xE3
#define SHT2x_HOLD_MASTER_RH				0xE5
#define SHT2x_NOHOLE_MASTER_T				0xF3
#define SHT2x_NOHOLE_MASTER_RH			0xF5
#define SHT2x_WRITE_USER_REG 				0xE6
#define SHT2x_READ_USER_REG 				0xE7
#define SHT2x_SOFT_RESET 						0xFE

#define STK_CTRL 	*(volatile unsigned int*)0xE000E010
#define STK_LOAD 	*(volatile unsigned int*)0xE000E014
#define STK_VAL 		*(volatile unsigned int*)0xE000E018
#define STK_CALIB  *(volatile unsigned int*)0xE000E01C

#define ENABLE 0
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
FILE __stdout;
uint8_t rx_data;
uint8_t recover;
uint16_t adcVal[2];
float temperature, humidity;
int timer_it = 0;
int i = 0;
int pring_flag = 0;

uint32_t echoTime = 0;
int distance = 0;
uint8_t value = 25;
uint8_t cnt = 0;
uint8_t stop_cnt = 0;
int mode = 0;
int direction = 0;
uint8_t val[2];





/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
int fputc(int ch, FILE* stream){
	HAL_UART_Transmit(&huart2, (uint8_t*)&ch, 1, 0xFFFF);
	return ch;
}
float SHT20(int);

void delay_us(uint16_t);
void HAL_Delay_Porting();
void SysTic_Init();
void Ultra();
void Motor_Auto();
void Motor_Manual();
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
  MX_DMA_Init();
  MX_USART2_UART_Init();
  MX_ADC1_Init();
  MX_I2C1_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_TIM10_Init();
  /* USER CODE BEGIN 2 */
	HAL_UART_Receive_IT(&huart2,&rx_data,1);
	HAL_TIM_Base_Start(&htim3);
	HAL_TIM_Base_Start_IT(&htim10);
	HAL_ADC_Start_DMA(&hadc1,(uint32_t*)adcVal,1);
	
		/*Ultra*/
	TIM3->PSC = 100-1;
	/*Motor*/
	TIM4->PSC = 2000-1;
	TIM4->ARR = 1000-1;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
  while (1)
  {
		
		HAL_Delay(100);
		if(rx_data == 'c'){
			printf("%d\n",adcVal[0]);
			rx_data = NULL;
		}else if(rx_data == 't'){
			temperature = SHT20(TEMP);
			printf("%.2f\n",temperature);
			rx_data = NULL;
		}else if(rx_data == 'h'){
			humidity = SHT20(HUMI);
			printf("%.2f\n",humidity);
			rx_data = NULL;
		}
		
		
		
		if (timer_it == 1) {
			cnt = value+ i;
			TIM4->CCR1 = cnt;
			//Ultra();

			if (i < 45) i += 90;
			else if ( i > 45) i -=90;
			timer_it = 0;
		}
		
		/*else if(rx_data == 'A' || rx_data == 'M' || rx_data == 'L' || rx_data == 'R'){
			
			recover = rx_data;
			
			if(rx_data == 'A') mode = 1;
			else if(rx_data == 'M') mode = 2;
			else if(rx_data == 'L') direction = 1;
			else if(rx_data == 'R') direction = 2;
			
			HAL_Delay(100);
			if(mode == 1) Motor_Auto();
			else if(mode == 2)Motor_Manual();
			
		}*/
		//Ultra();
		
//		if(distance < 5) {
//			stop_cnt = cnt;
//			val[0] = distance;
//			val[1] = ((stop_cnt-25)*180) / 100;
//			printf("%d\n", val[0]);
//			printf("%d\n", val[1]);
//			HAL_Delay(200);
//			
//		}
//		printf("%d\n", distance);		
//		for(int i = 0; i < 90; i+=10) {
//			cnt = value+ i;
//			TIM4->CCR1 = cnt;
//			HAL_Delay(100);
//		}
////		
//		for(int i = 0; i < 100; i +=10) {
//			cnt = 120 - i;
//			TIM4->CCR1 = cnt;
//			HAL_Delay(100);
//		}
////	
//		printf("%d\n", cnt);
				/*
		if(rx_data == 'A') mode = 1;
		else if(rx_data == 'M') mode = 2;
		else if(rx_data == 'L') direction = 1;
		else if(rx_data == 'R') direction = 2;
		*/
		
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 100;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void delay_us(uint16_t us) 
{
		__HAL_TIM_SET_COUNTER(&htim3, 0); 
	while(__HAL_TIM_GET_COUNTER(&htim3)< us ); 
}

void SysTic_Init()
{
	STK_LOAD =  100-1; // 1us
	STK_VAL = 0;
	STK_CTRL = 6;
	uwTick = 0;
}
void HAL_Delay_Porting()
{
	STK_LOAD = 100000 - 1;
	STK_CTRL |= 7;
}
void Ultra()
{
	
	SysTic_Init();
	HAL_GPIO_WritePin(Trig_GPIO_Port, Trig_Pin, 1);
	delay_us(15); 
	HAL_GPIO_WritePin(Trig_GPIO_Port, Trig_Pin, 0);
	while (HAL_GPIO_ReadPin(Echo_GPIO_Port, Echo_Pin) == 0);
	STK_CTRL |= (1 << ENABLE); 		
	while(HAL_GPIO_ReadPin(Echo_GPIO_Port, Echo_Pin) == 1);
	echoTime = HAL_GetTick(); 	
	STK_CTRL &= ~(1<<ENABLE);
		
	/*340m/s - >  340x100cm/1000000us -> 0.034cm/us*/
	distance = echoTime / 2 * 0.034;
	
	//printf("Distance = %d cm \r\n", distance);
		
	HAL_Delay_Porting();
	HAL_Delay(200);
}
void Motor_Auto()
{
//	HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
	for(int i = 0; i < 90; i+=10) {
		//HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
		cnt = value+ i;
		TIM4->CCR1 = cnt;
		//Ultra();
	
		
	}
	
	for(int i = 0; i < 100; i +=10) {
		cnt = 120 - i;
		TIM4->CCR1 = cnt;
		//Ultra();
		
		
	}
}

void Motor_Manual()
{
		TIM4->CCR1 = cnt;
		if(direction == 1) {
			HAL_Delay(100);
			//printf("L_check\n");
			HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
			value += 10;
			cnt = value;
			TIM4->CCR1 = cnt;
			direction = 0;		
		}
		if(direction == 2) {
			HAL_Delay(100);
			//printf("R_check\n");
			HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
			value -= 10;
			cnt = value;
			TIM4->CCR1 = cnt;
			direction = 0;
		}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if(huart->Instance == USART2){
		HAL_UART_Receive_IT(&huart2,&rx_data,1);
		/*
		if(rx_data == 'A') mode = 1;
		else if(rx_data == 'M') mode = 2;
		else if(rx_data == 'L') direction = 1;
		else if(rx_data == 'R') direction = 2;
		*/
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
		timer_it = 1;
}

float SHT20(int select){
	
	uint8_t I2cData[3];
	uint16_t SLAVER_ADDR = SHT2x_ADDR;
	uint16_t sensor;
	float convData = 0.0;
	
	
	if(select == TEMP){
		I2cData[0] = SHT2x_NOHOLE_MASTER_T;
		HAL_I2C_Master_Transmit(&hi2c1, (uint16_t)SLAVER_ADDR, (uint8_t*)I2cData,1,0xFFFF);
		HAL_Delay(100);
		HAL_I2C_Master_Receive(&hi2c1, (uint16_t)SLAVER_ADDR, (uint8_t*)I2cData, 2,0xFFFF);
		sensor = (I2cData[0] << 8)|I2cData[1];
		convData = -46.85+175.72 / 65536 *(float)sensor;
		return convData;
	}
	
	if(select == HUMI){
	I2cData[0] = SHT2x_NOHOLE_MASTER_RH;
		HAL_I2C_Master_Transmit(&hi2c1,(uint16_t)SLAVER_ADDR,(uint8_t*)I2cData,1,0xFFFF);
		HAL_Delay(100);
		HAL_I2C_Master_Receive(&hi2c1,(uint16_t)SLAVER_ADDR,(uint8_t*)I2cData, 2,0xFFFF);
		sensor = (I2cData[0] << 8)|I2cData[1];
		convData = -6 + 125 *((float)sensor/65536);
		return convData;
		
	}
	
	return 0;
	
}
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
