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
	WriteDataEnqueue(MOTOR_Type, 0, BLDC);      // Motor - Type: 1 = brushless Motor (BLDC)
	WriteDataEnqueue(MOTOR_PolN, 0, 8);      // Motor - Number of Poles (8 - bg95)

	//Encoder feedback enable
	WriteDataEnqueue(VEL_Feedback, 0, 2410);      //  Encoder feedback for the velocity controller
	WriteDataEnqueue(SVEL_Feedback, 0, 2410);      //  Encoder feedback for the secondary velocity controller
	WriteDataEnqueue(MOTOR_ENC_Resolution, 0, 4096);      // Encoder resolution in counts: sin/cos = 4096, 1000h

	// b.) Current limits (Attention: Note the motor data sheet!):
	WriteDataEnqueue(CURR_LimitMaxPos, 0, 50000);      // Curr ent limit - max. positive [mA]
	WriteDataEnqueue(CURR_LimitMaxNeg, 0, 50000);      // Current limit - max. negative [mA]

	WriteDataEnqueue(CURR_DynLimit, CURR_DynLimitMode, Disable);      // Dynamic Current Limit I*t - Mode: 1 = active
	WriteDataEnqueue(CURR_DynLimit, CURR_DynLimitPeak, 12000);      // Dynamic Current Limit I*t - Peak Current [mA]
	WriteDataEnqueue(CURR_DynLimit, CURR_DynLimitCont, 2640);      // Dynamic Current Limit I*t - Continuous Current [mA]
	WriteDataEnqueue(CURR_DynLimit, CURR_DynLimitTime, 1000);      // Dynamic Current Limit I*t - Time [ms]

	// c.) Controller parameters
	// Primary Velocity controller
	WriteDataEnqueue(VEL_Kp, 0, 500);      // PID-Position Controller - Proportional gain
	WriteDataEnqueue(VEL_Ki, 0, 50);      // PID-Position Controller - Integral Factor
	WriteDataEnqueue(VEL_Kd, 0, 1);      // PID-Position Controller - Differential gain

	WriteDataEnqueue(VEL_ILimit, 0, 10000);      // integration limit of the position controller
	WriteDataEnqueue(VEL_Kvff, 0, 1000);      // velocity feed foward factor
	WriteDataEnqueue(VEL_Kaff, 0, 1);      // Accelation feed foward factor

	// Secondary Velocity controller
	WriteDataEnqueue(SVEL_Kp, 0, 500);      // PI-Velocity Controller - Proportional gain
	WriteDataEnqueue(SVEL_Ki, 0, 50);      // PI-Velocity Controller - Integration constant
	WriteDataEnqueue(SVEL_KIxR, 0, 1);      // compensation factor

	// Current controller
	WriteDataEnqueue(CURR_Kp, 0, 35);      // PI-Current Controller - Proportional gain
	WriteDataEnqueue(CURR_Ki, 0, 2);      // PI-Current Controller - Integration constant
}

void BG95::RecommendationParamEnqueue()
{
	//----------------------------2.) Recommendation parameters:
	const int32_t drive_vel = this->drive_vel_;
	const int32_t max_vel = this->max_vel_;
	const uint32_t acc_rpm = this->acc_rpm_;
	const uint32_t acc_time = this->acc_time_;
	const uint32_t dec_rpm = this->dec_rpm_;
	const uint32_t dec_time = this->dec_time_;
	const uint32_t qdec_rpm = this->qdec_rpm_;
	const uint32_t qdec_time = this->qdec_time_;

	WriteDataEnqueue(VEL_DesiredValue, 0, drive_vel);      // Velocity - rpm

	WriteDataEnqueue(VEL_LimitMaxPos, 0, max_vel);      // Velocity Max. Limit - Positive Direction - rpm
	WriteDataEnqueue(VEL_LimitMaxNeg, 0, max_vel);      // Velocity Max. Limit - Negative Direction - rpm

	WriteDataEnqueue(VEL_Acc_dV, 0, acc_rpm);      // Velocity Acceleration - Delta v
	WriteDataEnqueue(VEL_Acc_dT, 0, acc_time);      // Velocity Acceleration - Delta t

	WriteDataEnqueue(VEL_Dec_dV, 0, dec_rpm);      // Velocity Deceleration - Delta v
	WriteDataEnqueue(VEL_Dec_dT, 0, dec_time);      // Velocity Deceleration - Delta t

	WriteDataEnqueue(VEL_Dec_QuickStop_dV, 0, qdec_rpm);      // Velocity Deceleration - Quick-Stop - Delta v
	AsyncWriteDataEnqueue(VEL_Dec_QuickStop_dT, 0, qdec_time);      // Velocity Deceleration - Quick-Stop - Delta t

	WriteDataEnqueue(DEV_Cmd, 0, CMD_Update);      // update velocity parameters
}

