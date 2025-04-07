/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "cmsis_os2.h"
#include "RTE_Components.h"
#include <stdio.h>
#include <stdlib.h>
#ifdef    RTE_VIO_BOARD
#include "cmsis_vio.h"
#endif
#if defined(RTE_Compiler_EventRecorder)
#include "EventRecorder.h"
#endif
#include "../../parson.h"
#include <string.h>
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

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MPU_Config(void);
static void MX_GPIO_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/**
  * Override default HAL_GetTick function
  */
uint32_t HAL_GetTick (void) {
  static uint32_t ticks = 0U;
         uint32_t i;

  if (osKernelGetState () == osKernelRunning) {
    return ((uint32_t)osKernelGetTickCount());
  }

  /* If Kernel is not running wait approximately 1 ms then increment 
     and return auxiliary tick counter value */
  for (i = (SystemCoreClock >> 14U); i > 0U; i--) {
    __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP();
    __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP();
  }
  return ++ticks;
}

/**
  * Override default HAL_InitTick function
  */
HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority) {
  
  UNUSED(TickPriority);

  return HAL_OK;
}


extern unsigned char __heap_base__;
extern unsigned char __heap_limit__;

static unsigned char *heap_end = &__heap_base__;

void check_heap_bounds() {
    unsigned char *heap_start = &__heap_base__;
    unsigned char *heap_end   = &__heap_limit__;

    size_t heap_size = heap_end - heap_start;

    printf("Heap starts at: %p\n", heap_start);
    printf("Heap ends   at: %p\n", heap_end);
    printf("Heap size   is: %u bytes\n", (unsigned int)heap_size);
}

void * brk(ptrdiff_t incr) {
    unsigned char *prev_heap_end = heap_end;
    unsigned char *new_heap_end = heap_end + incr;

    if (new_heap_end > &__heap_limit__) {
        // Out of heap memory!
        printf("_sbrk: heap overflow!\n");
        return (void *)-1;
    }

    heap_end = new_heap_end;
    return (void *)prev_heap_end;
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MPU Configuration--------------------------------------------------------*/
  MPU_Config();

  /* Enable the CPU Cache */

  /* Enable I-Cache---------------------------------------------------------*/
  SCB_EnableICache();

  /* Enable D-Cache---------------------------------------------------------*/
  SCB_EnableDCache();

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  SystemCoreClockUpdate();
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
	printf("Hello World!\r\n");
	void *a=malloc(80000);	
	printf("a is at %p\n", a);
	if(a==NULL)
		printf("malloc failed\n");
	else
	{
		*((unsigned char*)a+399)='Z';
		printf("data in malloc area %c\n",*((unsigned char*)a+2047));
	}
		
	
	check_heap_bounds();

    const char *json = 
 "server:nginx"			
 "{\"zones\":{"
    "\"zone1\":{"
        "\"brightness\":255,"
        "\"color\":{\"blueValue\":0,\"greenValue\":0,\"redValue\":255},"
        "\"currentMode\":\"gradient\","
        "\"gradient1\":{\"blueValue1\":31,\"greenValue1\":255,\"redValue1\":218},"
        "\"gradient2\":{\"blueValue2\":226,\"greenValue2\":61,\"redValue2\":255},"
        "\"powerOn\":true"
    "},"
    "\"zone2\":{"
        "\"brightness\":145,"
        "\"color\":{\"blueValue\":0,\"greenValue\":0,\"redValue\":255},"
        "\"currentMode\":\"rainbow\","
        "\"gradient1\":{\"blueValue1\":43,\"greenValue1\":0,\"redValue1\":255},"
        "\"gradient2\":{\"blueValue2\":255,\"greenValue2\":104,\"redValue2\":66},"
        "\"powerOn\":false"
    "},"
    "\"zone3\":{"
        "\"brightness\":255,"
        "\"color\":{\"blueValue\":0,\"greenValue\":217,\"redValue\":255},"
        "\"currentMode\":\"static\","
        "\"gradient1\":{\"blueValue1\":43,\"greenValue1\":0,\"redValue1\":255},"
        "\"gradient2\":{\"blueValue2\":255,\"greenValue2\":104,\"redValue2\":66},"
        "\"powerOn\":true"
    "}"
"}}"
"end:end";

  const char *firstBrace = strchr(json, '{');


	  // Parse JSON
    JSON_Value *root_value = json_parse_string(firstBrace);
    if (!root_value) {
        printf("Error: Failed to parse JSON\n");
        return 1;
    }

    JSON_Object *root_object = json_value_get_object(root_value);
    JSON_Object *zones = json_object_get_object(root_object,"zones");
		//printf("zones string:%s\n",json_object_get_string(root_object, "zones"));
		

    if (!zones) {
        printf("Error: 'zones' object not found!\n");
        json_value_free(root_value);
        return 1;
    }

    // Iterate through all zones
    size_t num_zones = json_object_get_count(zones);
    for (size_t i = 0; i < num_zones; i++) {
        const char *zone_name = json_object_get_name(zones, i);
        JSON_Object *zone = json_object_get_object(zones, zone_name);

        printf("\nZone: %s\n", zone_name);
        printf("Brightness: %.0f\n", json_object_get_number(zone, "brightness"));
        printf("Current Mode: %s\n", json_object_get_string(zone, "currentMode"));
        printf("Power On: %s\n", json_object_get_boolean(zone, "powerOn") ? "true" : "false");

        // Color
        JSON_Object *color = json_object_get_object(zone, "color");
        printf("Color - Red: %.0f, Green: %.0f, Blue: %.0f\n",
               json_object_get_number(color, "redValue"),
               json_object_get_number(color, "greenValue"),
               json_object_get_number(color, "blueValue"));

        // Gradient 1
        JSON_Object *grad1 = json_object_get_object(zone, "gradient1");
        printf("Gradient1 - Red: %.0f, Green: %.0f, Blue: %.0f\n",
               json_object_get_number(grad1, "redValue1"),
               json_object_get_number(grad1, "greenValue1"),
               json_object_get_number(grad1, "blueValue1"));

        // Gradient 2
        JSON_Object *grad2 = json_object_get_object(zone, "gradient2");
        printf("Gradient2 - Red: %.0f, Green: %.0f, Blue: %.0f\n",
               json_object_get_number(grad2, "redValue2"),
               json_object_get_number(grad2, "greenValue2"),
               json_object_get_number(grad2, "blueValue2"));
    }

    // Free JSON memory
    json_value_free(root_value);
	
	
#ifdef RTE_VIO_BOARD
  vioInit();
#endif

#if defined(RTE_Compiler_EventRecorder) && \
    (defined(__MICROLIB) || \
    !(defined(RTE_CMSIS_RTOS2_RTX5) || defined(RTE_CMSIS_RTOS2_FreeRTOS)))
  EventRecorderInitialize(EventRecordAll, 1U);
#endif

  osKernelInitialize();                         /* Initialize CMSIS-RTOS2 */
  app_initialize();                             /* Initialize application */
  osKernelStart();                              /* Start thread execution */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

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
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 432;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

 /* MPU Configuration */

void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct = {0};

  /* Disables the MPU */
  HAL_MPU_Disable();

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.BaseAddress = 0x20000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_512KB;
  MPU_InitStruct.SubRegionDisable = 0x0;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL1;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Number = MPU_REGION_NUMBER1;
  MPU_InitStruct.BaseAddress = 0xC0000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_16MB;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);
  /* Enables the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
