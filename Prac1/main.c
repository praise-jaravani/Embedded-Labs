/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
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
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdint.h>
#include "stm32f0xx.h"
#include <stdbool.h>
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
TIM_HandleTypeDef htim16;
/* USER CODE BEGIN PV */
const char MASTER_LED_PATTERN = 0xE9;  // 1110 1001
char next_LED_state_num = 0;  // 0 to 8
const uint32_t PERIODS[] = { 500 - 1, 2000 - 1, 1000 - 1 };  // 0.5 sec, 2 sec and 1 sec
const int NO_UPDATE = -1, UPDATE_TO_PERIOD_0 = 0, UPDATE_TO_PERIOD_1 = 1, UPDATE_TO_PERIOD_2 = 2;
int update_period_state = NO_UPDATE;  // default state
bool pause_LED_updates = false;
/* USER CODE END PV */
/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM16_Init(void);
/* USER CODE BEGIN PFP */
void TIM16_IRQHandler(void);
char get_LED_pattern(char state);
void display_LED_pattern(char pattern);
char next_LED_state(void);
void reset_LED_state(void);
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
 MX_TIM16_Init();
 /* USER CODE BEGIN 2 */
 __HAL_TIM_SET_AUTORELOAD(&htim16, PERIODS[UPDATE_TO_PERIOD_0]);
 HAL_TIM_Base_Start_IT(&htim16);  // start timer with interrupts
 /* USER CODE END 2 */
 /* Infinite loop */
 /* USER CODE BEGIN WHILE */
 while (1)
 {
   /* USER CODE END WHILE */
   /* USER CODE BEGIN 3 */
   if (!HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0)) {
   	update_period_state = UPDATE_TO_PERIOD_0;
   }
   if (!HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1)) {
   	update_period_state = UPDATE_TO_PERIOD_1;
   }
   if (!HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_2)) {
   	update_period_state = UPDATE_TO_PERIOD_2;
   }
   if (!HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_3)) {
   	reset_LED_state();
   }
 }
 /* USER CODE END 3 */
}
/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
 LL_FLASH_SetLatency(LL_FLASH_LATENCY_0);
 while(LL_FLASH_GetLatency() != LL_FLASH_LATENCY_0)
 {
 }
 LL_RCC_HSI_Enable();
  /* Wait till HSI is ready */
 while(LL_RCC_HSI_IsReady() != 1)
 {
 }
 LL_RCC_HSI_SetCalibTrimming(16);
 LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
 LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
 LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSI);
  /* Wait till System clock is ready */
 while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_HSI)
 {
 }
 LL_SetSystemCoreClock(8000000);
  /* Update the time base */
 if (HAL_InitTick (TICK_INT_PRIORITY) != HAL_OK)
 {
   Error_Handler();
 }
}
/**
 * @brief TIM16 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM16_Init(void)
{
 /* USER CODE BEGIN TIM16_Init 0 */
 /* USER CODE END TIM16_Init 0 */
 /* USER CODE BEGIN TIM16_Init 1 */
 /* USER CODE END TIM16_Init 1 */
 htim16.Instance = TIM16;
 htim16.Init.Prescaler = 8000-1;
 htim16.Init.CounterMode = TIM_COUNTERMODE_UP;
 htim16.Init.Period = 1000-1;
 htim16.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
 htim16.Init.RepetitionCounter = 0;
 htim16.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
 if (HAL_TIM_Base_Init(&htim16) != HAL_OK)
 {
   Error_Handler();
 }
 /* USER CODE BEGIN TIM16_Init 2 */
 NVIC_EnableIRQ(TIM16_IRQn);
 /* USER CODE END TIM16_Init 2 */
}
/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void)
{
 LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */
 /* GPIO Ports Clock Enable */
 LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOF);
 LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
 LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
 /**/
 LL_GPIO_ResetOutputPin(LED0_GPIO_Port, LED0_Pin);
 /**/
 LL_GPIO_ResetOutputPin(LED1_GPIO_Port, LED1_Pin);
 /**/
 LL_GPIO_ResetOutputPin(LED2_GPIO_Port, LED2_Pin);
 /**/
 LL_GPIO_ResetOutputPin(LED3_GPIO_Port, LED3_Pin);
 /**/
 LL_GPIO_ResetOutputPin(LED4_GPIO_Port, LED4_Pin);
 /**/
 LL_GPIO_ResetOutputPin(LED5_GPIO_Port, LED5_Pin);
 /**/
 LL_GPIO_ResetOutputPin(LED6_GPIO_Port, LED6_Pin);
 /**/
 LL_GPIO_ResetOutputPin(LED7_GPIO_Port, LED7_Pin);
 /**/
 GPIO_InitStruct.Pin = Button0_Pin;
 GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
 GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
 LL_GPIO_Init(Button0_GPIO_Port, &GPIO_InitStruct);
 /**/
 GPIO_InitStruct.Pin = Button1_Pin;
 GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
 GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
 LL_GPIO_Init(Button1_GPIO_Port, &GPIO_InitStruct);
 /**/
 GPIO_InitStruct.Pin = Button2_Pin;
 GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
 GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
 LL_GPIO_Init(Button2_GPIO_Port, &GPIO_InitStruct);
 /**/
 GPIO_InitStruct.Pin = Button3_Pin;
 GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
 GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
 LL_GPIO_Init(Button3_GPIO_Port, &GPIO_InitStruct);
 /**/
 GPIO_InitStruct.Pin = LED0_Pin;
 GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
 GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
 GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
 GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
 LL_GPIO_Init(LED0_GPIO_Port, &GPIO_InitStruct);
 /**/
 GPIO_InitStruct.Pin = LED1_Pin;
 GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
 GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
 GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
 GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
 LL_GPIO_Init(LED1_GPIO_Port, &GPIO_InitStruct);
 /**/
 GPIO_InitStruct.Pin = LED2_Pin;
 GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
 GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
 GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
 GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
 LL_GPIO_Init(LED2_GPIO_Port, &GPIO_InitStruct);
 /**/
 GPIO_InitStruct.Pin = LED3_Pin;
 GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
 GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
 GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
 GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
 LL_GPIO_Init(LED3_GPIO_Port, &GPIO_InitStruct);
 /**/
 GPIO_InitStruct.Pin = LED4_Pin;
 GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
 GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
 GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
 GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
 LL_GPIO_Init(LED4_GPIO_Port, &GPIO_InitStruct);
 /**/
 GPIO_InitStruct.Pin = LED5_Pin;
 GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
 GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
 GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
 GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
 LL_GPIO_Init(LED5_GPIO_Port, &GPIO_InitStruct);
 /**/
 GPIO_InitStruct.Pin = LED6_Pin;
 GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
 GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
 GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
 GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
 LL_GPIO_Init(LED6_GPIO_Port, &GPIO_InitStruct);
 /**/
 GPIO_InitStruct.Pin = LED7_Pin;
 GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
 GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
 GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
 GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
 LL_GPIO_Init(LED7_GPIO_Port, &GPIO_InitStruct);
 /**/
 GPIO_InitStruct.Pin = LL_GPIO_PIN_9;
 GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
 GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
 LL_GPIO_Init(GPIOB, &GPIO_InitStruct);
