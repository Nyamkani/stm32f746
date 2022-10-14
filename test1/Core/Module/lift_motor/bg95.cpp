/*
 * bg85.cpp
 *
 *  Created on: Sep 29, 2022
 *      Author: studio3s
 */

#include <lift_motor/bg95.h>

// TODO Auto-generated constructor stub

BG95::BG95(CAN_HandleTypeDef *hcanx)
{
	this->hcanx_ = hcanx;
}


BG95::BG95(CAN_HandleTypeDef *hcanx, int nodeid)
{
	this->hcanx_ = hcanx;
	this->nodeid_ = nodeid;
	//this->Txid_ = Txid;
	//this->Rxid_ = Rxid;
}

// TODO Auto-generated destructor stub
BG95::~BG95()
{
	//if you using heap memory, delete all in this function
	HAL_CAN_DeInitialization();
}


//--------------------------------------------------------------------send or read function
uint16_t BG95::TransmitSendRequest()
{
	CANData_HandleTypeDef temp_DATA = RequestQueue.front();
	CAN_TxHeaderTypeDef TxHeader;
	uint8_t TxData[8];
	uint32_t TxMailbox;
	uint8_t state;

	/* Configure Transmission process */
	TxHeader.StdId = this->nodeid_ + temp_DATA.txid_;                 // Standard Identifier, 0 ~ 0x7FF
	TxHeader.ExtId = 0x01;                  // Extended Identifier, 0 ~ 0x1FFFFFFF
	TxHeader.RTR = CAN_RTR_DATA;            // frame type, DATA or REMOTE
	TxHeader.IDE = CAN_ID_STD;              // identifier type, STD or EXT
	TxHeader.DLC = temp_DATA.data_length_;                       // data length, 0 ~ 8 byte
	TxHeader.TransmitGlobalTime = DISABLE;  // timestamp counter capture.

	/* Set the data to be transmitted */
	std::copy(temp_DATA.write_Data_Byte_, temp_DATA.write_Data_Byte_ + temp_DATA.data_length_, TxData);

	state = HAL_CANTransmit(hcanx_, &TxHeader, (unsigned char*)TxData, &TxMailbox);

	return state;
}

uint16_t BG95::TransmitReceiveResponse()
{
	CAN_RxHeaderTypeDef RxHeader;
	uint8_t RxData[8] = {0,};
	uint8_t state;

	/* Configure Receive process */
	state = HAL_CANReceive(hcanx_, &RxHeader, (unsigned char*)RxData);
	if(state == HAL_OK)
	{
		/* Retrieve the received data */
		RequestQueue.front().rxid_ = this->nodeid_ + RxHeader.StdId;

		std::copy(RequestQueue.front().read_Data_Byte_,
				RequestQueue.front().read_Data_Byte_ + RequestQueue.front().data_length_, RxData);
	}

	return state;
}

//---------------------------------------------------------------Command queue functions
//queue system functions

void BG95::AsyncQueueSaveRequest(CANData_HandleTypeDef cmd){this->RequestQueue.insert(it, cmd);}

void BG95::QueueSaveRequest(CANData_HandleTypeDef cmd){this->RequestQueue.push_back(cmd);}
void BG95::QueueDeleteRequest()
{
	if(!(RequestQueue.empty())) this->RequestQueue.erase(RequestQueue.begin());
}


//--------------------------------------------------------------------initialization
void BG95::HAL_CAN_Initialization()
{
	 /* Can Start */
	/* Activate CAN RX notification */
	if ((HAL_CAN_Start(this->hcanx_) != HAL_OK))// ||
		//	(HAL_CAN_ActivateNotification(this->hcanx_, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK))
	{
		this->err_code_ = initfailed;
	}
}

void BG95::HAL_CAN_DeInitialization()
{
	if (HAL_CAN_Stop(this->hcanx_) != HAL_OK){} //this->err_code_ = initfailed;
}


void BG95::DefaultParamInit()
{
	this->default_param_buff_[0]=1;//-Power Enable(1) Enable
	this->default_param_buff_[1]=1200;//PID Controller-Gain
	this->default_param_buff_[2]=0;//PID Controller-Integral Factor
	this->default_param_buff_[3]=0;//PID Controller-Differential Factor
	this->default_param_buff_[4]=2800;//SVel PI-Controller - Gain
	this->default_param_buff_[5]=0;//SVel PI-Controller - Integral Factor
	this->default_param_buff_[6]=0;//SVel IxR Factor
	this->default_param_buff_[7]=12;//PI-Current Controller - Proportional Factor
	this->default_param_buff_[8]=1;//PI-Current Controller - Integral Factor(�ݵ�� 1 �̻�)
	this->default_param_buff_[9]=30000;//Current Limit - Max. Positiv
	this->default_param_buff_[10]=30000;//Current Limit - Max. Negativ
	this->default_param_buff_[11]=2000;//Velocity Deceleration-Quick-Stop-Delta v  3,000 rpm
	this->default_param_buff_[12]=1000;//Velocity Deceleration-Quick-Stop- Delta t 500mS

	this->nodeid_ = 1;

	this->max_rpm_ = 3800;
	this->acc_rpm_ = 3800;
	this->acc_time_  = 1000;
	this->dec_rpm_  = 3800;
	this->dec_time_ = 1000;
}











