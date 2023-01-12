/*
 * bg95_CANcmd.h
 *
 *  Created on: Oct 14, 2022
 *      Author: studio3s
 */

#ifndef MODULE_LIFT_MOTOR_BG95_DEFINE_H_
#define MODULE_LIFT_MOTOR_BG95_DEFINE_H_


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

enum CAN_nodeid_reference
{
	NMT_id = 0x00,
	abort_id = 0x80,
	PDO_toHost = 0x180,
	PDO_toNode = 0x200,
	SDO_reading_id = 0x580,
	SDO_sending_id = 0x600,
	EMG_id = 0x700,
};

enum comm_Error_status
{
	COMM_OK = 0x00U,
	HAL_CAN_SEND_ERROR = 0x0001U,
	HAL_CAN_SEND_BUSY = 0x0002U,
	HAL_CAN_SEND_TIMEOUT = 0x0004U,
	HAL_CAN_RECV_ERROR = 0x0008U,
	HAL_CAN_RECV_BUSY = 0x0010U,
	HAL_CAN_RECV_TIMEOUT = 0x0020U,

	COMM_DATA_MISMATCH = 0x0100U,

	CAN_ABORT_ERROR = 0x2000U,
	COMM_RECV_TIMEOUT = 0x8000U,
};

enum CAN_byte_size_reference
{
	CAN_Send_Default_Byte = 0x22,
	CAN_Send_4_Byte = 0x23,
	CAN_Send_2_Byte = 0x2b,
	CAN_Send_1_Byte = 0x2f,

	CAN_Recv_Default_Byte = 0x40,
	CAN_Recv_4_Byte = 0x43,
	CAN_Recv_2_Byte = 0x4b,
	CAN_Recv_1_Byte = 0x4f,
};


//--------------------------------------------3000

enum DSA_Parameters
{
	//General Paramerter
	DEV_Cmd = 0x3000,
	DEV_ErrorReg = 0x3001,
	DEV_Status = 0x3002,
	DEV_Mode = 0x3003,
	DEV_Enable = 0x3004,
	CmdOnError = 0x3010,
	EventOnUserCmd = 0x3014,
	DEV_Code = 0x3020,


	/*Read parameter*/
	IO_AIN_Voltage_Ue = 0x3110,
	IO_AIN_Voltage_Up = 0x3111,
	IO_AIN_Current_Im = 0x3113,


	//Current Controller
	CURR_Kp = 0x3210,
	CURR_Ki = 0x3211,


	CURR_LimitMaxPos = 0x3221,
	CURR_LimitMaxNeg = 0x3223,
	CURR_DynLimit = 0x3224,

	VEL_DesiredValue = 0x3300,
	VEL_LimitMaxPos = 0x3321,
	VEL_LimitMaxNeg = 0x3323,
	VEL_Acc_dV = 0x3340,
	VEL_Acc_dT = 0x3341,
	VEL_Dec_dV = 0x3342,
	VEL_Dec_dT = 0x3343,
	VEL_Dec_QuickStop_dV = 0x3344,
	VEL_Dec_QuickStop_dT = 0x3345,



	VEL_Kp = 0x3310,
	VEL_Ki = 0x3311,
	VEL_Kd = 0x3312,
	VEL_ILimit = 0x3313,
	VEL_Kvff = 0x3314,
	VEL_Kaff = 0x3315,

	VEL_Feedback = 0x3350,

	SVEL_DesiredValue = 0x3500,
	SVEL_Kp = 0x3510,
	SVEL_Ki = 0x3511,
	SVEL_KIxR = 0x3517,
	SVEL_Feedback = 0x3550,

	POS_PositionLimit = 0x3720,


	POS_FollowingErrorWindow = 0x3732,

	POS_ActualTargetPosition = 0x3760,
	POS_ActualCommandPosition = 0x3761,
	POS_ActualPosition = 0x3762,


	POS_Mova = 0x3790,
	POS_Movr = 0x3791,

	MOTOR_Type = 0x3900,
	MOTOR_PolN = 0x3910,

	MOTOR_Polarity = 0x3911,
	MOTOR_ENC_Resolution = 0x3962,

	/*Break management configuration.*/
	Brake_Management = 0x39a0,

	MEASUREMENT_Vel_rpm = 0x3a04,

};


//--------------------------------------------------------Specific data
enum General_data
{
	Disable = 0x00,
	Enable = 0x01,
};

enum Motor_Dir
{
	normal = 0x00,
	reversed = 0x1b,
};


enum DEV_Device_Mode_Data
{
	MODE_OFF = 0x00,
	MODE_Current = 0x02,
	MODE_Vel = 0x03,
	MODE_SVel = 0x05,
	MODE_Pos = 0x07,
};

enum DEV_Cmd_Data
{
	CMD_NOP = 0x00,
	CMD_ClearError = 0x01,
	CMD_QuickStop = 0x02,
	CMD_Halt = 0x03,
	CMD_Continue = 0x04,
	CMD_Update = 0x05,
	CMD_StoreParam = 0x80,
	CMD_RestoreParam = 0x81,
	CMD_DefaultParam = 0x82,
	CMD_ClearParam = 0x83,
};

enum Brake_Management_DOut_Pin
{
	DOut_3_LAtive = 0xfe9d,
	DOut_2_LAtive = 0xfe9e,
	DOut_1_LAtive = 0xfe9f,
	DOut_0_LAtive = 0xfeA0,
	No_Output = 0x00,
	DOut_0_HAtive = 0x0160,
	DOut_1_HAtive = 0x0161,
	DOut_2_HAtive = 0x0162,
	DOut_3_HAtive = 0x0163,
};

//--------------------------------------------------------Sub Index

enum NO_SubIndex
{
	NO_Function = 0x00,
};

enum MOTOR_Type_SubIndix
{
	DC = 0x00,
	BLDC = 0x01,
};




enum CURR_DynLimit_SubIndex
{
	CURR_DynLimitMode = 0x00,
	CURR_DynLimitPeak = 0x01,
	CURR_DynLimitCont = 0x02,
	CURR_DynLimitTime = 0x03,
	CURR_ActualDynLimit = 0x12,
};

enum Brake_Management_SubIndex
{
	BM_Config = 0x00,
	BM_Status = 0x01,
	BM_Dout = 0x08,
	BM_OffDelay1 = 0x10,
	BM_OffDelay2 = 0x11,
	BM_OnDelay1 = 0x12,
	BM_OnDelay2 = 0x13,
	BM_OffOrConditionFlags = 0x18,
	BM_OnOrConditionFlags = 0x1a,
};



enum POS_PositionLimit_SubIndex
{
	Lim_Min = 0,
	Lim_Max = 1,
};



#endif /* MODULE_LIFT_MOTOR_BG95_DEFINE_H_ */
