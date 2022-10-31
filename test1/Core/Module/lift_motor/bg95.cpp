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

// TODO Auto-generated destructor stub.
BG95::~BG95()
{
	//if you using heap memory, delete all in this function
	HAL_CAN_DeInitialization();
}


//--------------------------------------------------------------------send or read function
uint16_t BG95::TransmitSendRequest()
{
	CAN_WData_HandleTypeDef temp_DATA = {0};
	CAN_TxHeaderTypeDef TxHeader;
	uint8_t TxData[8] = {0,};
	uint32_t TxMailbox;
	uint8_t state = HAL_ERROR;

	if(!(AsyncRequestQueue.empty())) {temp_DATA =  AsyncRequestQueue.front();}
	else if(!(RequestQueue.empty())) {temp_DATA =  RequestQueue.front();}

	/* Configure Transmission process */
	TxHeader.StdId = this->nodeid_ + temp_DATA.txid_;                 // Standard Identifier, 0 ~ 0x7FF
	TxHeader.ExtId = 0x01;                							  // Extended Identifier, 0 ~ 0x1FFFFFFF
	TxHeader.RTR = CAN_RTR_DATA;          							  // frame type, DATA or REMOTE
	TxHeader.IDE = CAN_ID_STD;            							  // identifier type, STD or EXT
	TxHeader.DLC = temp_DATA.data_length_;                   	      // data length, 0 ~ 8 byte
	TxHeader.TransmitGlobalTime = DISABLE;  					      // timestamp counter capture.

	/* Set the data to be transmitted */
	std::copy(temp_DATA.write_Data_Byte_, temp_DATA.write_Data_Byte_ + temp_DATA.data_length_, TxData);

	/*request transmission of a message*/
	state = HAL_CANTransmit(this->hcanx_, &TxHeader, (unsigned char*)TxData,  &TxMailbox);

	/* Monitor the Tx mailboxes availability until at least one Tx mailbox is free*/
	while(HAL_CAN_GetTxMailboxesFreeLevel(this->hcanx_) != 3);

	//waiting for message to leave
	while((HAL_CAN_IsTxMessagePending((this->hcanx_) , TxMailbox)));

	//waiting for transmission request to be completed by checking RQCPx
	while( !(hcanx_->Instance->TSR & ( 0x1 << (7 * ( TxMailbox - 1 )))));

	return state;
}

uint16_t BG95::TransmitReceiveResponse()
{
	CAN_RxHeaderTypeDef RxHeader;

	uint8_t RxData[8] = {0,};
	uint8_t state = HAL_ERROR;

	CAN_RData_HandleTypeDef cal_data;

	/* Configure Receive process */
	//state = HAL_CANReceive(hcanx_, &RxHeader, (unsigned char*)RxData);

	/* Monitoring queue until at least one message is received */
	//if(HAL_CAN_GetRxFifoFillLevel(this->hcanx_, CAN_RX_FIFO0) == 0) return HAL_ERROR;
	//while(HAL_CAN_GetRxFifoFillLevel(this->hcanx_, CAN_RX_FIFO0) != 1)

	/* Configure Receive process */
	state = HAL_CANReceive(this->hcanx_, &RxHeader, (unsigned char*)RxData);



	/*if data is vaild*/
	if(state == HAL_OK)
	{

		cal_data.rxid_ = RxHeader.StdId;

		cal_data.data_length_ = RxHeader.DLC;

		for(int i =0; i<=7; i++) cal_data.read_Data_Byte_[i] = RxData[i];

		//std::copy(cal_data.read_Data_Byte_,
		//		cal_data.read_Data_Byte_ + cal_data.data_length_, RxData);

		QueueSaveReceive(cal_data);

		/*if data is error data*/
		//if(RxHeader)

		/* Retrieve the received data */
		//RequestQueue.front().rxid_ = this->nodeid_ + RxHeader.StdId;

		//std::copy(RequestQueue.front().read_Data_Byte_,
		//		RequestQueue.front().read_Data_Byte_ + RequestQueue.front().data_length_, RxData);
	}

	return state;
}

