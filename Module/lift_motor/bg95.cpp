/*
 * bg85.cpp
 *
 *  Created on: Sep 29, 2022
 *      Author: studio3s
 */

#include <lift_motor/bg95.h>

BG95::BG95(CAN_HandleTypeDef *hcanx, int Txid, int Rxid, int data_length)
{
	// TODO Auto-generated constructor stub
	this->hcanx_ = hcanx;
	this->Txid_ = Txid;
	this->Rxid_ = Rxid;
	this->data_length_ = data_length;

}

BG95::~BG95()
{
	// TODO Auto-generated destructor stub
	//if you using heap memory, delete all in this function
}

void BG95::CAN_DataEnque(uint8_t *pData) {QueueSaveRequest((char*)pData);}

//send or read function
uint16_t BG95::TransmitSendRequest()
{
	char temp_data_[data_length_];
	strcpy(temp_data_,RequestQueue.front());
	HALCANTransmit(hcanx_, &TxHeader_, (unsigned char*)temp_data_, &TxMailbox_);
	return 0;
}

uint16_t BG95::TransmitReceiveResponse()
{

	return 0;
}

//---------------------------------------------------------------Command queue functions
//queue system functions
void BG95::QueueSaveRequest(char* cmd){this->RequestQueue.push_back(cmd);}
void BG95::QueueDeleteRequest(){this->RequestQueue.erase(RequestQueue.begin());}


//--------------------------------------------------------------------initialization
void BG95::CAN_TxHandlerSetup()
{
	/* Configure Transmission process */
	this->TxHeader_.StdId = Txid_;                 // Standard Identifier, 0 ~ 0x7FF
	this->TxHeader_.ExtId = 0x01;                  // Extended Identifier, 0 ~ 0x1FFFFFFF
	this->TxHeader_.RTR = CAN_RTR_DATA;            // Frame Type, DATA or REMOTE
	this->TxHeader_.IDE = CAN_ID_STD;              // Identified, STD or EXT
	this->TxHeader_.DLC = data_length_;                       // Tx Frame length, 0 ~ 8 byte
	this->TxHeader_.TransmitGlobalTime = DISABLE; 			 // timestamping en/disable.
}

void BG95::CAN_RxHandlerSetup()
{
	/* Configure Transmission process */
	this->RxHeader_.StdId = Rxid_;                 // Standard Identifier, 0 ~ 0x7FF
	this->RxHeader_.ExtId = 0x01;                  // Extended Identifier, 0 ~ 0x1FFFFFFF
	this->RxHeader_.RTR = CAN_RTR_DATA;            // Frame Type, DATA or REMOTE
	this->RxHeader_.IDE = CAN_ID_STD;              // Identified, STD or EXT
	this->RxHeader_.DLC = data_length_;                       // Tx Frame length, 0 ~ 8 byte
}

void BG95::HAL_CAN_Initialization()
{
	if (HAL_CAN_Start(this->hcanx_) != HAL_OK) this->err_code_ = initfailed;
}

void BG95::HAL_CAN_DeInitialization()
{
	if (HAL_CAN_Stop(this->hcanx_) != HAL_OK){} //this->err_code_ = initfailed;
}



void BG95::DataBufferInit()
{
	memset(TxData_, '\0', sizeof(TxData_));
	memset(RxData_, '\0', sizeof(RxData_));
}



//--------------------------------------------------------------------Drive
void BG95::DriveInit()
{

	//ReadBufferInit();
	DataBufferInit();
}


void BG95::DriveComm()
{
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
	CAN_TxHandlerSetup();
	CAN_RxHandlerSetup();
	HAL_CAN_Initialization();

	if(TxData_ == nullptr) TxData_ = new uint8_t[data_length_];
	if(RxData_ == nullptr) RxData_ = new uint8_t[data_length_];

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
