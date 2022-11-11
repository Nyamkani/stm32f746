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
	WriteDataEnqueue(0x3004, 0, 0);      // Power Disable

	//Motor Type
	WriteDataEnqueue(0x3900, 0, 1);      // Motor - Type: 1 = brushless Motor (BLDC)
	WriteDataEnqueue(0x3910, 0, 8);      // Motor - Number of Poles (8 - bg95)

	//Encoder feedback enable
	WriteDataEnqueue(0x3550, 0, 2410);      //  Encoder feedback for the velocity controller
	WriteDataEnqueue(0x3550, 0, 2410);      //  Encoder feedback for the secondary velocity controller
	WriteDataEnqueue(0x3962, 0, 4096);      // Encoder resolution in counts: sin/cos = 4096, 1000h

	//WriteDataEnqueue(0x35A1, 0, 20000);      // Maximum Sub Velocity: 20000 = 2.0 m/s

	// b.) Current limits (Attention: Note the motor data sheet!):
	WriteDataEnqueue(0x3221, 0, 50000);      // Current limit - max. positive [mA]
	WriteDataEnqueue(0x3223, 0, 50000);      // Current limit - max. negative [mA]

	WriteDataEnqueue(0x3224, 0, 0);      // Dynamic Current Limit I*t - Mode: 1 = active
	WriteDataEnqueue(0x3224, 1, 12000);      // Dynamic Current Limit I*t - Peak Current [mA]
	WriteDataEnqueue(0x3224, 2, 2640);      // Dynamic Current Limit I*t - Continuous Current [mA]
	WriteDataEnqueue(0x3224, 3, 1000);      // Dynamic Current Limit I*t - Time [ms]

	// c.) Controller parameters
	// Primary Velocity controller
	WriteDataEnqueue(0x3310, 0, 500);      // PID-Position Controller - Proportional gain
	WriteDataEnqueue(0x3311, 0, 50);      // PID-Position Controller - Integral Factor
	WriteDataEnqueue(0x3312, 0, 1);      // PID-Position Controller - Differential gain

	WriteDataEnqueue(0x3313, 0, 10000);      // integration limit of the position controller
	WriteDataEnqueue(0x3314, 0, 1000);      // velocity feed foward factor
	WriteDataEnqueue(0x3315, 0, 1);      // Accelation feed foward factor

	// Secondary Velocity controller
	WriteDataEnqueue(0x3510, 0, 500);      // PI-Velocity Controller - Proportional gain
	WriteDataEnqueue(0x3511, 0, 50);      // PI-Velocity Controller - Integration constant
	WriteDataEnqueue(0x3517, 0, 1);      // compensation factor

	// Current controller
	WriteDataEnqueue(0x3210, 0, 35);      // PI-Current Controller - Proportional gain
	WriteDataEnqueue(0x3211, 0, 2);      // PI-Current Controller - Integration constant

	WriteDataEnqueue(0x3000, 0, 128);      // Stores actual parameters
}

void BG95::RecommendationParamEnqueue()
{
	//----------------------------2.) Recommendation parameters:
	const int32_t max_vel = this->max_vel_;
	const uint32_t acc_rpm = this->acc_rpm_;
	const uint32_t acc_time = this->acc_time_;
	const uint32_t dec_rpm = this->dec_rpm_;
	const uint32_t dec_time = this->dec_time_;
	const uint32_t qdec_rpm = this->qdec_rpm_;
	const uint32_t qdec_time = this->qdec_time_;

	WriteDataEnqueue(0x3300, 0, max_vel);      // Velocity - rpm

	WriteDataEnqueue(0x3321, 0, max_vel);      // Velocity Max. Limit - Positive Direction - rpm
	WriteDataEnqueue(0x3323, 0, max_vel);      // Velocity Max. Limit - Negative Direction - rpm

	WriteDataEnqueue(0x3340, 0, acc_rpm);      // Velocity Acceleration - Delta v
	WriteDataEnqueue(0x3341, 0, acc_time);      // Velocity Acceleration - Delta t

	WriteDataEnqueue(0x3342, 0, dec_rpm);      // Velocity Deceleration - Delta v
	WriteDataEnqueue(0x3343, 0, dec_time);      // Velocity Deceleration - Delta t

	WriteDataEnqueue(0x3344, 0, qdec_rpm);      // Velocity Deceleration - Quick-Stop - Delta v
	WriteDataEnqueue(0x3345, 0, qdec_time);      // Velocity Deceleration - Quick-Stop - Delta t

	WriteDataEnqueue(0x3000, 0, 128);      // Stores actual parameters
}

void BG95::HardwareParamEnqueue()
{

	//save all config.
	WriteDataEnqueue(0x3000, 0, 128);      // Stores actual parameters
	WriteDataEnqueue(0x3000, 0, 5);      // save actual parameters

	//final approach
	WriteDataEnqueue(0x3732, 0, 10000);   // position following error - window


}


