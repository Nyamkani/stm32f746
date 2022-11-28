/*
 * IntegratedSensorManager.cpp
 *
 *  Created on: Jul 15, 2022
 *      Author: studio3s
 */



#include "sensor_manager.h"


//*WARNING* DO NOT CHANGE GLOBAL VARIABLE NAME
//If your stm32-uart-handlers are different, declare your extern huartx


extern UART_HandleTypeDef huart5;
extern UART_HandleTypeDef huart6;


bool SensorManager::bDestroyed_ = false;
SensorManager* SensorManager::pIns_ = NULL;


//--------------------------------------------------------------------------------------------------Single instance:
SensorManager::SensorManager() {};
SensorManager::SensorManager(const SensorManager& other){};
SensorManager& SensorManager::operator=(const SensorManager& ref) {return *this;};
SensorManager::~SensorManager()
{
	DeleteAllSensor();
	bDestroyed_ = true;
}

void SensorManager::ManagerCreate()
{
	static SensorManager ins;
	pIns_ = &ins;
}

void SensorManager::ManagerDelete()
{
	pIns_->~SensorManager();
}

SensorManager& SensorManager::GetInstance()
{
	if(bDestroyed_)
	{
		pIns_ = new SensorManager();
		// new(pIns) =  SensorManager;
		atexit(ManagerDelete);
		bDestroyed_ = false;
	}
	else if (pIns_ == NULL)
	{
		ManagerCreate();
	}
	 return *pIns_;
}


void SensorManager::DeleteAllSensor()
{
	for (auto& index : common_sensors_) delete (index);
	common_sensors_.clear();
	for (auto& index : pgv100_) delete (index);
	pgv100_.clear();
}


void SensorManager::AllSensorReset()
{
	CommonSensorReset();
	PGV100Reset();
}



//---------------------------------------------------------------------sensor function
//common sensor
void SensorManager::CommonSensorRegister()
{
	if(common_sensors_.empty())
	{
		//J6 -> PG9 ~ J11 -> PG15	, J12 -> PE0 , J16 -> PE4
		common_sensors_.emplace_back(new CommonSensor(AORG_L, ActiveH, Filter_5, GPIOG, GPIO_PIN_9));
		common_sensors_.emplace_back(new CommonSensor(AORG_R, ActiveH, Filter_5, GPIOG, GPIO_PIN_10));
		common_sensors_.emplace_back(new CommonSensor(WORG_L, ActiveH, Filter_5, GPIOG, GPIO_PIN_12));
		common_sensors_.emplace_back(new CommonSensor(WORG_R, ActiveH, Filter_5, GPIOG, GPIO_PIN_13));
		common_sensors_.emplace_back(new CommonSensor(TORG_L, ActiveH, Filter_5, GPIOG, GPIO_PIN_14));
		common_sensors_.emplace_back(new CommonSensor(TORG_C, ActiveH, Filter_5, GPIOG, GPIO_PIN_15));

		common_sensors_.emplace_back(new CommonSensor(TORG_R, ActiveH, Filter_5, GPIOE, GPIO_PIN_0));
		common_sensors_.emplace_back(new CommonSensor(LORG_U, ActiveH, Filter_5, GPIOE, GPIO_PIN_1));
		common_sensors_.emplace_back(new CommonSensor(LORG_C, ActiveH, Filter_5, GPIOE, GPIO_PIN_2));
		common_sensors_.emplace_back(new CommonSensor(LORG_L, ActiveH, Filter_5, GPIOE, GPIO_PIN_3));
		common_sensors_.emplace_back(new CommonSensor(IN_L, ActiveH, Filter_5, GPIOE, GPIO_PIN_4));
		//common_sensors_.emplace_back(new CommonSensor(IN_R, ActiveH, Filter_5, GPIOE, GPIO_PIN_4));
		//common_sensors_.resize(common_sensors_.size());
	}
}

void SensorManager::CommonSensorInitialize()
{
	CommonSensorRegister();
	CommonSensorDrive();
}



void SensorManager::CommonSensorReset()
{
	for (auto& index : common_sensors_) delete (index);
	common_sensors_.clear();
	CommonSensorRegister();
}



void SensorManager::CommonSensorDrive()
{
	for (auto& sensors : common_sensors_) sensors->Drive();
	CommonSensorGatherData();
}

uint16_t SensorManager::CommonSensorGetData(uint16_t index)
{
	uint16_t common_sensor_data = 0;
	for (auto& sensors : common_sensors_)
	{
		if((sensors->GetSensorIndex() == index))
		{
			common_sensor_data |=  (sensors->GetSensorData());
		}
	}
	return common_sensor_data;
}

