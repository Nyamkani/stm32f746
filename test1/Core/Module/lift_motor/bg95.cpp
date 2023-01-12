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


BG95::BG95(CAN_HandleTypeDef *hcanx, int nodeid) // @suppress("Class members should be properly initialized")
{
	this->hcanx_ = hcanx;
	this->nodeid_ = nodeid;
}

// TODO Auto-generated destructor stub.
BG95::~BG95()
{
	//if you using heap memory, delete all in this function
	HAL_CAN_DeInitialization();
}

//--------------------------------------------------------------------Initialization member variables
void BG95::IntializeParameters()
{
	//acc., dec. speed params
	this->actual_max_vel_ = 0;
	this->acc_rpm_ = 0;
	this->acc_time_ = 0;
	this->dec_rpm_ = 0;
	this->dec_time_ = 0;
	this->qdec_rpm_ = 0;
	this->qdec_time_ = 0;

	this->max_vel_ = 3000;
	this->drive_vel_ = 1000;
	this->motor_dir_ = 0x00;


	//device status
	this->comm_stat_reg_ = COMM_OK;
	this->dev_stat_reg_ = 0;
	this->dev_err_data_ = 0;
	this->module_error_data_ = 0;

	this->is_init_ = false;
	this->is_run_= false;
	this->is_err_ = false;
	this->is_send_ready_ = false;

	AsyncRequestQueue.clear();

	RequestQueue.clear();

	ReceiveQueue.clear();

	return;
}

bool BG95::HAL_CAN_Initialization()
{
	/* CAN Start */
	return (HAL_CAN_Start(this->hcanx_));
}

bool BG95::HAL_CAN_DeInitialization()
{
	/* CAN Stop */
	return (HAL_CAN_Stop(this->hcanx_));
}



//--------------------------------------------------------------------send or read function
uint16_t BG95::SendRequest()
{
	CAN_TxHeaderTypeDef TxHeader;
	uint8_t TxData[8] = {0,};
	uint32_t TxMailbox;
	uint8_t state = HAL_ERROR;
	uint8_t send_queue_type = this->send_queue_type_;

	/*get the sending data*/
	CAN_WData_HandleTypeDef temp_DATA = SelectSendQueueData(send_queue_type);

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

	/*to save error state from send request*/
	switch(state)
	{
		case HAL_OK: state = COMM_OK; break;

		case HAL_ERROR: state = HAL_CAN_SEND_ERROR; break;

		case HAL_BUSY: state = HAL_CAN_SEND_BUSY; break;

		case HAL_TIMEOUT: state = HAL_CAN_SEND_TIMEOUT; break;
	}

	/*If state is ok. Save temp data*/
	if(state == COMM_OK) this->send_data_buffer = temp_DATA;

	/*Store current state*/
	this->comm_stat_reg_ |= state;

	return state;
}