/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}
/* USER CODE BEGIN 4 */
void display_LED_pattern(char pattern) {
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, pattern % 2);
	pattern = pattern >> 1;
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, pattern % 2);
	pattern = pattern >> 1;
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, pattern % 2);
	pattern = pattern >> 1;
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, pattern % 2);
	pattern = pattern >> 1;
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, pattern % 2);
	pattern = pattern >> 1;
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, pattern % 2);
   pattern = pattern >> 1;
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, pattern % 2);
	pattern = pattern >> 1;
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, pattern);
}
void reset_LED_state() {
	display_LED_pattern(MASTER_LED_PATTERN);  // display first state
	next_LED_state_num = 1;  // next state is the second state
	__HAL_TIM_SET_COUNTER(&htim16, 0);  // resets the timer's counter
}
// Timer rolled over
void TIM16_IRQHandler(void)
{
	// Acknowledge interrupt
	HAL_TIM_IRQHandler(&htim16);
	// if the period of the timer needs to change
	if (update_period_state != NO_UPDATE) {
		uint32_t previous_period = __HAL_TIM_GET_AUTORELOAD(&htim16);
		__HAL_TIM_SET_AUTORELOAD(&htim16, PERIODS[update_period_state]);
		// forces the timer to end its cycle and use the new period
		pause_LED_updates = true;  // pause the interrupt from below from causing LED updates
		__HAL_TIM_SET_COUNTER(&htim16, previous_period);
		update_period_state = NO_UPDATE;
	}
	if (!pause_LED_updates) {
		char pattern = MASTER_LED_PATTERN << next_LED_state_num;
		next_LED_state_num = (next_LED_state_num + 1) % 9;  // updates next LED state number
		display_LED_pattern(pattern);
	} else {
		pause_LED_updates = false;
	}
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