//---------------------------------------------------------------Command queue functions
//queue system functions
void BG95::WriteDataEnqueue(int index, int subindex, int data)
{
	/*Declare the buffers*/
	CAN_WData_HandleTypeDef cal_data;
	int txid;
	int index_ = index;
	int subindex_ = subindex;
	int data_ = data;
	int data_size_  = 0;

	/*to check data byte size*/
	if(data_ > 0xffff) { data_size_ = 4;}
	else if(data_ > 0xff) {data_size_ = 2;}
	else { data_size_ = 1;}

	/*to confirm the command byte*/
	switch(data_size_)
	{
		case 1: txid = 0x2f; break;
		case 2: txid = 0x2b; break;
		case 4: txid = 0x23; break;
		default: txid = 0x22; break;
	}

	/*input the data to buffer*/
	cal_data.txid_ = 0x600;   //SDO
	cal_data.data_length_ = 8;

	cal_data.write_Data_Byte_[0] = txid;    					//command byte(4bytes fixed)
	cal_data.write_Data_Byte_[1] = (index_ & 0xff);				//Object Index (lsb)
	cal_data.write_Data_Byte_[2] = ((index_ >> 8) & 0xff);		//Object Index (msb)
	cal_data.write_Data_Byte_[3] = subindex_;					//sub Index

	if(data !=0)
	{
		for(int i = 4; i <= 7; i++) cal_data.write_Data_Byte_[i] = ((data_ >> (8 * (i - 4))) & 0xff);
	}

	/*Register data from buffer*/
	QueueSaveRequest(cal_data);
}

void BG95::AsyncWriteDataEnqueue(int index, int subindex, int data)
{
	/*Declare the buffers*/
	CAN_WData_HandleTypeDef cal_data;
	int txid;
	int index_ = index;
	int subindex_ = subindex;
	int data_ = data;
	int data_size_  = 0;


	/*to confirm the command byte*/
	switch(data_size_)
	{
		case 1: txid = 0x2f; break;
		case 2: txid = 0x2b; break;
		case 4: txid = 0x23; break;
		default: txid = 0x22; break;
	}

	/*input the data to buffer*/
	cal_data.txid_ = 0x600;
	cal_data.data_length_ = 8;

	cal_data.write_Data_Byte_[0] = txid;    					//command byte(4bytes fixed)
	cal_data.write_Data_Byte_[1] = (index_ & 0xff);				//Object Index (lsb)
	cal_data.write_Data_Byte_[2] = ((index_ >> 8) & 0xff);		//Object Index (msb)
	cal_data.write_Data_Byte_[3] = subindex_;					//sub Index

	if(data !=0)
	{
		for(int i = 4; i <= 7; i++) cal_data.write_Data_Byte_[i] = ((data_ >> (8 * (i - 4))) & 0xff);
	}

	/*Register data from buffer*/
	AsyncQueueSaveRequest(cal_data);
}

void BG95::ReadDataEnqueue(int index, int subindex, int data)
{
	/*Declare the buffers*/
	CAN_WData_HandleTypeDef cal_data;
	int index_ = index;
	int data_ = data;

	/*input the data to buffer*/
	cal_data.txid_ = 0x600;
	cal_data.data_length_ = 8;

	cal_data.write_Data_Byte_[0] = 0x40;    					//command byte(4bytes fixed)
	cal_data.write_Data_Byte_[1] = (index_ & 0xff);				//Object Index (lsb)
	cal_data.write_Data_Byte_[2] = ((index_ >> 8) & 0xff);		//Object Index (msb)
	cal_data.write_Data_Byte_[3] = subindex;					//sub Index

	if(data !=0)
	{
		for(int i = 4; i<=7; i++) cal_data.write_Data_Byte_[i] = ((data_ >> (8 * (i - 4))) & 0xff);
	}

	/*Register data from buffer*/
	QueueSaveRequest(cal_data);
}

