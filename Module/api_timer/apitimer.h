/*
 * apitimer.h
 *
 *  Created on: Oct 17, 2022
 *      Author: studio3s
 */

#ifndef MODULE_API_TIMER_APITIMER_H_
#define MODULE_API_TIMER_APITIMER_H_

class api_timer
{
	public:
		api_timer();
		virtual ~api_timer();


	private:
		long long int time_ = 0;



		//--------------------------------public functions
		void UpdateTime();

};

#endif /* MODULE_API_TIMER_APITIMER_H_ */
