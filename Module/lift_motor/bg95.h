/*
 * bg85.h
 *
 *  Created on: Sep 29, 2022
 *      Author: kss
 */

#include "stm32f7xx_hal.h"
#include "transmit_tools/transmit_tools.h"

#ifndef MODULE_LIFT_MOTOR_BG95_H_
#define MODULE_LIFT_MOTOR_BG95_H_


enum bg95_err_code
{
	initfailed = 0,

};


typedef struct
{
	int id_;
	int data_length_;
	unsigned char Data_Byte_[8];
} CANData_HandleTypeDef;





class BG95
{
	/*Con., Deconstructor*/
	public:
		BG95(CAN_HandleTypeDef *hcanx, int Txid, int RXid);
		virtual ~BG95();


	private:
		CAN_HandleTypeDef *hcanx_ = NULL;
		//CAN_FilterTypeDef  sFilterConfig;
		CAN_TxHeaderTypeDef TxHeader_;
		CAN_RxHeaderTypeDef RxHeader_;
		uint8_t *TxData_ = NULL;
		uint8_t *RxData_ = NULL;
		uint32_t TxMailbox_;
		uint32_t RxMailbox_;

		int Txid_;
		int Rxid_;

		int err_code_;
		bool comm_status_;



		//queue
		std::vector<CANData_HandleTypeDef> RequestQueue;

	//---------------------------------------------------------------------------------Fuctions
	private:
		//send or read function
		void CAN_DataEnque(uint8_t *pData);


		uint16_t TransmitSendRequest();
		uint16_t TransmitReceiveResponse();

		//queue system functions
		void QueueSaveRequest(char* cmd);
		void QueueDeleteRequest();

		void CAN_TxHandlerSetup();
		void CAN_RxHandlerSetup();
		void HAL_CAN_Initialization();
		void HAL_CAN_DeInitialization();
		void DataBufferInit();

		void DriveInit();
		void DriveComm();
		void DriveAnalysis();

	public:
		void Initialization();
		void DeInitialization();
		void Drive();

};

#endif /* MODULE_LIFT_MOTOR_BG95_H_ */