uint16_t BG95::RecvResponse()
{
	CAN_RxHeaderTypeDef RxHeader;
	CAN_RData_HandleTypeDef cal_data;
	uint8_t RxData[8] = {0,};
	uint8_t state = HAL_ERROR;

	/* Configure Receive process */
	state = HAL_CANReceive(this->hcanx_, &RxHeader, (unsigned char*)RxData);

	/*to save error state from send request*/
	switch(state)
	{
		case HAL_OK: state = COMM_OK; break;

		case HAL_ERROR: state = HAL_CAN_RECV_ERROR; break;

		case HAL_BUSY: state = HAL_CAN_RECV_BUSY; break;

		case HAL_TIMEOUT: state = HAL_CAN_RECV_TIMEOUT; break;
	}

	/*If state is ok. Save temp data to receive queue*/
	if(state == COMM_OK)
	{
		cal_data.rxid_ = RxHeader.StdId;

		cal_data.data_length_ = RxHeader.DLC;

		for(int i =0; i<=7; i++) cal_data.read_Data_Byte_[i] = RxData[i];

		QueueSaveReceive(cal_data);
	}

	/*Store current state*/
	this->comm_stat_reg_ |= state;

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
	if(data_ > 0xffff || data_< 0) { data_size_ = 4;}
	else if(data_ > 0xff) {data_size_ = 2;}
	else { data_size_ = 1;}


	/*to confirm the command byte*/
	switch(data_size_)
	{
		case 1: txid = CAN_Send_1_Byte; break;

		case 2: txid = CAN_Send_2_Byte; break;

		case 4: txid = CAN_Send_4_Byte; break;

		default: txid = CAN_Send_Default_Byte; break;
	}

	/*input the data to buffer*/
	cal_data.txid_ = SDO_sending_id;   //SDO
	cal_data.data_length_ = 8;

	cal_data.write_Data_Byte_[0] = txid;    					//command byte(4bytes fixed)
	cal_data.write_Data_Byte_[1] = (index_ & 0xff);				//Object Index (lsb)
	cal_data.write_Data_Byte_[2] = ((index_ >> 8) & 0xff);		//Object Index (msb)
	cal_data.write_Data_Byte_[3] = subindex_;					//sub Index

	/*make data a lsb char to CAN buffer*/
	for(int i = 4; i <= 7; i++) cal_data.write_Data_Byte_[i] = ((data_ >> (8 * (i - 4))) & 0xff);

	/*Register data from buffer*/
	QueueSaveRequest(cal_data);

	return;
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

	/*to check data byte size*/
	if(data_ > 0xffff || data_< 0) { data_size_ = 4;}
	else if(data_ > 0xff) {data_size_ = 2;}
	else { data_size_ = 1;}

	/*to confirm the command byte*/
	switch(data_size_)
	{
		case 1: txid = CAN_Send_1_Byte; break;

		case 2: txid = CAN_Send_2_Byte; break;

		case 4: txid = CAN_Send_4_Byte; break;

		default: txid = CAN_Send_Default_Byte; break;
	}

	/*input the data to buffer*/
	cal_data.txid_ = SDO_sending_id;
	cal_data.data_length_ = 8;

	cal_data.write_Data_Byte_[0] = txid;    					//command byte(4bytes fixed)
	cal_data.write_Data_Byte_[1] = (index_ & 0xff);				//Object Index (lsb)
	cal_data.write_Data_Byte_[2] = ((index_ >> 8) & 0xff);		//Object Index (msb)
	cal_data.write_Data_Byte_[3] = subindex_;					//sub Index

	/*make data a lsb char to CAN buffer*/
	for(int i = 4; i <= 7; i++) cal_data.write_Data_Byte_[i] = ((data_ >> (8 * (i - 4))) & 0xff);

	/*Register data from buffer*/
	AsyncQueueSaveRequest(cal_data);

	return;
}

void BG95::ReadDataEnqueue(int index, int subindex)
{
	/*Declare the buffers*/
	CAN_WData_HandleTypeDef cal_data;
	int index_ = index;

	/*input the data to buffer*/
	cal_data.txid_ = SDO_sending_id;
	cal_data.data_length_ = 8;

	cal_data.write_Data_Byte_[0] = CAN_Recv_Default_Byte;    					//command byte(4bytes fixed)
	cal_data.write_Data_Byte_[1] = (index_ & 0xff);				//Object Index (lsb)
	cal_data.write_Data_Byte_[2] = ((index_ >> 8) & 0xff);		// Object Index (msb)
	cal_data.write_Data_Byte_[3] = subindex;					//sub Index

	/*Register data from buffer*/
	QueueSaveRequest(cal_data);

	return;
}


//--------------------------------------------------------------queue

int BG95::SelectSendQueueType()
{
	int queue_type;

	if(!(IsAsyncRequestQueueEmpty())) {queue_type = async;}

	else if(!(IsRequestQueueEmpty())) {queue_type = sync;}

	this->send_queue_type_ = queue_type;

	return this->send_queue_type_;
}

CAN_WData_HandleTypeDef BG95::SelectSendQueueData(int type)
{
	int type_ = type;
	CAN_WData_HandleTypeDef WData_HandleType = {0,};

	switch(type_)
	{
		case async: WData_HandleType = AsyncRequestQueue.front(); break;

		case sync:  WData_HandleType = RequestQueue.front(); break;
	}

	return WData_HandleType;
}

void BG95::AsyncQueueSaveRequest(CAN_WData_HandleTypeDef cmd)
{
	if(this->is_run_) this->AsyncRequestQueue.push_back(cmd);

	return;
}

void BG95::QueueSaveRequest(CAN_WData_HandleTypeDef cmd){this->RequestQueue.push_back(cmd);}

void BG95::QueueDeleteRequest()
{
	int queue_type = this->send_queue_type_;

	switch(queue_type)
	{
		case async: this->AsyncRequestQueue.erase(AsyncRequestQueue.begin()); break;

		case sync: this->RequestQueue.erase(RequestQueue.begin()); break;
	}

	return ;
}