void BG95::BreakManagementEnqueue()
{
	/*Manual oepn break*/
	//WriteDataEnqueue(0x3150, 0, 2);      // Open Break

	/*Break management config.*/
	WriteDataEnqueue(0x39a0, 0, 3);      // Brake Management - Configuration - auto disable when the movoment ends

	WriteDataEnqueue(0x39a0, 24, 5);      // Brake Management - Activating Condition
	WriteDataEnqueue(0x39a0, 26, 5);      // Brake Management - Deactivating Condition
	WriteDataEnqueue(0x39a0, 8, -353);      // Brake Management - Brake Output(Digital output 1 High-active)

	WriteDataEnqueue(0x39a0, 16, 200);      // Brake Management - Brake Output
	WriteDataEnqueue(0x39a0, 17, 200);      // Brake Management - Brake Output
	WriteDataEnqueue(0x39a0, 18, 200);      // Brake Management - Brake Output
	WriteDataEnqueue(0x39a0, 19, 200);      // Brake Management - Brake Output

	//WriteDataEnqueue(0x3000, 0, 128);      // Stores actual parameters
}



/*for move mode*/
void BG95::SetPositionControlEnqueue()
{
	int vel_ = this->max_vel_;

	AsyncWriteDataEnqueue(0x3004, 0, 0);      // Power Disable

	AsyncWriteDataEnqueue(0x3000, 0, 1);      // reset error register

	AsyncWriteDataEnqueue(0x3003, 0, 7);      // device mode "position mode"

	AsyncWriteDataEnqueue(0x3300, 0, vel_);   // desired velocity (max_rpm)

	AsyncWriteDataEnqueue(0x3000, 0, 4);      // continue

	AsyncWriteDataEnqueue(0x3004, 0, 1);      // Power Enable
}


void BG95::AbsPosCommandEnqueue(int *tPos)
{
	int tPos_ = 0;

	if(tPos != NULL) tPos_ = *tPos;

	AsyncWriteDataEnqueue(0x3000, 0, 1);      // reset error register

	AsyncWriteDataEnqueue(0x3790, 0, tPos_);   // target position

}

void BG95::RelPosCommandEnqueue(int *tPos)
{
	int tPos_ = 0;

	if(tPos != NULL) tPos_ = *tPos;

	AsyncWriteDataEnqueue(0x3000, 0, 1);      // reset error register

	AsyncWriteDataEnqueue(0x3791, 0, tPos_);   // target position

}

/*for jog mode*/
void BG95::SetVelocityControlEnqueue(bool dir)
{
	bool dir_ = dir;
	int vel_ = this->max_vel_/5;

	if(!dir_) vel_ = vel_*(-1);

	AsyncWriteDataEnqueue(0x3004, 0, 0);      // Power Disable

	AsyncWriteDataEnqueue(0x3000, 0, 1);      // reset error register

	AsyncWriteDataEnqueue(0x3003, 0, 3);      // device mode "velocity mode"

	AsyncWriteDataEnqueue(0x3300, 0, vel_);   // desired velocity (2500 rpm)

	AsyncWriteDataEnqueue(0x3000, 0, 4);      // continue

	AsyncWriteDataEnqueue(0x3004, 0, 1);      // Power Enable
}


void BG95::ReadSchduleCommandEnqueue()
{
	//hardware param
	//ReadDataEnqueue(0x3110, 0, 0);      // actual power voltage
	ReadDataEnqueue(0x3111, 0, 0);      // actual motor voltage

	ReadDataEnqueue(0x3113, 0, 0);      // actual motor current
	ReadDataEnqueue(0x3760, 0, 0);      // Actual Target Position
	//ReadDataEnqueue(0x3761, 0, 0);      // Actual Command Position

	//motor pos.
	ReadDataEnqueue(0x3762, 0, 0);      // Actual Position

	//mptor dir.
	ReadDataEnqueue(0x3911, 0, 0);      // Motor - Polarity(direction)

	//motor dynamics
	ReadDataEnqueue(0x3a04, 1, 0);      // Currently velocity(rpm)

	//setting parameters
	ReadDataEnqueue(0x3300, 0, 0);      // Velocity - rpm

	ReadDataEnqueue(0x3340, 0, 0);      // Velocity Acceleration - Delta v
	ReadDataEnqueue(0x3341, 0, 0);      // Velocity Acceleration - Delta t

	ReadDataEnqueue(0x3342, 0, 0);      // Velocity Deceleration - Delta v
	ReadDataEnqueue(0x3343, 0, 0);      // Velocity Deceleration - Delta t

	ReadDataEnqueue(0x3344, 0, 0);      // Velocity Deceleration - Quick-Stop - Delta v
	ReadDataEnqueue(0x3345, 0, 0);      // Velocity Deceleration - Quick-Stop - Delta t

	//check Device status
	ReadDataEnqueue(0x3001, 0, 0);      //error status

	ReadDataEnqueue(0x3002, 0, 0);      //status register
}



