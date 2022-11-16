/*
 * bg95_command.cpp
 *
 *  Created on: Oct 28, 2022
 *      Author: studio3s
 */


#include <lift_motor/bg95.h>



//Enqueue
//-----------------------------------------------------------------------applications





//---------------------------------------------------------------------------------------------------Enqueue
//--------------------------------------------------------------------this parameters are followed from communication manual

void BG95::MandatoryParamEnqueue()
{
	//----------------------------1.) Mandatory parameters:
	// a.) Device parameters:

	//Motor Type
	AsyncWriteDataEnqueue(MOTOR_Type, 0, BLDC);      // Motor - Type: 1 = brushless Motor (BLDC)
	AsyncWriteDataEnqueue(MOTOR_PolN, 0, 8);      // Motor - Number of Poles (8 - bg95)

	//Encoder feedback enable
	AsyncWriteDataEnqueue(VEL_Feedback, 0, 2410);      //  Encoder feedback for the velocity controller
	AsyncWriteDataEnqueue(SVEL_Feedback, 0, 2410);      //  Encoder feedback for the secondary velocity controller
	AsyncWriteDataEnqueue(MOTOR_ENC_Resolution, 0, 4096);      // Encoder resolution in counts: sin/cos = 4096, 1000h

	// b.) Current limits (Attention: Note the motor data sheet!):
	AsyncWriteDataEnqueue(CURR_LimitMaxPos, 0, 50000);      // Current limit - max. positive [mA]
	AsyncWriteDataEnqueue(CURR_LimitMaxNeg, 0, 50000);      // Current limit - max. negative [mA]

	AsyncWriteDataEnqueue(CURR_DynLimit, CURR_DynLimitMode, 0);      // Dynamic Current Limit I*t - Mode: 1 = active
	AsyncWriteDataEnqueue(CURR_DynLimit, CURR_DynLimitPeak, 12000);      // Dynamic Current Limit I*t - Peak Current [mA]
	AsyncWriteDataEnqueue(CURR_DynLimit, CURR_DynLimitCont, 2640);      // Dynamic Current Limit I*t - Continuous Current [mA]
	AsyncWriteDataEnqueue(CURR_DynLimit, CURR_DynLimitTime, 1000);      // Dynamic Current Limit I*t - Time [ms]

	// c.) Controller parameters
	// Primary Velocity controller
	AsyncWriteDataEnqueue(VEL_Kp, 0, 500);      // PID-Position Controller - Proportional gain
	AsyncWriteDataEnqueue(VEL_Ki, 0, 50);      // PID-Position Controller - Integral Factor
	AsyncWriteDataEnqueue(VEL_Kd, 0, 1);      // PID-Position Controller - Differential gain

	AsyncWriteDataEnqueue(VEL_ILimit, 0, 10000);      // integration limit of the position controller
	AsyncWriteDataEnqueue(VEL_Kvff, 0, 1000);      // velocity feed foward factor
	AsyncWriteDataEnqueue(VEL_Kaff, 0, 1);      // Accelation feed foward factor

	// Secondary Velocity controller
	AsyncWriteDataEnqueue(SVEL_Kp, 0, 500);      // PI-Velocity Controller - Proportional gain
	AsyncWriteDataEnqueue(SVEL_Ki, 0, 50);      // PI-Velocity Controller - Integration constant
	AsyncWriteDataEnqueue(SVEL_KIxR, 0, 1);      // compensation factor

	// Current controller
	AsyncWriteDataEnqueue(CURR_Kp, 0, 35);      // PI-Current Controller - Proportional gain
	AsyncWriteDataEnqueue(CURR_Ki, 0, 2);      // PI-Current Controller - Integration constant
}