void BG95::QueueSaveReceive(CAN_RData_HandleTypeDef cmd){this->ReceiveQueue.push_back(cmd);}

void BG95::QueueDeleteReceive()
{
	if(!(ReceiveQueue.empty())) {this->ReceiveQueue.erase(ReceiveQueue.begin());}

	return;
}

void BG95::QueueChangeReceive()
{
	if(ReceiveQueue.size() >= 2)
	{
		//1. duplicate recv queue front data
		CAN_RData_HandleTypeDef temp_Data = ReceiveQueue.front();

		//2. delete front data
		QueueDeleteReceive();

		//3. push back to recv queue
		QueueSaveReceive(temp_Data);
	}

	return;
}


bool BG95::IsAsyncRequestQueueEmpty() {return AsyncRequestQueue.empty();}

bool BG95::IsRequestQueueEmpty() {return RequestQueue.empty();}

bool BG95::IsReceiveQueueEmpty() {return ReceiveQueue.empty();}




//--------------------------------------------------------------Check Timeout function
//Process Condition functions
bool BG95::IsSendTickReached()
{
	if(++(this->drive_tick_) >= 5 ) return false;

	this->drive_tick_ = 0;

	return true;
}


bool BG95::IsRecvTimedOut()
{
	if((HAL_GetTick() - this->comm_timestamp_) >= this->comm_timeout_)
	{
		/*timeout error*/
		this->comm_stat_reg_ |= COMM_RECV_TIMEOUT;

		return false;
	}

	this->comm_num_try_ = 0;

	return true;
}

//--------------------------------------------------------------------Check Data Analysis Drive
bool BG95::CheckReceivedNodeId()
{
	/*receive */
	int id = ReceiveQueue.front().rxid_ - this->nodeid_;

	if(id == abort_id)
	{
		this->comm_stat_reg_ |= CAN_ABORT_ERROR;

		return false;
	}

	/*
	switch(id)
	{
		//case NMT_id: break;     //ignored

		case abort_id: this->comm_stat_reg_ = CAN_ABORT_ERROR break;	//aborted -> CAN comm. error

		case SDO_reading_id: break; //SDO mode normal id

		case PDO_toHost: break;	//PDO mode normal id

		//case EMG_id: break; //ignored

		default:  break;
	}
  */

	return true;
}

bool BG95::CheckReceivedReadFunction()
{
	const CAN_RData_HandleTypeDef recv_data = ReceiveQueue.front();

	if(!( (recv_data.read_Data_Byte_[0] >= 0x40 && recv_data.read_Data_Byte_[0] <= 0x4f ) ||
			recv_data.read_Data_Byte_[0] == 0x60 ) || (recv_data.read_Data_Byte_[0] == 0x80))
	{
		return false;
	}

	return true;
}


bool BG95::CheckCommandData()
{
	//compare the data
	const CAN_WData_HandleTypeDef write_temp_data  = this->send_data_buffer;
	const CAN_RData_HandleTypeDef read_temp_data = ReceiveQueue.front();

	for (int count = 1; count <= 2 ; count++)
	{
		if(write_temp_data.write_Data_Byte_[count] !=
				read_temp_data.read_Data_Byte_[count]) return false;
	}

	return true;
}

bool BG95::CheckExceptionCase()
{
	//compare the data
	const CAN_RData_HandleTypeDef read_temp_data = ReceiveQueue.front();

	//case 1. an error occur
	if((read_temp_data.read_Data_Byte_[1] == 0x01) &
			(read_temp_data.read_Data_Byte_[2] == 0x30)) return false;

	//case 2. an abort error income
	if(this->comm_stat_reg_ == CAN_ABORT_ERROR) return false;

	return true;
}

bool BG95::DataAnalysis()
{
	//this is just for sequence
	while(1)
	{
		//1. Check Node id
		if(!(CheckReceivedNodeId())) break;

		//2 .Check read function code
		if(!(CheckReceivedReadFunction())) break;

		//3. check both command codes
		if(!(CheckCommandData()))
		{
			QueueChangeReceive();

			break;
		}

		//return no error
		return true;
	}

	//4. check Exception case - if case is included return no error
	if(!(CheckExceptionCase())) return true;

	return false;
}



