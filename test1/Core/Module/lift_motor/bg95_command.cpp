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

	WriteDataEnqueue(0x3900, 0, 1);      // Motor - Type: 1 = brushless Motor (BLDC)
	WriteDataEnqueue(0x3910, 0, 8);      // Motor - Number of Poles (8 - bg95)
	WriteDataEnqueue(0x3962, 0, 4096);      // Encoder resolution in counts: sin/cos = 4096, 1000h
	WriteDataEnqueue(0x3350, 0, 2410);      // Position feedback: 2410 = ENCODER-Feedback (sin/cos)
	WriteDataEnqueue(0x3550, 0, 2410);      // Velocity feedback: 2410 = ENCODER-Feedback (sin/cos)
	WriteDataEnqueue(0x35A1, 0, 20000);      // Maximum velocity: 20000 = 2.0 m/s

	// b.) Current limits (Attention: Note the motor data sheet!):
	WriteDataEnqueue(0x3221, 0, 10000);      // Current limit - max. positive [mA]
	WriteDataEnqueue(0x3223, 0, 10000);      // Current limit - max. negative [mA]

	WriteDataEnqueue(0x3224, 0, 0);      // Dynamic Current Limit I*t - Mode: 1 = active
	WriteDataEnqueue(0x3224, 1, 12000);      // Dynamic Current Limit I*t - Peak Current [mA]
	WriteDataEnqueue(0x3224, 2, 2640);      // Dynamic Current Limit I*t - Continuous Current [mA]
	WriteDataEnqueue(0x3224, 3, 1000);      // Dynamic Current Limit I*t - Time [ms]

	// c.) Controller parameters (recommendations):
	WriteDataEnqueue(0x3310, 0, 500);      // PID-Position Controller - Proportional gain
	WriteDataEnqueue(0x3312, 0, 1);      // PID-Position Controller - Differential gain

	WriteDataEnqueue(0x3313, 0, 1);      // integration limit of the position controller
	WriteDataEnqueue(0x3314, 0, 1000);      // velocity feed foward factor
	WriteDataEnqueue(0x3315, 0, 1);      // Accelation feed foward factor

	WriteDataEnqueue(0x3510, 0, 500);      // PI-Velocity Controller - Proportional gain
	WriteDataEnqueue(0x3511, 0, 50);      // PI-Velocity Controller - Integration constant
	WriteDataEnqueue(0x3517, 0, 1);      // compensation factor

	WriteDataEnqueue(0x3210, 0, 35);      // PI-Current Controller - Proportional gain
	WriteDataEnqueue(0x3211, 0, 2);      // PI-Current Controller - Integration constant

	WriteDataEnqueue(0x3000, 0, 128);      // Stores actual parameters
}

void BG95::RecommendationParamEnqueue()
{
	//----------------------------2.) Recommendation parameters:
	const uint16_t max_rpm = this->max_rpm_;
	const uint16_t acc_rpm_ = this->acc_rpm_;
	const uint16_t acc_time_ = this->acc_time_;
	const uint16_t dec_rpm_ = this->dec_rpm_;
	const uint16_t dec_time_= this->dec_time_;
	const uint16_t qdec_rpm = this->qdec_rpm_;
	const uint16_t qdec_time = this->qdec_time_;

	WriteDataEnqueue(0x3300, 0, max_rpm);      // Velocity - rpm

	WriteDataEnqueue(0x3321, 0, max_rpm);      // Velocity Max. Limit - Positive Direction - rpm
	WriteDataEnqueue(0x3323, 0, max_rpm);      // Velocity Max. Limit - Negative Direction - rpm

	WriteDataEnqueue(0x3340, 0, acc_rpm_);      // Velocity Acceleration - Delta v
	WriteDataEnqueue(0x3341, 0, acc_time_);      // Velocity Acceleration - Delta t

	WriteDataEnqueue(0x3342, 0, dec_rpm_);      // Velocity Deceleration - Delta v
	WriteDataEnqueue(0x3343, 0, dec_time_);      // Velocity Deceleration - Delta t


	WriteDataEnqueue(0x3344, 0, qdec_rpm);      // Velocity Deceleration - Quick-Stop - Delta v
	WriteDataEnqueue(0x3345, 0, qdec_time);      // Velocity Deceleration - Quick-Stop - Delta t

	WriteDataEnqueue(0x3000, 0, 128);      // Stores actual parameters
}