//---------------------------------------------------------------------------------------------------Commands

void BG95::InitializeCommand()
{
	MandatoryParamEnqueue();
	RecommendationParamEnqueue();
	BreakManagementEnqueue();
	HardwareParamEnqueue();

}

void BG95::AbsPosCommand(int *tPos)
{
	SetPositionControlEnqueue();

	AbsPosCommandEnqueue(tPos);

}

void BG95::RelPosCommand(int *tPos)
{
	SetPositionControlEnqueue();

	RelPosCommandEnqueue(tPos);
}

void BG95::VelClockCommand()
{
	SetVelocityControlEnqueue(true);
}

void BG95::VelCClockCommand()
{
	SetVelocityControlEnqueue(false);
}



void BG95::ClearParamCommand()
{
	WriteDataEnqueue(0x3000, 0, 131);
}



void BG95::StopMotorCommand()
{
	AsyncWriteDataEnqueue(0x3000, 0, 3);      // halt
}


void BG95::EMGStopMotorCommand()
{
	AsyncWriteDataEnqueue(0x3000, 0, 2);      // quick stop
}



	/*
	print "ActTrgPos                =", mc.Can.SdoRd(NodeId, 0x3760, 0)
	print "ActCmdPos                =", mc.Can.SdoRd(NodeId, 0x3761, 0)
	print "ActPos                   =", mc.Can.SdoRd(NodeId, 0x3762, 0)
	print "ActPosFollowingError     =", mc.Can.SdoRd(NodeId, 0x3763, 0)
	print "ActIndexPos              =", mc.Can.SdoRd(NodeId, 0x3764, 0)

	print "ActTrgPos_cnt            =", mc.Can.SdoRd(NodeId, 0x3770, 0)
	print "ActCmdPos_cnt            =", mc.Can.SdoRd(NodeId, 0x3771, 0)
	print "ActPos_cnt               =", mc.Can.SdoRd(NodeId, 0x3772, 0)
	print "ActPosFollowingError_cnt =", mc.Can.SdoRd(NodeId, 0x3773, 0)
	print "ActIndexPos_cnt          =", mc.Can.SdoRd(NodeId, 0x3774, 0)
	*/


//write value functions
void BG95::SetMaxVelocityCommand(int32_t vel)
{
	int32_t vel_ = vel;

	WriteDataEnqueue(0x3300, 0, vel_);		//velocity

	WriteDataEnqueue(0x3321, 0, vel_);      // Velocity Max. Limit - Positive Direction - rpm

	WriteDataEnqueue(0x3323, 0, vel_);      // Velocity Max. Limit - Negative Direction - rpm

	WriteDataEnqueue(0x3000, 0, 5);     	//Updates set values
}

void BG95::SetAccelerationCommand(uint32_t acc)
{

	uint32_t acc_ = acc;

	WriteDataEnqueue(0x3340, 0, acc_);      // Velocity Acceleration - Delta v

	WriteDataEnqueue(0x3341, 0, acc_);      // Velocity Acceleration - Delta t

	WriteDataEnqueue(0x3000, 0, 5);     	//Updates set values
}


void BG95::SetDecelerationCommand(uint32_t dec)
{
	uint32_t dec_ = dec;

	WriteDataEnqueue(0x3342, 0, dec_);      // Velocity Deceleration - Delta v

	WriteDataEnqueue(0x3343, 0, dec_);      // Velocity Deceleration - Delta t

	WriteDataEnqueue(0x3000, 0, 5);      	//Updates set values
}


void BG95::SetQuickStopDecelerationCommand(uint32_t qdec)
{
	uint32_t qdec_ = qdec;

	WriteDataEnqueue(0x3344, 0, qdec_);      // Velocity Deceleration - Quick-Stop - Delta v

	WriteDataEnqueue(0x3345, 0, qdec_);      // Velocity Deceleration - Quick-Stop - Delta t

	WriteDataEnqueue(0x3000, 0, 5);      	//Updates set values
}

void BG95::SetPowerEnableCommand()
{
	WriteDataEnqueue(0x3004, 0, 1);      // Power Enable - enable
}

void BG95::SetPowerDisableCommand()
{
	WriteDataEnqueue(0x3004, 0, 0);      // Power Enable - disable
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

void BG95::SetDirectionCommand()
{
	int motor_dir = this->motor_dir_;
	if(motor_dir == 0x1b) motor_dir = 0x00;
	else motor_dir = 0x1b;

	WriteDataEnqueue(0x3004, 0, 0);      // Power Enable - disable

	WriteDataEnqueue(0x3911, 0, motor_dir);      // Motor - Polarity(direction)

	WriteDataEnqueue(0x3004, 0, 1);      // Power Enable - enable
}

void BG95::ResetPositionCommand()
{
	WriteDataEnqueue(0x3762, 0, 0);      // reset actual position
}

void BG95::ResetErrorCommand()
{
	WriteDataEnqueue(0x3000, 0, 1);      // reset error register
}
