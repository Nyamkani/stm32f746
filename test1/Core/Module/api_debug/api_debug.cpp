/*
 * api_debug.cpp
 *
 *  Created on: Jul 26, 2022
 *      Author: studio3s
 */



#include "../api_debug/api_debug.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>


/*NOTICE*/
/*We will use only USART1 for debugging*/

extern UART_HandleTypeDef huart1; 	//for debugging
extern UART_HandleTypeDef huart3;	//for Controller

static uint8_t str1_;
static uint8_t str3_;

void vprint(const char *fmt, va_list argp)
{
    char string[200];
    if(0 < vsprintf(string,fmt,argp)) // build string
    {
        //HAL_UART_Transmit(&huart1, (uint8_t*)string, strlen(string), 0xffffff); // send message via UART
    	  HAL_UART_Transmit(&huart1, (uint8_t*)string, strlen(string), 10); // send message via UART
    }
}

void Dprintf(const char *fmt, ...) // custom printf() function
{
    va_list argp;
    va_start(argp, fmt);
    vprint(fmt, argp);
    va_end(argp);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == USART1)
	{
		HAL_NVIC_DisableIRQ(USART1_IRQn); //Rx Callback 함수 Disable
		HAL_UART_Transmit_IT(&huart1, &str1_, sizeof(str1_));
		HAL_NVIC_EnableIRQ(USART1_IRQn);  //Rx callback 함수 enable
		HAL_UART_Receive_IT(&huart1, &str1_, sizeof(str1_));

	}
	else if(huart->Instance == USART3)
	{
		//HAL_NVIC_DisableIRQ(USART3_IRQn); //Rx Callback 함수 Disable
		//HAL_NVIC_EnableIRQ(USART3_IRQn);  //Rx callback 함수 enable
		HAL_UART_Receive_IT(&huart3, &str3_, 1);
	}
}





void Debug_Uart_Init()
{
	HAL_UART_Receive_IT(&huart1, &str1_, 1); 	//for debugging
	HAL_UART_Receive_IT(&huart3, &str3_, 1);	//for Controlling
}


//--------------------------------------------------------------test
/* USER CODE BEGIN PV */

extern CAN_HandleTypeDef hcan1;

/* USER CODE END PV */
extern char char_ethernet;
BG95 bg95test(&hcan1);


//for uart1 debug
void DebugDrive()
{
	int cmd = 0;
	int cmd1 = str1_;
	int cmd2 = str3_;
	int cmd3 = char_ethernet;



	if (cmd1 != 0) cmd = cmd1;
	else if (cmd2 !=0) cmd = cmd2;
	else if (cmd3 !=0) cmd = cmd3;

	switch(cmd)
	{
		case '1': SensorManager::GetInstance().PGV100DirStraight(); /*function 1*/ break;
		case '2': SensorManager::GetInstance().PGV100DirLeft();/*function 2*/ break;
		case '3': SensorManager::GetInstance().PGV100DirRight();/*function 2*/ break;
		case '4': SensorManager::GetInstance().PGV100Drive(); break;
		case '5': SensorManager::GetInstance().CommonSensorDrive(); break;
		//can test
		case '6': bg95test.Initialization(); break;
		case '7': bg95test.SetPositionCommand(); break;
		case '8': bg95test.Drive(); break;

		default: break;
	}
	memset(&str1_, '\0', sizeof(str1_));
	memset(&str3_, '\0', sizeof(str3_));
	memset(&char_ethernet, '\0', sizeof(char_ethernet));
}

//for Bluetooth uart3 debug
void BTDebugDrive()
{
	int cmd = str3_;
	switch(cmd)
	{
		case 'a': SensorManager::GetInstance().PGV100DirStraight(); /*function 1*/ break;
		case 'b': SensorManager::GetInstance().PGV100DirLeft();/*function 2*/ break;
		case 'c': SensorManager::GetInstance().PGV100DirRight();/*function 2*/ break;
		default: break;
	}
}










/*
double pgvxpos;
double pgvypos;
double pgvangle;
uint16_t pgvtag;
uint16_t errinfo;

uint16_t sensorsval;
bool sensor1val;

double pcvxpos;
double pcvypos;
*/


/*
double pgvxpos;
double pgvypos;
double pgvangle;
uint16_t pgvtag;
uint16_t errinfo;


bool sensor1val;

double pcvxpos;
double pcvypos;

double pcvAngle;
uint16_t pcverr;
uint16_t pcvtag;
bool errup;
*/


void debug_Command()
{
	/*
	str = str - 48;
	switch(str)
	{
		case 1: SensorManager::GetInstance().PGV100DirStraight(); break;
		case 2: SensorManager::GetInstance().PGV100DirLeft(); break;
		case 3: SensorManager::GetInstance().PGV100DirRight(); break;
		case 4:
			SensorManager::GetInstance().PGV100Drive();

			pgvxpos = SensorManager::GetInstance().PGV100GetXData(0);
			pgvypos = SensorManager::GetInstance().PGV100GetYData(0);
			pcvAngle = SensorManager::GetInstance().PGV100GetAngleData(0);
			pcverr = SensorManager::GetInstance().PGV100GetErrData(0);
			pcvtag = SensorManager::GetInstance().PGV100GetTagData(0);
			errup = SensorManager::GetInstance().PGV100IsErrUp(0);
			break;
*/
	//}
	//str = 0x00;


	//SensorManager::GetInstance().PGV100Register();
	//SensorManager::GetInstance().CommonSensorRegister();
	//auto testmodbus = new HALModbusRtu(&huart1);

	//auto pgv100 = new PNFPosSensor(PCV80, RS485, Port5, GPIOB, GPIO_PIN_0, meter_1, 0.0, 0.0, 0.0);
	//auto sensor1 = new CommonSensor(WORG_R, ActiveH, Filter_5, GPIOG, GPIO_PIN_9);
	//auto pcv80 = new PNFPosSensor(PCV80, RS485, Port5, 0.0, 0.0, 0.0);
	//pgv100->Initializaition();
	//pcv80->Initializaition();

	/*
	SensorManager::GetInstance().PGV100Initialize();
	while(1)
	{
		//pgv100->Drive();
		//pcv80->Drive();
		//pgvxpos = pgv100->GetXPos();
		//pgvypos = pgv100->GetYPos();
		//pgvangle = pgv100->GetAngle();
		//pgvtag = pgv100->GetTagNo();
		//errinfo = pgv100->GetSensorErr();

		//modbus test
		//char tempdata[] = {0x02, 0x05,0x03, 0x08, 0x10};
		//testmodbus->HALModbusRtuMultiRequest(01, 10, tempdata, (sizeof(tempdata)/sizeof(char)));

		//sensor1->Drive();
		//sensor1val = sensor1->GetSensorData();
		//pcvxpos = pcv80->GetXPos();
		//pcvypos= pcv80->GetYPos();

		SensorManager::GetInstance().CommonSensorDrive();
		SensorManager::GetInstance().PGV100Drive();

		//sensorsval = SensorManager::GetInstance().CommonSensorGetAllData();
		//pgvxpos = SensorManager::GetInstance().PNFPosSensorGetXData(0);
		//pgvypos = SensorManager::GetInstance().PNFPosSensorGetYData(0);

		HAL_Delay(10);

	}
	*/
}