void BG95::BreakManagementEnqueue()
{
	/*Manual oepn break*/
	//AsyncWriteDataEnqueue(0x3150, 0, 2);      // Open Break

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

void BG95::HardwareParamEnqueue()
{
	//Etc. - error following
	WriteDataEnqueue(POS_FollowingErrorWindow, 0, 1000);   // position following error - window

	WriteDataEnqueue(DEV_Mode, 0, MODE_Pos);      // device mode "position mode"

	//save all config.
	WriteDataEnqueue(DEV_Cmd, 0, CMD_StoreParam);      //DeviceCommand -  Stores actual parameters

	WriteDataEnqueue(DEV_Cmd, 0, CMD_Continue);      //DeviceCommand -  Stores actual parameters
}




//----------------------------------------------------------------------
/*for Move mode*/

//0x00 = clock, 0x1b = cclock
void BG95::SetDirectionEnqueue(int motor_dir)
{
	int motor_dir_ = motor_dir;

	AsyncWriteDataEnqueue(MOTOR_Polarity, 0, motor_dir_);      // Motor - Polarity(direction)
}

void BG95::SetPositionControlModeEnqueue()
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
void BG95::SetVelocityControlModeEnqueue()
{
	AsyncWriteDataEnqueue(DEV_Mode, 0, MODE_Vel);      // device mode "sub -velocity mode"

	AsyncWriteDataEnqueue(DEV_Cmd, 0, CMD_Continue);      // continue
}

void BG95::SetSubVelocityControlModeEnqueue()
{
	AsyncWriteDataEnqueue(DEV_Mode, 0, MODE_SVel);      // device mode "sub -velocity mode"

	AsyncWriteDataEnqueue(DEV_Cmd, 0, CMD_Continue);      // continue
}

void BG95::SchduleCommandEnqueue()
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
	WriteDataEnqueue(DEV_Cmd, 0, Enable);  // reset error register

	WriteDataEnqueue(DEV_Enable, 0, Disable);

	MandatoryParamEnqueue();

	RecommendationParamEnqueue();

	BreakManagementEnqueue();

	HardwareParamEnqueue();

	WriteDataEnqueue(DEV_Cmd, 0, Enable);      // reset error register

	WriteDataEnqueue(DEV_Enable, 0, Enable);

	WriteDataEnqueue(DEV_Cmd, 0, CMD_Halt); //stop motor
}

void BG95::AbsPosCommand(int vel, uint32_t acc, uint32_t dec, int tpos)
{
	int vel_ = vel;
	uint32_t acc_ = acc;
	uint32_t dec_ = dec;
	int tpos_ = tpos;

	SetPowerDisableCommand();

	SetVelocityCommand(vel_);

	SetAccelerationCommand(acc_);

	SetDecelerationCommand(dec_);

	SetPositionControlModeEnqueue();

	SetPowerEnableCommand();

	AbsPosCommandEnqueue(tpos_);

	ResetErrorCommand();
}

void BG95::RelPosCommand(int vel, uint32_t acc, uint32_t dec, int tpos)
{
	int vel_ = vel;
	uint32_t acc_ = acc;
	uint32_t dec_ = dec;
	int tpos_ = tpos;

	SetPowerDisableCommand();

	SetVelocityCommand(vel_);

	SetAccelerationCommand(acc_);

	SetDecelerationCommand(dec_);

	SetPositionControlModeEnqueue();

	SetPowerEnableCommand();

	RelPosCommandEnqueue(tpos_);

	ResetErrorCommand();
}

