/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.cpp
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
#include <ethernet/tcp_rtos/tcp_rtos.h>
#include <ethernet/udp_rtos/udp_rtos.h>
#include <api_debug/api_debug.h>
#include "main.h"
#include "cmsis_os.h"
#include "lwip.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "init_stm32f7/api_init.h"
#include "sensor_manager/sensor_manager.h"
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


osThreadId InitTaskHandle;
osThreadId CommonSensorTasHandle;
osThreadId PGV100TaskHandle;
osThreadId ETHTaskHandle;
/* USER CODE BEGIN PV */
uint16_t sensorsval;
double pgvxpos = 0.0;
double pgvypos = 0.0;
double pcvAngle =0.0;
uint16_t pcverr = 0;
uint16_t pcvtag = 0;
bool errup = 0;

extern uint8_t str;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/

void StartInitTask(void const * argument);
void StartCommonSensorTask(void const * argument);
void StartPGV100Task(void const * argument);
void StartETHTask(void const * argument);

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

  /* USER CODE END SysInit */

  /* USER CODE BEGIN 2 */

  init_stm32f746();

	Dprintf("Stm32f746 Initializing is complete!\n");

	/* USER CODE BEGIN 5 */
	//debug init
	Debug_Uart_Init();

	Dprintf("Module Initializing\n");
	HAL_Delay(5000);

	//sensor manager initialize
	//need some times for boot pgv100
	SensorManager::GetInstance().CommonSensorInitialize();
	SensorManager::GetInstance().PGV100Initialize();

	Dprintf("Module Initializing is complete!\n");
  /* USER CODE END 2 */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of InitTask */
  osThreadDef(InitTask, StartInitTask, osPriorityRealtime, 0, configMINIMAL_STACK_SIZE);
  InitTaskHandle = osThreadCreate(osThread(InitTask), NULL);

  /* definition and creation of CommonSensorTask */
  osThreadDef(CommonSensorTas, StartCommonSensorTask, osPriorityLow, 0, configMINIMAL_STACK_SIZE);
  CommonSensorTasHandle = osThreadCreate(osThread(CommonSensorTas), NULL);

  /* definition and creation of PGV100Task */
  //osThreadDef(PGV100Task, StartPGV100Task, osPriorityAboveNormal, 0, configMINIMAL_STACK_SIZE);
  //PGV100TaskHandle = osThreadCreate(osThread(PGV100Task), NULL);

  /* definition and creation of ETHTask */
  osThreadDef(ETHTask, StartETHTask, osPriorityNormal, 0, configMINIMAL_STACK_SIZE);
  ETHTaskHandle = osThreadCreate(osThread(ETHTask), NULL);
  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}


/* USER CODE BEGIN Header_StartInitTask */
/**
  * @brief  Function implementing the InitTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartInitTask */
void StartInitTask(void const *argument)
{
	Dprintf("Ethernet Initializing\n");

	/* init code for LWIP */
	MX_LWIP_Init();

	//UdpRtosServerInit();
	UdpClientInit();
	TcpServerInit();

	Dprintf("Ethernet Initializing is complete!\n");

	/* Infinite loop */
	for(;;)
	{
		osDelay(1000);  //1000ms
		//vTaskDelayUntil(&xLastWakeTime, xTime);
		vTaskDelete(NULL);
	}
	/* USER CODE END 5 */
}

/* USER CODE BEGIN Header_StartCommonSensorTask */
/**
* @brief Function implementing the CommonSensorTas thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartCommonSensorTask */
void StartCommonSensorTask(void const *argument)
{
  /* USER CODE BEGIN StartCommonSensorTask */
	const TickType_t xTime = pdMS_TO_TICKS(1);
	TickType_t xLastWakeTime = xTaskGetTickCount();
  /* Infinite loop */
  for(;;)
  {
	//vTaskDelayUntil(&xLastWakeTime, xTime);
	SensorManager::GetInstance().CommonSensorDrive();
	sensorsval = SensorManager::GetInstance().CommonSensorGetAllData();
    osDelay(1);
	vTaskDelayUntil(&xLastWakeTime, xTime);
  }
  /* USER CODE END StartCommonSensorTask */
}

/* USER CODE BEGIN Header_StartPGV100Task */
/**
* @brief Function implementing the PGV100Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartPGV100Task */
void StartPGV100Task(void const *argument)
{
  /* USER CODE BEGIN StartPGV100Task */
	const TickType_t xTime = pdMS_TO_TICKS(50);
	TickType_t xLastWakeTime = xTaskGetTickCount();

  /* Infinite loop */
  for(;;)
  {
	//vTaskDelay(pdMS_TO_TICKS(30));
	SensorManager::GetInstance().PGV100Drive();

	pgvxpos = SensorManager::GetInstance().PGV100GetXData();
	pgvypos = SensorManager::GetInstance().PGV100GetYData();
	pcvAngle = SensorManager::GetInstance().PGV100GetAngleData();
	pcverr = SensorManager::GetInstance().PGV100GetErrData();
	pcvtag = SensorManager::GetInstance().PGV100GetTagData();
	errup = SensorManager::GetInstance().PGV100IsErrUp();
	vTaskDelayUntil(&xLastWakeTime, xTime);
    osDelay(10);
	//vTaskDelay(pdMS_TO_TICKS(80));
  }
  /* USER CODE END StartPGV100Task */
}

/* USER CODE BEGIN Header_StartETHTask */
/**
* @brief Function implementing the ETHTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartETHTask */
void StartETHTask(void const *argument)
{
  /* USER CODE BEGIN StartETHTask */
	const TickType_t xTime = pdMS_TO_TICKS(50);
	TickType_t xLastWakeTime = xTaskGetTickCount();
  /* Infinite loop */
  for(;;)
  {
	  	DebugDrive();

		pgvxpos = SensorManager::GetInstance().PGV100GetXData();
		pgvypos = SensorManager::GetInstance().PGV100GetYData();
		pcvAngle = SensorManager::GetInstance().PGV100GetAngleData();
		pcverr = SensorManager::GetInstance().PGV100GetErrData();
		pcvtag = SensorManager::GetInstance().PGV100GetTagData();
		errup = SensorManager::GetInstance().PGV100IsErrUp();

		//vTaskDelayUntil(&xLastWakeTime, xTime);
		//ethernetif_input(&gnetif);
		//sys_check_timeouts();
		vTaskDelayUntil(&xLastWakeTime, xTime);
		//osDelay(1);
  }
  /* USER CODE END StartETHTask */
}