void BG95::DataProcess(int index, int subindex, int data)
{
	switch(index)
	{
		case IO_AIN_Voltage_Up: this->motor_voltage_ = data ; break;
		case IO_AIN_Current_Im: this->motor_current_ = data ; break;

		case POS_ActualTargetPosition: this->target_pos_ = data ; break; //actual target position
		case POS_ActualPosition: this->motor_pos_ = data ; break;  //actual position
		//case 0x396a: this->motor_pos_ = data ; break; //encoder pos

		case MOTOR_Polarity: this->motor_dir_ = data ; break;//motor dir

		case MEASUREMENT_Vel_rpm:
			if(subindex == 1) this->motor_vel_ = data ;
			break;

		//setting parameters
		case VEL_DesiredValue: this->actual_max_vel_ = data; break;

		case VEL_Acc_dV: this->acc_rpm_ = data; break;
		case VEL_Acc_dT: this->acc_time_ = data; break;

		case VEL_Dec_dV: this->dec_rpm_ = data; break;
		case VEL_Dec_dT: this->dec_time_ = data; break;

		case VEL_Dec_QuickStop_dV: this->qdec_rpm_ = data; break;
		case VEL_Dec_QuickStop_dT: this->qdec_time_ = data; break;

		case DEV_ErrorReg: this->dev_err_data_ = (uint16_t)data ; break;
		case DEV_Status: this->dev_stat_reg_ = (uint16_t)data ; break;

		default: this->dev_err_data_ = (uint16_t)data;  break;
	}
}

void BG95::RecvDataProcess()
{
	CAN_RData_HandleTypeDef recv_data = ReceiveQueue.front();
	int index = 0;
	int subindex = 0;
	int data = 0;

	//4. revert index byte
	index |= recv_data.read_Data_Byte_[1];
	index |= (int)recv_data.read_Data_Byte_[2] << 8;

	//5.revert sub-index byte
	subindex = recv_data.read_Data_Byte_[3];

	//6.revert data byte
	for(int i = 4; i <= 7; i++) data  |=  (int)recv_data.read_Data_Byte_[i] << (8 * (i - 4));

	//7. Data Processing
	DataProcess(index, subindex, data);

	return;
}



//--------------------------------------------------------------------Error Checks


bool BG95::CommErrorCheck()
{
	int status = this->comm_stat_reg_;
	bool err_up = false;

	/*Check status and address the desired error*/
	switch(status)
	{
		/*if Communication is ok or recv timed out (for ingnore). exit function*/
		case COMM_OK: this->comm_status_filter = 0; break;

		case HAL_CAN_RECV_TIMEOUT: this->comm_status_filter = 0; break;

		/*Error up*/
		//case CAN_ABORT_ERROR: err_up = true; break;

		case COMM_RECV_TIMEOUT: if(++(this->comm_num_try_) >= 2) {err_up = true;} break;

		default: if(++(this->comm_status_filter) >= 5) {err_up = true;} break;
	}

	return err_up;
}


void BG95::ModuleErrorCheck()
{
	int error_data = 0;

	//0 . Device error check
	if(IsMotorErrUp() || this->dev_err_data_ != 0) error_data = 0x10000 + GetMotorErrData();

	//1. Comm. error check
	else if(CommErrorCheck()) error_data = 0x20000 + this->comm_stat_reg_;

	//2. address the error data
	if(error_data != 0)
	{
		this->module_error_data_ = error_data;

		/*Error flag up*/
		this->is_err_ = true;
	}

	return;
}

//--------------------------------------------------------------------Drive
bool BG95::DriveCheck()
{
	//0. if init is false -> dont drive
	if(!(IsInitTrue())) return false;

	//1. if Init. is true, and run is false -> wait for empty queue
	if(!(IsRunTrue()) & !(IsRequestQueueEmpty()) & !(IsErrTrue())) this->is_run_ = true;

	//2. if err up, return false
	if(this->module_error_data_ != COMM_OK) this->is_err_ = true;
	else this->is_err_ = false;

	//3. Stop all Write Command
	if(IsErrTrue())
	{
		this->is_run_ = false;

		//return false;

	}

	return true;
}



void BG95::InitProcess()
{
	//0. add read data queue and
	if(IsRequestQueueEmpty() & IsRequestQueueEmpty()) SchduleCommandEnqueue();

	//1. Initialize Communication buffer
	this->comm_stat_reg_ = COMM_OK;

	return;
}