/*this function is just hardcode. not a final version*/
void BG95::DataAnalysis()
{
	//1.check Receive queue
	if((ReceiveQueue.empty())) return;

	CAN_RData_HandleTypeDef cal_data = ReceiveQueue.front();
	int index = 0;
	int subindex = 0;
	int data = 0;

	//2.check commandbyte is matched(sdo)
	if(cal_data.rxid_ != (this->nodeid_ + 0x580) ) return;


	//3.only check read function for now
	if(!( cal_data.read_Data_Byte_[0] >= 0x40 && cal_data.read_Data_Byte_[0] <= 0x4f )) return;

	//4.
	//index |= cal_data.read_Data_Byte_[1] & 0x00ff;
	//index |= cal_data.read_Data_Byte_[2] & 0xff00 ;
	index |= cal_data.read_Data_Byte_[1];
	index |= cal_data.read_Data_Byte_[2] * 256 ;

	//5.
	subindex = cal_data.read_Data_Byte_[3];

	//6.
	data |= cal_data.read_Data_Byte_[4];
	data |= cal_data.read_Data_Byte_[5] *  256 ;
	data |= cal_data.read_Data_Byte_[6] *  512 ;
	data |= cal_data.read_Data_Byte_[7] *  1024 ;

	DataProcess(index, subindex, data);
}

void BG95::DataProcess(int index, int subindex, int data)
{
	switch(index)
	{
		case 0x3111: this->motor_voltage_ = data ; break;
		case 0x3113: this->motor_current_ = data ; break;
		//case 0x3762: this->motor_pos_ = data ; break;
		case 0x396a: this->motor_pos_ = data ; break;

		case 0x3a04:
			if(subindex == 1) this->motor_vel_ = data ;
			break;

		case 0x3340: this->motor_acc_ = data; break;
		case 0x3342: this->motor_dec_ = data; break;
		case 0x3001: this->err_data_ = (uint16_t)data ; break;
		case 0x3002: this->stat_reg_ = (uint16_t)data ; break;

	}
}




void BG95::AsyncQueueSaveRequest(CAN_WData_HandleTypeDef cmd){this->AsyncRequestQueue.push_back(cmd);}

void BG95::QueueSaveRequest(CAN_WData_HandleTypeDef cmd){this->RequestQueue.push_back(cmd);}
void BG95::QueueDeleteRequest()
{
	/*temporary code*/
	if(!(AsyncRequestQueue.empty())) {this->AsyncRequestQueue.erase(AsyncRequestQueue.begin());}
	else {if(!(RequestQueue.empty())) {this->RequestQueue.erase(RequestQueue.begin());}}


}


void BG95::QueueSaveReceive(CAN_RData_HandleTypeDef cmd){this->ReceiveQueue.push_back(cmd);}
void BG95::QueueDeleteReceive()
{
	if(!(ReceiveQueue.empty())) {this->ReceiveQueue.erase(ReceiveQueue.begin());}
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


//--------------------------------------------------------------------Drive
void BG95::DriveInit()
{
	/*test read function*/
	if(RequestQueue.empty()) ReadSchduleCommandEnqueue();
}


void BG95::DriveComm()
{
	if(TransmitSendRequest()!=HAL_OK)
	{
		this->comm_status_ = false;
	}
	else
	{
		this->comm_status_ = true;
	}

	HAL_Delay(10);

	if(TransmitReceiveResponse()!=HAL_OK)
	{
		this->comm_status_ = false;
	}
	else
	{
		this->comm_status_ = true;
	}
}

void BG95::DriveAnalysis()
{
	DataAnalysis();
	QueueDeleteRequest();
	QueueDeleteReceive();
	//ProcessGetTotalInfo();
}


//--------------------------------------------------------------------Applications
//main functions

void BG95::Initialization()
{
	InitializeCommand();

	HAL_CAN_Initialization();

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

//Read value functions
const uint32_t BG95::GetMotorVoltage() {return this->motor_voltage_;}
const int32_t BG95::GetMotorCurrent() {return this->motor_current_;}
const int32_t BG95::GetMotorPosition() {return this->motor_pos_;}
const int32_t BG95::GetMotorVelocity() {return this->motor_vel_;}
const uint32_t BG95::GetMotorAccelation() {return this->motor_acc_;}
const uint32_t BG95::GetMotorDeceleration() {return this->motor_dec_;}

const uint32_t BG95::GetMotorStatus() {return this->stat_reg_;}
const uint32_t BG95::GetMotorErrData() {return this->err_data_;}

//read status functions
const bool BG95::IsPowerUp() {return (this->stat_reg_&& 0x01);}
const bool BG95::IsErrUp() {return (this->stat_reg_&& 0x02);}
const bool BG95::IsMotorMoving() {return (this->stat_reg_&& 0x04);}
const bool BG95::IsTargetPosReached() {return (this->stat_reg_&& 0x08);}


