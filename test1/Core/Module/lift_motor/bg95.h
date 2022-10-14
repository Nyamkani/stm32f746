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



/*command byte data id info*/
//-----------------------sdo data structure
//------cob-id
//sending 600h + id
//reading 581h + id
//------for reading data
//sending for reading  - 40h
//receiving date -  42,4f,4b,43h
//------for writing data
//sending for writing  - 22,2f,2b,23h
//receiving date -  60h




enum bg95_err_code
{
	initfailed = 0,

};


typedef struct
{
	int txid_;
	int rxid_;
	int data_length_;
	unsigned char write_Data_Byte_[8] = {0,};
	unsigned char read_Data_Byte_[8] = {0,};
} CANData_HandleTypeDef;





class BG95
{
	/*Con., Deconstructor*/
	public:
		BG95(CAN_HandleTypeDef *hcanx);
		BG95(CAN_HandleTypeDef *hcanx, int nodeid);
		virtual ~BG95();


	private:
		CAN_HandleTypeDef *hcanx_ = NULL;
		//CAN_FilterTypeDef  sFilterConfig;
		uint16_t nodeid_ = 127;

		int err_code_;
		bool comm_status_;

		//-----------------------------------------------------------Dunkor params
		//Dunkor can comm. default params.
		uint16_t default_param_buff_[15]= {0};

		//acc., dec. speed params
		uint16_t max_rpm_;
		uint16_t acc_rpm_ ;
		uint16_t acc_time_;
		uint16_t dec_rpm_ ;
		uint16_t dec_time_;

		//Read parameters
		uint32_t motor_voltage_;	//mV
		uint32_t motor_current_;	//mA

		uint32_t motor_pos_;
		uint16_t motor_vel_;	//rpm
		uint16_t motor_acc_;	//dV[rpm]
		uint16_t motor_dec_;	//dV[rpm]

		//queue(vector)
		std::vector<CANData_HandleTypeDef> RequestQueue;
		std::vector<CANData_HandleTypeDef>::iterator it = RequestQueue.begin();

	//---------------------------------------------------------------------------------Fuctions
	private:
		//send or read function
		void CAN_DataEnque(uint8_t *pData);


		uint16_t TransmitSendRequest();
		uint16_t TransmitReceiveResponse();

		//queue system functions
		void AsyncQueueSaveRequest(CANData_HandleTypeDef cmd);
		void QueueSaveRequest(CANData_HandleTypeDef cmd);
		void QueueDeleteRequest();

		void WriteDataEnqueue(int index, int subindex, int data);
		void ReadDataEnqueue(int index, int subindex, int data);

		void HAL_CAN_Initialization();
		void HAL_CAN_DeInitialization();
		void DataBufferInit();
		void DefaultParamInit();


		void DriveInit();
		void DriveComm();
		void DriveAnalysis();

	public:
		void Initialization();
		void DeInitialization();
		void Drive();
		void TestEnque();

		//test
		void SetPositionCommand();
		void ReadSchduleCommand();

};

#endif /* MODULE_LIFT_MOTOR_BG95_H_ */