void BG95::RecommendationParamEnqueue()
{
	//----------------------------2.) Recommendation parameters:
	const int32_t drive_vel = this->drive_vel_;
	const uint32_t acc_rpm = this->acc_rpm_;
	const uint32_t acc_time = this->acc_time_;
	const uint32_t dec_rpm = this->dec_rpm_;
	const uint32_t dec_time = this->dec_time_;
	const uint32_t qdec_rpm = this->qdec_rpm_;
	const uint32_t qdec_time = this->qdec_time_;

	AsyncWriteDataEnqueue(VEL_DesiredValue, 0, drive_vel);      // Velocity - rpm

	AsyncWriteDataEnqueue(VEL_LimitMaxPos, 0, drive_vel);      // Velocity Max. Limit - Positive Direction - rpm
	AsyncWriteDataEnqueue(VEL_LimitMaxNeg, 0, drive_vel);      // Velocity Max. Limit - Negative Direction - rpm

	AsyncWriteDataEnqueue(VEL_Acc_dV, 0, acc_rpm);      // Velocity Acceleration - Delta v
	AsyncWriteDataEnqueue(VEL_Acc_dT, 0, acc_time);      // Velocity Acceleration - Delta t

	AsyncWriteDataEnqueue(VEL_Dec_dV, 0, dec_rpm);      // Velocity Deceleration - Delta v
	AsyncWriteDataEnqueue(VEL_Dec_dT, 0, dec_time);      // Velocity Deceleration - Delta t

	AsyncWriteDataEnqueue(VEL_Dec_QuickStop_dV, 0, qdec_rpm);      // Velocity Deceleration - Quick-Stop - Delta v
	AsyncWriteDataEnqueue(VEL_Dec_QuickStop_dT, 0, qdec_time);      // Velocity Deceleration - Quick-Stop - Delta t

	AsyncWriteDataEnqueue(DEV_Cmd, 0, CMD_Update);      // update velocity parameters
}

void BG95::HardwareParamEnqueue()
{
	//Etc. - error following
	AsyncWriteDataEnqueue(POS_FollowingErrorWindow, 0, 1000);   // position following error - window

	//save all config.
	AsyncWriteDataEnqueue(DEV_Cmd, 0, CMD_StoreParam);      //DeviceCommand -  Stores actual parameters
}


void BG95::BreakManagementEnqueue()
{
	/*Manual oepn break*/
	//WriteDataEnqueue(0x3150, 0, 2);      // Open Break

	/*Break management configuration.*/
	WriteDataEnqueue(Brake_Management, BM_Config, 3);      // Brake Management - Configuration - auto disable when the movoment ends

	WriteDataEnqueue(Brake_Management, BM_OffOrConditionFlags, 5);      // Brake Management - Activating Condition

	WriteDataEnqueue(Brake_Management, BM_OnOrConditionFlags, 5);      // Brake Management - Deactivating Condition

	WriteDataEnqueue(Brake_Management, BM_Dout, DOut_1_LAtive);      // Brake Management - Brake Output(Digital output 1 High-active)

	WriteDataEnqueue(Brake_Management, BM_OffDelay1, 200);      // Brake Management - Brake Output

	WriteDataEnqueue(Brake_Management, BM_OffDelay2, 200);      // Brake Management - Brake Output

	WriteDataEnqueue(Brake_Management, BM_OnDelay1, 200);      // Brake Management - Brake Output

	WriteDataEnqueue(Brake_Management, BM_OnDelay2, 200);      // Brake Management - Brake Output
}





//----------------------------------------------------------------------
/*for Move mode*/
void BG95::SetPositionControlEnqueue()
{
	AsyncWriteDataEnqueue(DEV_Mode, 0, MODE_Pos);      // device mode "position mode"

	AsyncWriteDataEnqueue(DEV_Cmd, 0, CMD_Continue);      // continue
}

void BG95::AbsPosCommandEnqueue(int tPos)
{
	int tPos_ = tPos;

	AsyncWriteDataEnqueue(POS_Mova, 0, tPos_);   // target position
}

void BG95::RelPosCommandEnqueue(int tPos)
{
	int tPos_ = tPos;

	AsyncWriteDataEnqueue(POS_Movr, 0, tPos_);   // target position
}