void BG95::HardwareParamEnqueue()
{

	/*Break management config.*/
	//WriteDataEnqueue(0x3154, 0, 2);      // Brake Management - Enable Digital Outputs

	//WriteDataEnqueue(0x39a0, 0, 1);      // Brake Management - Configuration - auto disable when the movoment ends

	//WriteDataEnqueue(0x39a0, 24, 5);      // Brake Management - Activating Condition
	//WriteDataEnqueue(0x39a0, 26, 4);      // Brake Management - Deactivating Condition
	//WriteDataEnqueue(0x39a0, 8, 353);      // Brake Management - Brake Output

	//WriteDataEnqueue(0x39a0, 16, 1000);      // Brake Management - Brake Output
	//WriteDataEnqueue(0x39a0, 17, 1000);      // Brake Management - Brake Output
	//WriteDataEnqueue(0x39a0, 18, 1000);      // Brake Management - Brake Output
	//WriteDataEnqueue(0x39a0, 19, 1000);      // Brake Management - Brake Output

	/*Manual oepn break*/
	WriteDataEnqueue(0x3150, 0, 2);      // Open Break




	//save all config.
	WriteDataEnqueue(0x3000, 0, 128);      // Stores actual parameters
	WriteDataEnqueue(0x3000, 0, 5);      // save actual parameters

	//final approach
	WriteDataEnqueue(0x3732, 0, 1000);   // position following error - window
	WriteDataEnqueue(0x3762, 0, 0);   // reset actual position
	WriteDataEnqueue(0x3000, 0, 1);      // reset error register

}

/*for move mode*/
void BG95::SetPositionControlEnqueue()
{
	int vel_ = this->max_rpm_;

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
	int vel_ = this->max_rpm_/5;

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
	//ReadDataEnqueue(0x3760, 0, 0);      // Actual Target Position
	//ReadDataEnqueue(0x3761, 0, 0);      // Actual Command Position

	//motor pos.
	//ReadDataEnqueue(0x3762, 0, 0);      // Actual Position(
	ReadDataEnqueue(0x396a, 0, 0);      // Actual Position(

	//motor dynamics
	ReadDataEnqueue(0x3a04, 1, 0);      // velocity(rpm)
	ReadDataEnqueue(0x3340, 0, 0);      // Acceleration(dV)
	ReadDataEnqueue(0x3342, 0, 0);      // Deceleration(dV)

	//check Device status
	ReadDataEnqueue(0x3001, 0, 0);      //error status
	ReadDataEnqueue(0x3002, 0, 0);      //status register
}



//---------------------------------------------------------------------------------------------------Commands

void BG95::InitializeCommand()
{
	MandatoryParamEnqueue();
	RecommendationParamEnqueue();
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



void BG95::testvelcommand()
{
	WriteDataEnqueue(0x3000, 0, 4);      // continue
	WriteDataEnqueue(0x3000, 0, 1);      // reset error register
	WriteDataEnqueue(0x3003, 0, 3);      // device mode "vel mode"
	WriteDataEnqueue(0x3000, 0, 4);      // continue
	WriteDataEnqueue(0x3300, 0, 150);   // desired velocity (2500 rpm)
}





void BG95::StopMotorCommand()
{
	//WriteDataEnqueue(0x3000, 0, 2);      // quick stop
	WriteDataEnqueue(0x3000, 0, 3);      // halt
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



