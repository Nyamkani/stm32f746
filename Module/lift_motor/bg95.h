/*
 * bg85.h
 *
 *  Created on: Sep 29, 2022
 *      Author: kss
 */

#include "stm32f7xx_hal.h"
#include "transmit_tools/transmit_tools.h"

#include <lift_motor/bg95_define.h>

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

enum bg95_send_type
{
	none = 0,
	async = 1,
	sync = 2,
};

enum bg95_status
{
	initfailed = 0,
	initdone = 1,
	send_ready = 2,
	send_failed= 3,
	recv_ready = 4,
	recv_failed = 5,
	analysis_ready = 6,
	adnalysis_failed = 7,
	error = 10,
};

enum nodeid_reference
{
	NMT_id = 0x00,
	abort_id = 0x80,
	PDO_toHost = 0x180,
	PDO_toNode = 0x200,
	SDO_reading_id = 0x580,
	SDO_sending_id = 0x600,
	EMG_id = 0x700,
};

enum CAN_Error_status
{
	//HAL_OK = 0,
	//HAL_ERROR = 1,
	//HAL_BUSY = 2,
	//HAL_TIMEOUT = 3,
	Recv_TIMEOUT = 0x04,
};






typedef struct
{
	int txid_;
	int data_length_;
	unsigned char write_Data_Byte_[8] = {0,};
} CAN_WData_HandleTypeDef;

typedef struct
{
	int rxid_;
	int data_length_;
	unsigned char read_Data_Byte_[8] = {0,};
} CAN_RData_HandleTypeDef;







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

		//device software status
		bool is_init_ = false;
		bool is_run_ = false;
		bool is_err_ = false;

		//device communication status
		bool is_send_ready_ = false;
		int CAN_status_;
		int CAN_status_filter;
		uint32_t comm_timestamp_ = 0;
		uint32_t comm_timeout_ = 150;  //ms
		uint8_t drive_tick = 0 ;
		int comm_num_try = 0;
		int comm_max_try = 2;


		//-----------------------------------------------------------Dunkor params
		//Dunkor can comm. Device Parameters.

		//acc., dec. speed params
		int32_t actual_max_vel_;
		uint32_t acc_rpm_;
		uint32_t acc_time_;
		uint32_t dec_rpm_;
		uint32_t dec_time_;
		uint32_t qdec_rpm_;
		uint32_t qdec_time_;

		//Only Read parameters
		uint32_t motor_voltage_ = 0;	//mV
		int32_t motor_current_ = 0 ;	//mA

		int32_t motor_pos_;	//present postiion
		int32_t target_pos_; //target position
		int32_t motor_vel_;	//rpm

		//device status
		uint32_t comm_stat_reg_;
		uint32_t comm_err_data_;
		uint32_t dev_stat_reg_;
		uint32_t dev_err_data_;

		//mode
		int32_t max_vel_;
		int32_t drive_vel_;
		int motor_dir_;

		//queue(vector)
		int send_queue_type_;
		std::vector<CAN_WData_HandleTypeDef> AsyncRequestQueue;
		std::vector<CAN_WData_HandleTypeDef> RequestQueue;

		//Send data Buffer
		CAN_WData_HandleTypeDef send_data_buffer;

		//Receive data
		std::vector<CAN_RData_HandleTypeDef> ReceiveQueue;

	//---------------------------------------------------------------------------------Fuctions
	private:
		//send or read function
		void IntializeParameters();

		uint16_t TransmitSendRequest();
		uint16_t TransmitReceiveResponse();

		//queue system functions
		int SelectSendQueueType();
		int SelectSendQueueData();

		CAN_WData_HandleTypeDef SelectSendQueueData(int type);

		void AsyncQueueSaveRequest(CAN_WData_HandleTypeDef cmd);
		void QueueSaveRequest(CAN_WData_HandleTypeDef cmd);
		void QueueDeleteRequest();
		bool IsAsyncRequestQueueEmpty();
		bool IsRequestQueueEmpty();

		void QueueSaveReceive(CAN_RData_HandleTypeDef cmd);
		void QueueDeleteReceive();
		bool IsReceiveQueueEmpty();
		void QueueChangeReceive();

		bool IsSendTickReached();
		bool IsRecvTimedOut();





		void WriteDataEnqueue(int index, int subindex, int data);
		void AsyncWriteDataEnqueue(int index, int subindex, int data);
		void ReadDataEnqueue(int index, int subindex);

		bool HAL_CAN_Initialization();
		bool HAL_CAN_DeInitialization();

		void SendProcess();
		void RecvProcess();

		void DriveInit();
		void DriveComm();
		void DriveAnalysis();

		bool CheckReceivedReadFunction();
		void CheckReceivedNodeId();
		bool CheckCommandData();
		void CheckCANState();


		bool DataAnalysis();
		void RecvDataProcess();
		void DataProcess(int index, int subindex, int data);


		void StatusCheck();


	public:
		bool Initialization();
		void DeInitialization();
		void Drive();

		//Enqueue
	private:
		void MandatoryParamEnqueue();
		void RecommendationParamEnqueue();
		void HardwareParamEnqueue();
		void BreakManagementEnqueue();
		void SetPositionControlModeEnqueue();
		void SetVelocityControlModeEnqueue();
		void SetSubVelocityControlModeEnqueue();
		void AbsPosCommandEnqueue(int tPos);
		void RelPosCommandEnqueue(int tPos);
		void SetDirectionEnqueue(int motor_dir);

		void ReadSchduleCommandEnqueue();


	public:
		//command function
		void AbsPosCommand(int vel, uint32_t acc, uint32_t dec, int tpos);
		void RelPosCommand(int vel, uint32_t acc, uint32_t dec, int tpos);
		void VelCommand(int vel, uint32_t acc, uint32_t dec);
		void SubVelCommand(int vel, uint32_t acc, uint32_t dec);

		void StopMotorCommand();
		void EMGStopMotorCommand();

		void InitializeCommand();

		void ClearParamCommand();


		//Read functions
		const uint32_t GetMotorVoltage();
		const int32_t GetMotorCurrent();
		const int32_t GetMotorPosition();
		const int32_t GetTargetPosition();
		const int32_t GetMotorVelocity();
		const uint32_t GetMotorAccelation();
		const uint32_t GetMotorDeceleration();

		const uint32_t GetMotorStatus();
		const uint32_t GetMotorErrData();

		void SetVelocityCommand(int32_t vel);
		void SetSubVelocityCommand(int32_t vel);
		void SetAccelerationCommand(uint32_t aec);
		void SetDecelerationCommand(uint32_t dec);
		void SetQuickStopDecelerationCommand(uint32_t qdec);

		void SetDirectionNormalCommand();
		void SetDirectionReverseCommand();

		void SetPowerEnableCommand();
		void SetPowerDisableCommand();

		void SetZeroPositionCommand();

		void SetPositionMinLimitCommand();
		void SetPositionMaxLimitCommand();
		//void SetInitalPositionCommand();


		void ResetPositionCommand();
		void ResetErrorCommand();


		//Internal Status Check
		const bool IsInitTrue();
		const bool IsRunTrue();
		const bool IsErrTrue();

		//read status functions
		const bool IsPowerUp();
		const bool IsErrUp();
		const bool IsMotorMoving();
		const bool IsTargetPosReached();
		const bool IsMotorReachLimit();

};

#endif /* MODULE_LIFT_MOTOR_BG95_H_ */