/*for jog mode*/
void BG95::SetVelocityControlEnqueue(bool dir)
{
	bool dir_ = dir;
	int vel_ = this->drive_vel_/5;

	if(!dir_) vel_ = vel_*(-1);

	AsyncWriteDataEnqueue(0x3004, 0, 0);      // Power Disable

	AsyncWriteDataEnqueue(0x3000, 0, 1);      // reset error register

	AsyncWriteDataEnqueue(0x3003, 0, 3);      // device mode "velocity mode"

	AsyncWriteDataEnqueue(0x3300, 0, vel_);   // desired velocity (2500 rpm)

	AsyncWriteDataEnqueue(0x3000, 0, 4);      // continue

	AsyncWriteDataEnqueue(0x3004, 0, 1);      // Power Enable
}

//0x00 = clock, 0x1b = cclock
void BG95::SetDirectionEnqueue(int motor_dir)
{
	int motor_dir_ = motor_dir;

	AsyncWriteDataEnqueue(MOTOR_Polarity, 0, motor_dir_);      // Motor - Polarity(direction)
}





void BG95::ReadSchduleCommandEnqueue()
{
	//ReadDataEnqueue(0x3110, 0, 0);      // actual power voltage

	ReadDataEnqueue(IO_AIN_Voltage_Up, 0);      // actual motor voltage

	ReadDataEnqueue(IO_AIN_Current_Im, 0);      // actual motor current

	ReadDataEnqueue(POS_ActualTargetPosition, 0);      // Actual Target Position

	//ReadDataEnqueue(POS_ActualCommandPosition, 0, 0);      // Actual Command Position

	ReadDataEnqueue(POS_ActualPosition, 0);      //motor pos. -  Actual Position

	ReadDataEnqueue(MOTOR_Polarity, 0);      // Mptor dir. - Polarity(direction)

	ReadDataEnqueue(MEASUREMENT_Vel_rpm, 1);      //motor dynamics - Currently velocity(rpm)

	ReadDataEnqueue(VEL_DesiredValue, 0);      // Velocity - rpm

	ReadDataEnqueue(VEL_Acc_dV, 0);      // Velocity Acceleration - Delta v

	ReadDataEnqueue(VEL_Acc_dT, 0);      // Velocity Acceleration - Delta t

	ReadDataEnqueue(VEL_Dec_dV, 0);      // Velocity Deceleration - Delta v

	ReadDataEnqueue(VEL_Dec_dT, 0);      // Velocity Deceleration - Delta t

	ReadDataEnqueue(VEL_Dec_QuickStop_dV, 0);      // Velocity Deceleration - Quick-Stop - Delta v

	ReadDataEnqueue(VEL_Dec_QuickStop_dT, 0);      // Velocity Deceleration - Quick-Stop - Delta t

	ReadDataEnqueue(DEV_ErrorReg, 0);      //check Device status - error status

	ReadDataEnqueue(DEV_Status, 0);      //check Device status - status register
}



//---------------------------------------------------------------------------------------------------Commands
void BG95::InitializeCommand()
{
	SetPowerDisableCommand();

	MandatoryParamEnqueue();

	RecommendationParamEnqueue();

	BreakManagementEnqueue();

	HardwareParamEnqueue();

	ResetErrorCommand();

	SetPowerEnableCommand();
}

void BG95::AbsPosCommand(int vel, uint32_t acc, uint32_t dec, int tpos)
{
	int vel_ = vel;
	uint32_t acc_ = acc;
	uint32_t dec_ = dec;
	int tpos_ = tpos;

	SetPowerDisableCommand();

	SetDriveVelocityCommand(vel_);

	SetAccelerationCommand(acc_);

	SetDecelerationCommand(dec_);

	SetPositionControlEnqueue();

	ResetErrorCommand();

	SetPowerEnableCommand();

	AbsPosCommandEnqueue(tpos_);
}

void BG95::RelPosCommand(int vel, uint32_t acc, uint32_t dec, int dir, int tpos)
{
	int vel_ = vel;
	uint32_t acc_ = acc;
	uint32_t dec_ = dec;
	int dir_ = dir;
	int tpos_ = tpos;

	SetPowerDisableCommand();

	SetDriveVelocityCommand(vel_);

	SetAccelerationCommand(acc_);

	SetDecelerationCommand(dec_);

	SetDirectionEnqueue(dir_);

	SetPositionControlEnqueue();

	ResetErrorCommand();

	SetPowerEnableCommand();

	RelPosCommandEnqueue(tpos_);
}