uint16_t SensorManager::AORG_LGetData(){return CommonSensorGetData(AORG_L);}
uint16_t SensorManager::AORG_RGetData(){return CommonSensorGetData(AORG_R);}
uint16_t SensorManager::WORG_LGetData(){return CommonSensorGetData(WORG_L);}
uint16_t SensorManager::WORG_RGetData(){return CommonSensorGetData(WORG_R);}
uint16_t SensorManager::TORG_LGetData(){return CommonSensorGetData(TORG_L);}
uint16_t SensorManager::TORG_CGetData(){return CommonSensorGetData(TORG_C);}
uint16_t SensorManager::TORG_RGetData(){return CommonSensorGetData(TORG_R);}
uint16_t SensorManager::LORG_UGetData(){return CommonSensorGetData(LORG_U);}
uint16_t SensorManager::LORG_CGetData(){return CommonSensorGetData(LORG_C);}
uint16_t SensorManager::LORG_LGetData(){return CommonSensorGetData(LORG_L);}
uint16_t SensorManager::IN_LGetData(){return CommonSensorGetData(IN_L);}
uint16_t SensorManager::IN_RGetData(){return CommonSensorGetData(IN_R);}
uint16_t SensorManager::OUT_LGetData(){return CommonSensorGetData(OUT_L);}
uint16_t SensorManager::OUT_RGetData(){return CommonSensorGetData(OUT_R);}


void SensorManager::CommonSensorGatherData()
{
	uint16_t temp_sensor_data = 0;
	for (auto& sensors : common_sensors_)
	{
		temp_sensor_data |=  ((sensors->GetSensorIndex()) * (sensors->GetSensorData()));
	}
	this->common_sensor_data_ = temp_sensor_data;
}

uint16_t SensorManager::CommonSensorGetAllData() const {return this->common_sensor_data_;}


//---------------------------------------------------------------------PNFPOSsensor function
//PNFPos sensor

void SensorManager::OpticalSensorInitialize()
{
	PGV100Register();
	PCV80Register();
}

void SensorManager::PGV100Register()
{
	if(pgv100_.empty())
	{
		pgv100_.emplace_back(new PGV100(0x00, RS485, &huart5, GPIOB, GPIO_PIN_0, milimeter_1, 0.0, 0.0, 0.0));
	}
}

bool SensorManager::PGV100Initialize()
{
	PGV100Register();

	for (auto& index : pgv100_) index->Initialization();

	return PGV100IsErrUp();
}


void SensorManager::PGV100Reset()
{
	for (auto& index : pgv100_) delete (index);
	pgv100_.clear();
	PGV100Register();
}



void SensorManager::PGV100Drive()
{
	for (auto& index : pgv100_) index->Drive();
}


double SensorManager::PGV100GetXData() const
{
	return pgv100_.at(0)->GetXPos();
}

double SensorManager::PGV100GetYData() const
{
	return pgv100_.at(0)->GetYPos();
}

double SensorManager::PGV100GetAngleData() const
{
	return pgv100_.at(0)->GetAngle();
}

uint16_t SensorManager::PGV100GetTagData() const
{
	return pgv100_.at(0)->GetTagNo();
}

double SensorManager::PGV100GetErrData() const
{
	return pgv100_.at(0)->GetErrStatus();
}

bool SensorManager::PGV100IsErrUp() const
{
	return pgv100_.at(0)->IsErrUp();
}


void SensorManager::PGV100DirStraight()
{
	pgv100_.at(0)->RequestChangeDirstraight();
}

void SensorManager::PGV100DirLeft()
{
	pgv100_.at(0)->RequestChangeDirleft();
}

void SensorManager::PGV100DirRight()
{
	pgv100_.at(0)->RequestChangeDirright();
}





void SensorManager::PCV80Register()
{
	if(pcv80_.empty())
	{
		pcv80_.emplace_back(new PCV80(0x00, RS485, &huart6, GPIOB, GPIO_PIN_1, milimeter_1, 0.0, 0.0));
	}
}

bool SensorManager::PCV80Initialize()
{
	PCV80Register();

	for (auto& index : pcv80_) index->Initialization();

	PCV80Drive();

	return PCV80IsErrUp();
}

void SensorManager::PCV80Reset()
{
	for (auto& index : pcv80_) delete (index);
	pcv80_.clear();
	PCV80Register();
}



void SensorManager::PCV80Drive()
{
	for (auto& index : pcv80_) index->Drive();
}


double SensorManager::PCV80GetXData() const
{
	return pcv80_.at(0)->GetXPos();
}

double SensorManager::PCV80GetYData() const
{
	return pcv80_.at(0)->GetYPos();
}

double SensorManager::PCV80GetErrData() const
{
	return pcv80_.at(0)->GetErrStatus();
}

bool SensorManager::PCV80IsErrUp() const
{
	return pcv80_.at(0)->IsErrUp();
}



