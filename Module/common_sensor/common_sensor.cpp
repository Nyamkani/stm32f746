/*
 * CommonSensor.cpp
 *
 *  Created on: Jul 14, 2022
 *      Author: studio3s
 */


#include "common_sensor.h"


CommonSensor::CommonSensor(){}

CommonSensor::CommonSensor(uint16_t index, bool active_type, uint8_t max_filter_cnt,
		GPIO_TypeDef* GPIOx, uint32_t PinMask)
{
	this->index_ = index;
	this->active_type_ = active_type;
	this->max_filter_cnt_ = max_filter_cnt;
	this->GPIOx_ = GPIOx;
	this->PinMask_ = PinMask;
}

CommonSensor::~CommonSensor(){}


bool CommonSensor::GetSensorData() const {return this->output_;}

uint16_t CommonSensor::GetSensorIndex() const {return this->index_;}


//use hal or ll function
bool CommonSensor::CheckSensorValue()
{
	if(HAL_GPIO_ReadPin(this->GPIOx_, this->PinMask_) == 1) (this->state) = 1;
	else (this->state) = 0;

	if(!active_type_) this->state = !(this->state);
	return this->state;
}

bool CommonSensor::IsInfoFiltered()
{
	return (this->now_filter_cnt_>= this->max_filter_cnt_);
}

void CommonSensor::FilterCountUp()
{
	if(this->now_filter_cnt_< this->max_filter_cnt_) this->now_filter_cnt_++;
}

void CommonSensor::FilterStatusChanged(){this->now_filter_cnt_ = 0;}


bool CommonSensor::Drive()
{
	bool prev_val = this->state;
	bool now_val = CheckSensorValue();


	//1. Check Error status
	if(prev_val != now_val)
	{
		FilterStatusChanged();
		this->output_ = prev_val;
		return this->output_;
	}

	//2. wait until state is stabled(normal or error)
	if(!IsInfoFiltered())
	{
		FilterCountUp();
		this->output_ = prev_val;
		return this->output_;
	}

	this->output_ = now_val;
	return this->output_;
}