void BG95::VelClockCommand(){SetVelocityControlEnqueue(true);}

void BG95::VelCClockCommand(){SetVelocityControlEnqueue(false);}

void BG95::ClearParamCommand(){WriteDataEnqueue(DEV_Cmd, 0, CMD_ClearParam);}

void BG95::StopMotorCommand(){AsyncWriteDataEnqueue(DEV_Cmd, 0, CMD_Halt);}     // halt

void BG95::EMGStopMotorCommand(){AsyncWriteDataEnqueue(DEV_Cmd, 0, CMD_QuickStop);}    // quick stop


//write value functions
void BG95::SetDriveVelocityCommand(int32_t vel)
{
	int32_t vel_ = vel;

	AsyncWriteDataEnqueue(VEL_DesiredValue, 0, vel_);      // Velocity - rpm

	AsyncWriteDataEnqueue(VEL_LimitMaxPos, 0, vel_);      // Velocity Max. Limit - Positive Direction - rpm

	AsyncWriteDataEnqueue(VEL_LimitMaxNeg, 0, vel_);      // Velocity Max. Limit - Negative Direction - rpm

	AsyncWriteDataEnqueue(DEV_Cmd, 0, CMD_Update);     	//Updates set values
}

void BG95::SetAccelerationCommand(uint32_t acc)
{
	uint32_t acc_ = acc;

	AsyncWriteDataEnqueue(VEL_Acc_dV, 0, acc_);      // Velocity Acceleration - Delta v

	AsyncWriteDataEnqueue(VEL_Acc_dT, 0, acc_);      // Velocity Acceleration - Delta t

	AsyncWriteDataEnqueue(DEV_Cmd, 0, CMD_Update);     	//Updates set values
}

void BG95::SetDecelerationCommand(uint32_t dec)
{
	uint32_t dec_ = dec;

	AsyncWriteDataEnqueue(VEL_Dec_dV, 0, dec_);      // Velocity Deceleration - Delta v

	AsyncWriteDataEnqueue(VEL_Dec_dT, 0, dec_);      // Velocity Deceleration - Delta t

	AsyncWriteDataEnqueue(DEV_Cmd, 0, CMD_Update);     	//Updates set values
}



void BG95::SetQuickStopDecelerationCommand(uint32_t qdec)
{
	uint32_t qdec_ = qdec;

	WriteDataEnqueue(VEL_Dec_QuickStop_dV, 0, qdec_);      // Velocity Deceleration - Quick-Stop - Delta v

	WriteDataEnqueue(VEL_Dec_QuickStop_dT, 0, qdec_);      // Velocity Deceleration - Quick-Stop - Delta t

	AsyncWriteDataEnqueue(DEV_Cmd, 0, CMD_Update);     	//Updates set values
}


void BG95::SetPowerEnableCommand()
{
	AsyncWriteDataEnqueue(DEV_Enable, 0, Enable);      // Power Enable - enable
}

void BG95::SetPowerDisableCommand()
{
	AsyncWriteDataEnqueue(DEV_Enable, 0, Disable);      // Power Enable - disable
}

void BG95::SetPositionMinLimitCommand()
{
	AsyncWriteDataEnqueue(0x3720, 0, this->motor_pos_);      // Position Limit - Minimum
}

void BG95::SetPositionMaxLimitCommand()
{
	AsyncWriteDataEnqueue(0x3720, 1, this->motor_pos_);      // Position Limit - Maximum
}

void BG95::SetInitalPositionCommand()
{
	AsyncWriteDataEnqueue(0x3620, 0, 0);      // Position Limit - Maximum
}

void BG95::ResetPositionCommand()
{
	WriteDataEnqueue(0x3762, 0, 0);      // reset actual position
}

void BG95::ResetErrorCommand()
{
	WriteDataEnqueue(0x3000, 0, 1);      // reset error register
}
