/*
 * PNFPosSensor.h
 *
 *  Created on: Jul 5, 2022
 *      Author: studio3s
 */

#ifndef MODULE_OPTICAL_SENSOR_PGV100_H_
#define MODULE_OPTICAL_SENSOR_PGV100_H_

#pragma once
//===========================================================================
//	Include Files
//===========================================================================

#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include <vector>
#include <string>
#include <queue>
#include <optical_sensor/PNFPosSensor/PNFPosSensor.h>


enum communication
{
	RS485 = 0,
	USB = 1,
	EtherNET = 2,
};

enum PGV100_dir
{
	Straight = 0,
	Left = 1,
	Right = 2,
};

enum PGV100_color
{
	Red = 0,
	Green = 1,
	Blue = 2,
};

enum buffer_length
{
	PGV100Color = 2,
	PGV100Dir = 3,
	PGV100Pos = 21,
};



enum PGV100_cmd
{
	//--------------------------------------------------PGV100 Commands
	//Write Comm. cmd
	PGV100StraightRequest = 0,                //for Reqeusting  changing  straight  direction
	PGV100LeftRequest = 1,                        //for Reqeusting  changing  left  direction
	PGV100RightRequest= 2,                       //for Reqeusting  changing  right direction

	PGV100RedRequest = 3,                         //for Reqeusting  changing  RED direction
	PGV100GreenRequest = 4,                       //for Reqeusting  changing  GREEN direction
	PGV100BlueRequest = 5,                        //for Reqeusting  changing  BLUE direction

	PGV100PosRequest = 6,                         //for Reqeusting messages    from head to receive POSITON
};

enum PGV100_unit
{
	milimeter_1 = 10,
	meter_1 = 10000,
};


class PGV100 : public PNFPosSensor
{
  public:
	   //Basic constructor
	PGV100();

	PGV100(uint16_t index, uint16_t commtype, UART_HandleTypeDef *huartx,
		   uint16_t Unit, double X_Offset, double Y_Offset, double Angle_Offset);

	PGV100(uint16_t index, uint16_t commtype, UART_HandleTypeDef *huartx,
		   GPIO_TypeDef* GPIO, uint16_t dir_pin_no, uint16_t Unit, double X_Offset, double Y_Offset, double Angle_Offset);

	~PGV100();

	private:
		//Exclusive data from pgv100
		double angle_ = 0;
		uint16_t tagNo_ = 0;
		uint16_t dir_ = 0;
		uint16_t color_ = 0;

		double angle_offset_;

		bool init_done_= false;

		//Limitation
		int32_t pos_area_max_ = 20000;      // Max. range of tape value                                       // PCV센서 범위 Maximum(mm)
		int32_t pos_area_min_ = (-100);   // Min. range of tape value

		//--------------------------------------------------------------------------- Inherited Module
		void ResetAllData();

		///First Time setup
		void RegisterRequsetCmd();
		void RegisterDefaultParam();

		//Initialization for work-loop
		void ReadBufferInit();

		//Queue repeat
		void QueueRepeatPosReqeust();

	private:
		//---------------------------------------------------------------Processing data
		double ProcessGetXPosInfo(std::vector<uint16_t> temp_buf);
		double ProcessGetYPosInfo(std::vector<uint16_t> temp_buf);
		uint32_t ProcessGetERRInfo(std::vector<uint16_t> temp_buf);


		bool ProcessIsTagDetected(std::vector<uint16_t> temp_buf);
		uint16_t ProcessGetTagNumber(std::vector<uint16_t> temp_buf);
		double ProcessGetAngleInfo(std::vector<uint16_t> temp_buf);

		uint16_t ProcessGetDirectionInfo(std::vector<uint16_t> temp_buf);
		uint16_t ProcessGetColorInfo(std::vector<uint16_t> temp_buf);

		uint16_t ProcessChecksumData(std::vector<uint16_t> temp_buf);
		uint16_t ProcessCheckErr(std::vector<uint16_t> temp_buf);

		//Parsing and Get error
		uint16_t ProcessGetTotalInfo();

	public:
		//---------------------------------------------------------------return value functions
		double GetXPos() const;
		double GetYPos() const;
		double GetAngle() const;
		uint16_t GetTagNo() const;
		uint16_t GetDir() const;
		uint16_t GetColor() const;
		uint32_t GetErrStatus() const;


		double GetXOffset() const;
		double GetYOffset() const;
		double GetAngleOffset() const;
		double GetUnit() const;
		uint16_t GetCommtype() const;

		//-----------------------------------------------------------------------Change parameters

		//void ChangeBuffersize(uint16_t Buffer_Size);
		PGV100& SetXOffset(double X_Offset);
		PGV100& SetYOffset(double Y_Offset);
		PGV100& SetAngleOffset(double Angle_Offset);
		PGV100& SetUnit(double unit);
		PGV100& SetErr(bool err_status);

		//----------------------------------------------------------------------Requset command
		void RequestChangeDirstraight();
		void RequestChangeDirleft();
		void RequestChangeDirright();

		void RequestChangeColoryellow();
		void RequestChangeColorred();
		void RequestChangeColorblue();

		void RequestGetPGV100Pos();


		//overriding functions
		//virtual for each sensors
	private:
		void DriveInit() override;
		bool DriveComm() override;
		void DriveAnalysis() override;

	public:
		//main functions
		void Initialization() override;
};








#endif /* MODULE_OPTICAL_SENSOR_PGV100_H_ */
