/*
 * PNFPosSensor.h
 *
 *  Created on: Aug 17, 2022
 *      Author: studio3s
 */

#ifndef MODULE_OPTICAL_SENSOR_PNFPOSSENSOR_PNFPOSSENSOR_H_
#define MODULE_OPTICAL_SENSOR_PNFPOSSENSOR_PNFPOSSENSOR_H_

#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include <vector>
#include <string>
#include <queue>
#include <main.h>
#include "stm32f7xx_hal.h"

enum err_list
{
	//STATE
	Good = 0x0000,
	ReadHeadTilted = 0x0001,  		//Read head tilted 180°.(pcv80 only)
	CodeConditionErr = 0x0002, 		//code condition error(code distance chk)
	NoDirectionDeclared = 0x0004,    	//No DIR. decision(Set POS.Sensor DIR.)
	NoColorDeclared = 0x0008,  		//No Color decision(Set Color choice)
	OutOfRange = 0x0010,      		//Out of Range
	NoPosition = 0x0020,      		//No Position
	CommTimeout = 0x0040,  			//Timeout(communication error)
	CheckSumErr = 0x0080,  			//chk_sum error ;
	InternalFatal = 0x1000,  			//internal error (Recommend to change sensors)
	//0x2000 = reserved
	//0x4000 = reserved
	//0x8000 = reserved
	//--------------------------------------------------------------------
};




class PNFPosSensor
{

	protected:
		//Basic constructor
		PNFPosSensor();
		virtual ~PNFPosSensor();

		//---------------------------------------------------------------------------Common parameters. declation
		//Common params
		uint16_t index_;

		//filter params
		uint16_t max_filter_cnt_ = 5;
		uint16_t now_filter_cnt_= 0;

		//to see useful values
		double xpos_ = 0;
		double ypos_ = 0;

		//to see err status
		uint16_t err_code_ = 0;
		bool comm_status_ = true;  //good

		//---------------------------------------------------------------------------pgv100 parameters. declation
		//params
		double  x_offset_;
		double  y_offset_;
		uint16_t comm_type_;
		uint16_t unit_= 10000;

		//if communication direction pins available
		bool comm_dir_available_;
		GPIO_TypeDef* GPIO_ = NULL;
		uint16_t dir_pin_no_;

		//uart transmit Buffer
		UART_HandleTypeDef *huartx_ = NULL;


		//Receive Buffer
		std::vector<uint16_t> pos_buf_;      // Response POS data buffer
		uint16_t max_read_buf_size_;

		//---------------------------------------------------------------------------485 Comm. cmds declation
		//Values for request cmd
		std::vector<uint16_t> RequestCmd;
		std::queue<uint16_t> RequestQueue;

		//---------------------------------------------------------------------------functions

		//send or read function
		uint16_t TransmitSendRequest();
		uint16_t TransmitReceiveResponse();

		//queue system functions
		void QueueSaveRequest(uint16_t cmd);
		void QueueDeleteRequest();

		bool IsInfoFiltered();
		void FilterCountUp();
		void FilterCountReset();



	public:
		//virtual function for each sensors
		virtual void DriveInit() = 0;
		virtual void DriveComm() = 0;
		virtual void DriveAnalysis() = 0;

		//main functions
		virtual void Initialization() = 0;
		bool IsErrUp();
		uint16_t Drive();


};

#endif /* MODULE_OPTICAL_SENSOR_PNFPOSSENSOR_PNFPOSSENSOR_H_ */
