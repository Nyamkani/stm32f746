/*
 * CommonSensor.h
 *
 *  Created on: Jul 14, 2022
 *      Author: studio3s
 */

#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include <vector>
#include <string>

#include "main.h"
#include "stm32f7xx_hal.h"
#include "stm32f746xx.h"


#ifndef INC_EXTINC_COMMONSENSOR_H_
#define INC_EXTINC_COMMONSENSOR_H_


enum ActiveHL
{
	ActiveL = 0,
	ActiveH = 1,
};

enum Output
{
	Detected = 0,
	Undetected = 1,
};

enum FilterMounts
{
	Filter_0 = 0,
	Filter_5 = 5,
	Filter_10 = 10,
};

class CommonSensor
{
	public:
		CommonSensor();
		CommonSensor(uint16_t index, bool active_type, uint8_t max_filter_cnt,
				GPIO_TypeDef* GPIOx, uint32_t PinMask);

		~CommonSensor();
	private:
		uint16_t index_;
		bool active_type_;
		bool output_;

		//pin configuration
		GPIO_TypeDef* GPIOx_ = NULL;
		uint32_t PinMask_ ;

		//filters
		uint8_t max_filter_cnt_ = 5;
		uint8_t now_filter_cnt_= 0;
		bool state = 0;

		//functions
		bool CheckSensorValue();


		bool IsInfoFiltered();
		void FilterCountUp();
		void FilterStatusChanged();

	public:
		bool Drive();
		bool GetSensorData() const;
		uint16_t GetSensorIndex() const;

};




#endif /* INC_EXTINC_COMMONSENSOR_H_ */
