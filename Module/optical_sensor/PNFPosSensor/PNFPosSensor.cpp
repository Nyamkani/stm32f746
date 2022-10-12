/*
 * PNFPosSensor.cpp
 *
 *  Created on: Aug 17, 2022
 *      Author: studio3s
 */


#include "PNFPosSensor.h"
#include "transmit_tools/transmit_tools.h"


PNFPosSensor::PNFPosSensor(){};
PNFPosSensor::~PNFPosSensor(){};


//---------------------------------------------------------------send or read functions
uint16_t PNFPosSensor::TransmitSendRequest()
{
	unsigned char temp_data_ =  RequestCmd[RequestQueue.front()];
	unsigned char temp_data_rev = ~temp_data_;
	unsigned char address_data[3]= {temp_data_, temp_data_rev, 0x00};

	if(this->comm_dir_available_ == true) HAL_GPIO_WritePin(this->GPIO_, (uint16_t)this->dir_pin_no_, GPIO_PIN_SET);

	return HAL_UsartTransmit(this->huartx_, address_data, (sizeof(address_data)/sizeof(address_data[0])));
}


uint16_t PNFPosSensor::TransmitReceiveResponse()
{
	unsigned char tempdata[this->max_read_buf_size_] = {0,};

	if(this->comm_dir_available_ == true) HAL_GPIO_WritePin(this->GPIO_, (uint16_t)this->dir_pin_no_, GPIO_PIN_RESET);

	if(HAL_UsartReceive(this->huartx_, tempdata, this->max_read_buf_size_) == HAL_OK)
	{
		for(int i = 0; i<this->max_read_buf_size_; i++)
		{
			this->pos_buf_.emplace_back(tempdata[i]);
		}
		return HAL_OK;
	}
	else
	{
		return HAL_TIMEOUT;
	}
}


//---------------------------------------------------------------Command queue functions
//queue system functions
void PNFPosSensor::QueueSaveRequest(uint16_t cmd){this->RequestQueue.push(cmd);}
void PNFPosSensor::QueueDeleteRequest()
{
	if(this->now_filter_cnt_ == 0) this->RequestQueue.pop();
}


//---------------------------------------------------------------filter functions

bool PNFPosSensor::IsInfoFiltered()
{
	return (this->now_filter_cnt_>= this->max_filter_cnt_);
}

void PNFPosSensor::FilterCountUp()
{
	if(this->now_filter_cnt_< this->max_filter_cnt_) this->now_filter_cnt_++;
}

void PNFPosSensor::FilterCountReset()
{
	this->now_filter_cnt_ = 0;
}


//---------------------------------------------------------------public main functions
bool PNFPosSensor::IsErrUp()
{
	if(this->err_code_ >= 1) return true;
	else return false;
}


uint16_t PNFPosSensor::Drive()
{
	DriveInit();

	DriveComm();

	DriveAnalysis();

	return 0;
}