void BG95::SendProcess()
{
	//0. the tick check for preventing 1-receiving by 1-drive
	if(!(IsSendTickReached())) return;

	//1. Check Queue is empty
	if(IsAsyncRequestQueueEmpty() & IsRequestQueueEmpty()) return;

	//2. Check ready for sending
	if(!(this->is_send_ready_)) return;

	//3. Get the high priority queue data. and If send data is none, return function
	if(SelectSendQueueType() == none) return;

	//4. Check CAN comm. sending failed, Only Accept status when HAL_OK
	if(SendRequest()!= HAL_OK) return;

	//5. Flag Set Off
	this->is_send_ready_ = false;

	//6. time-stamp recode
	this->comm_timestamp_ = HAL_GetTick();

	return;
}

void BG95::RecvProcess()
{
	//when the write command send
	if((!(this->is_send_ready_)))
	{
		//0. if Send is failed, Exit process
		if((this->comm_stat_reg_) != COMM_OK) return;

		//1. check timeed out
		if(!(IsRecvTimedOut())) return;

		//1-1. Check CAN comm. Receiving success or not
		if((RecvResponse() != COMM_OK))	return;

		//1-2. verify receiving data - exception case is included
		if(!(DataAnalysis())) return;

		this->is_send_ready_ = true;

	}
	//When the receiving data from devices without sending request
	else
	{
		//1. Check CAN comm. Receiving success or not
		if((RecvResponse() != COMM_OK))	return;
	}

	/*send,receive data is successfully matched*/
	//2. Received data process
	RecvDataProcess();

	QueueDeleteReceive();

	QueueDeleteRequest();


	return;
}

void BG95::PostProcess()
{
	//1. Check Motor Module Error
	ModuleErrorCheck();
/*
	//2.If get receive with no erros. delete receive data
	if((!(this->comm_stat_reg_== COMM_OK) || (this->comm_stat_reg_ == CAN_ABORT_ERROR))) return;

	QueueDeleteReceive();

	//3.If Send Request and get receive with no erros. delete request data
	if(this->is_send_ready_ == false)
	{
		QueueDeleteRequest();

		this->is_send_ready_ = true;
	}
*/
	return;
}


//--------------------------------------------------------------------Applications
//main functions

void BG95::Initialization()
{
	IntializeParameters();

	InitializeCommand();

	HAL_CAN_Initialization();

	this->is_init_ = true;

	this->is_send_ready_ = true;

	return;
}

void BG95::DeInitialization()
{
	IntializeParameters();

	HAL_CAN_DeInitialization();

	return;
}

void BG95::Drive()
{
	if(!(DriveCheck())) return;

	InitProcess();

	SendProcess();

	RecvProcess();

	PostProcess();

	return;
}

//Internal Status Check
const bool BG95::IsInitTrue() {return this->is_init_;}
const bool BG95::IsRunTrue() {return this->is_run_;}
const bool BG95::IsErrTrue() {return this->is_err_;}

const uint32_t BG95::GetModuleErrorData() {return this->module_error_data_;}


//dunkor device values
//Read value functions
const uint32_t BG95::GetMotorVoltage() {return this->motor_voltage_;}
const int32_t BG95::GetMotorCurrent() {return this->motor_current_;}
const int32_t BG95::GetMotorPosition() {return this->motor_pos_;}
const int32_t BG95::GetTargetPosition() {return this->target_pos_;}
const int32_t BG95::GetMotorVelocity() {return this->motor_vel_;}
const uint32_t BG95::GetMotorAccelation() {return this->acc_rpm_;}
const uint32_t BG95::GetMotorDeceleration() {return this->dec_rpm_;}

const uint32_t BG95::GetMotorStatus() {return this->dev_stat_reg_;}
const uint32_t BG95::GetMotorErrData() {return this->dev_err_data_;}

//read status functions
const bool BG95::IsMotorPowerUp() {return (this->dev_stat_reg_& 0x01);}
const bool BG95::IsMotorErrUp() {return (this->dev_stat_reg_& 0x02);}
const bool BG95::IsMotorMoving() {return (this->dev_stat_reg_& 0x08);}
const bool BG95::IsTargetPosReached() {return (this->dev_stat_reg_& 0x10);}
const bool BG95::IsMotorReachLimit() {return (this->dev_stat_reg_& 0x4000);}





