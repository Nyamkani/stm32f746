/*
 * IntegratedSensorManager.h
 *
 *  Created on: Jul 15, 2022
 *      Author: studio3s
 */

#ifndef MODULE_SENSOR_MANAGER_SENSOR_MANAGER_H_
#define MODULE_SENSOR_MANAGER_SENSOR_MANAGER_H_


#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include <vector>
#include <string>
#include <queue>

#include "common_sensor/common_sensor.h"
#include <optical_sensor/pgv100/pgv100.h>
#include <optical_sensor/pcv80/pcv80.h>

enum pickliftmask
{
	AORG_L = 0x0001,    	//ARM origin sensor left   0x0001
	AORG_R = 0x0002,  		//ARM origin sensor right  0x0002
	WORG_L = 0x0004,		//WIDTH origin sensor left  0x0004
	WORG_R = 0x0008,		//WIDTH origin sensor right  0x0008
	TORG_L = 0x0010,		//TURN origin sensor left  0x0010
	TORG_C = 0x0020,		//TURN origin sensor center  0x0020
	TORG_R = 0x0040,		//TURN origin sensor right 0x0040
	LORG_U = 0x0080,		//LIFT upper limit sensor  0x0080
	LORG_C = 0x0100,		//LIFT origin sensor  0x0100
	LORG_L = 0x0200,		//LIFT lower limit sensor  0x0200
	//Reserved = 0x0400,
	//Reserved = 0x0800,
	IN_L = 0x1000,		//in load check sensor left  0x1000
	IN_R = 0x2000,		//in load check sensor right  0x2000
	OUT_L = 0x4000,		//out load check sensor left  0x4000
	OUT_R = 0x8000,		//out load check sensor right  0x8000
};


class SensorManager
{
	private:
		static bool bDestroyed_;
		static SensorManager* pIns_;

		//sensor vectors
		std::vector<CommonSensor*> common_sensors_;
		std::vector<PGV100*> pgv100_;
		std::vector<PCV80*> pcv80_;
		//std::vector<PGV100*> pnf_pos_sensors_;

		//uint16_t commonsensorindex_;
		//uint16_t pnfsensorindex_;

		uint16_t common_sensor_data_;
		//uint16_t PNF_sensor_data_;


		SensorManager();
		SensorManager(const SensorManager& other);
		SensorManager& operator=(const SensorManager& ref);
		~SensorManager();

		static void ManagerCreate();
		static void ManagerDelete();
		void DeleteAllSensor();
		void AllSensorReset();

	public:
		static SensorManager& GetInstance();


	//Sensor itselfs
	private:
		uint16_t common_sensors_status_;

	//applications from SensorManager
	public:
		//---------------------------------------------------------------------sensor function
		//common sensor
		void CommonSensorRegister();
		void CommonSensorInitialize();

		void CommonSensorReset();
		void CommonSensorDrive();
		void CommonSensorGatherData();
		uint16_t CommonSensorGetData(uint16_t index);
		uint16_t AORG_LGetData();
		uint16_t AORG_RGetData();
		uint16_t WORG_LGetData();
		uint16_t WORG_RGetData();
		uint16_t TORG_LGetData();
		uint16_t TORG_CGetData();
		uint16_t TORG_RGetData();
		uint16_t LORG_UGetData();
		uint16_t LORG_CGetData();
		uint16_t LORG_LGetData();
		uint16_t IN_LGetData();
		uint16_t IN_RGetData();
		uint16_t OUT_LGetData();
		uint16_t OUT_RGetData();
		uint16_t CommonSensorGetAllData() const;


		//---------------------------------------------------------------------PNFPOSsensor function
		//PNFPos sensor
		void OpticalSensorInitialize(); //first init

		void PGV100Register();
		bool PGV100Initialize();

		void PGV100Reset();
		void PGV100Drive();

		double PGV100GetXData() const;
		double PGV100GetYData() const;
		double PGV100GetAngleData() const;
		uint16_t PGV100GetTagData() const;
		double PGV100GetErrData() const;
		bool PGV100IsErrUp() const;


		void PGV100DirStraight();
		void PGV100DirLeft();
		void PGV100DirRight();

		void PCV80Register();
		bool PCV80Initialize();
		void PCV80Reset();
		void PCV80Drive();
		double PCV80GetXData() const;
		double PCV80GetYData() const;
		double PCV80GetErrData() const;
		bool PCV80IsErrUp() const;


};








#endif /* MODULE_SENSOR_MANAGER_SENSOR_MANAGER_H_ */