void BG95::VelCommand(int vel, uint32_t acc, uint32_t dec)
{

	int vel_ = vel;
	uint32_t acc_ = acc;
	uint32_t dec_ = dec;

	SetPowerDisableCommand();

	SetVelocityCommand(vel_);

	SetAccelerationCommand(acc_);

	SetDecelerationCommand(dec_);

	SetVelocityControlModeEnqueue();

	SetPowerEnableCommand();

	ResetErrorCommand();
}

void BG95::SubVelCommand(int vel, uint32_t acc, uint32_t dec)
{
	int vel_ = vel;
	uint32_t acc_ = acc;
	uint32_t dec_ = dec;

	SetPowerDisableCommand();

	SetSubVelocityCommand(vel_);

	SetAccelerationCommand(acc_);

	SetDecelerationCommand(dec_);

	SetSubVelocityControlModeEnqueue();

	SetPowerEnableCommand();

	ResetErrorCommand();
}

void BG95::ClearParamCommand(){WriteDataEnqueue(DEV_Cmd, 0, CMD_ClearParam);}

void BG95::StopMotorCommand(){AsyncWriteDataEnqueue(DEV_Cmd, 0, CMD_Halt);}     // halt

void BG95::EMGStopMotorCommand(){AsyncWriteDataEnqueue(DEV_Cmd, 0, CMD_QuickStop);}    // quick stop





//write value functions
void BG95::SetVelocityCommand(int32_t vel)
{
	int32_t vel_ = vel;

	AsyncWriteDataEnqueue(VEL_DesiredValue, 0, vel_);      // Velocity - rpm

	AsyncWriteDataEnqueue(VEL_LimitMaxPos, 0, vel_);      // Velocity Max. Limit - Positive Direction - rpm

	AsyncWriteDataEnqueue(VEL_LimitMaxNeg, 0, vel_);      // Velocity Max. Limit - Negative Direction - rpm

	AsyncWriteDataEnqueue(DEV_Cmd, 0, CMD_Update);     	//Updates set values
}

void BG95::SetSubVelocityCommand(int32_t vel)
{
	int32_t vel_ = vel;

	AsyncWriteDataEnqueue(SVEL_DesiredValue, 0, vel_);      // Velocity - rpm

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

	AsyncWriteDataEnqueue(VEL_Dec_QuickStop_dV, 0, qdec_);      // Velocity Deceleration - Quick-Stop - Delta v

	AsyncWriteDataEnqueue(VEL_Dec_QuickStop_dT, 0, qdec_);      // Velocity Deceleration - Quick-Stop - Delta t

	AsyncWriteDataEnqueue(DEV_Cmd, 0, CMD_Update);     	//Updates set values
}

void BG95::SetDirectionNormalCommand()
{
	int motor_dir_ = 0x00;

	SetPowerDisableCommand();

	AsyncWriteDataEnqueue(MOTOR_Polarity, 0, motor_dir_);      // Motor - Polarity(direction)

	SetPowerEnableCommand();
}

void BG95::SetDirectionReverseCommand()
{
	int motor_dir_ = 0x1b;

	SetPowerDisableCommand();

	AsyncWriteDataEnqueue(MOTOR_Polarity, 0, motor_dir_);      // Motor - Polarity(direction)

	SetPowerEnableCommand();
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
	AsyncWriteDataEnqueue(POS_PositionLimit, Lim_Min, this->motor_pos_);      // Position Limit - Minimum
}

void BG95::SetPositionMaxLimitCommand()
{
	AsyncWriteDataEnqueue(POS_PositionLimit, Lim_Max, this->motor_pos_);      // Position Limit - Maximum
}

/*
void BG95::SetInitalPositionCommand()
{
	AsyncWriteDataEnqueue(POS_ActualPosition, 0, 0);      //reset actual position
}
*/

void BG95::ResetPositionCommand()
{
	AsyncWriteDataEnqueue(POS_ActualPosition, 0, 0);      // reset actual position
}

void BG95::ResetErrorCommand()
{
	WriteDataEnqueue(DEV_Cmd, 0, Enable);      // reset error register
	//AsyncWriteDataEnqueue(DEV_Cmd, 0, Enable);      // reset error register
}