//--------------------------------------------------------------------Drive
void BG95::DriveInit()
{
	//ReadBufferInit();
	//DataBufferInit();
	if(RequestQueue.empty()) ReadSchduleCommand();
}


void BG95::DriveComm()
{
	//if(RequestQueue.empty()) return;

	if(TransmitSendRequest()!=HAL_OK || TransmitReceiveResponse()!=HAL_OK)
	{
		this->comm_status_ = false;
		return;
	}
	else
	{
		this->comm_status_ = true;
		return;
	}

}

void BG95::DriveAnalysis()
{
	QueueDeleteRequest();
	//ProcessGetTotalInfo();
}





void BG95::Initialization()
{
	HAL_CAN_Initialization();
	//if(TxData_ == nullptr) TxData_ = new uint8_t[data_length_];
	//if(RxData_ == nullptr) RxData_ = new uint8_t[data_length_];

	//RegisterRequsetCmd();
	//RegisterDefaultParam();

}

void BG95::DeInitialization()
{
	HAL_CAN_DeInitialization();
	//RegisterRequsetCmd();
	//RegisterDefaultParam();

}


void BG95::Drive()
{
	DriveInit();

	DriveComm();

	DriveAnalysis();
}



//-----------------------------------------------------------------------applications
void BG95::WriteDataEnqueue(int index, int subindex, int data)
{
	/*Declare the buffers*/
	CANData_HandleTypeDef cal_data;
	int index_ = index;
	int data_ = data;

	/*input the data to buffer*/
	cal_data.txid_ = 600;
	cal_data.data_length_ = 8;

	cal_data.write_Data_Byte_[0] = 0x40;    					//command byte(4bytes fixed)
	cal_data.write_Data_Byte_[1] = (index_ & 0xff);				//Object Index (lsb)
	cal_data.write_Data_Byte_[2] = ((index_ >> 8) & 0xff);		//Object Index (msb)
	cal_data.write_Data_Byte_[3] = subindex;					//sub Index

	if(data !=0)
	{
		cal_data.write_Data_Byte_[4] = (data_ & 0xff);				//data for lsb
		cal_data.write_Data_Byte_[5] = ((data_ >> 8) & 0xff);
		cal_data.write_Data_Byte_[6] = ((data_ >> 16) & 0xff);
		cal_data.write_Data_Byte_[7] = ((data_ >> 24) & 0xff);
	}

	/*Register data from buffer*/
	QueueSaveRequest(cal_data);
}

void BG95::ReadDataEnqueue(int index, int subindex, int data)
{
	/*Declare the buffers*/
	CANData_HandleTypeDef cal_data;
	int index_ = index;
	int data_ = data;

	/*input the data to buffer*/
	cal_data.txid_ = 580;
	cal_data.data_length_ = 8;

	cal_data.write_Data_Byte_[0] = 0x40;    					//command byte(4bytes fixed)
	cal_data.write_Data_Byte_[1] = (index_ & 0xff);				//Object Index (lsb)
	cal_data.write_Data_Byte_[2] = ((index_ >> 8) & 0xff);		//Object Index (msb)
	cal_data.write_Data_Byte_[3] = subindex;					//sub Index

	if(data !=0)
	{
		cal_data.write_Data_Byte_[4] = (data_ & 0xff);				//data for lsb
		cal_data.write_Data_Byte_[5] = ((data_ >> 8) & 0xff);
		cal_data.write_Data_Byte_[6] = ((data_ >> 16) & 0xff);
		cal_data.write_Data_Byte_[7] = ((data_ >> 24) & 0xff);
	}

	/*Register data from buffer*/
	QueueSaveRequest(cal_data);
}



void BG95::SetPositionCommand()
{
	WriteDataEnqueue(0x3000, 0, 1);      // reset error register
	WriteDataEnqueue(0x3003, 0, 7);      // device mode "position mode"
	WriteDataEnqueue(0x3300, 0, 2500);   // desired velocity (2500 rpm)
	WriteDataEnqueue(0x3732, 0, 1000);   // position following error - window
}

void BG95::ReadSchduleCommand()
{
	ReadDataEnqueue(0x3112, 0, 0);      // motor voltage
	ReadDataEnqueue(0x3113, 0, 0);      // actual motor current
	ReadDataEnqueue(0x3760, 0, 0);      // Actual Target Position
	ReadDataEnqueue(0x3761, 0, 1);      // Actual Command Position
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

void BG95::TestEnque()
{
	WriteDataEnqueue(14178, 0, 12345678);

}











