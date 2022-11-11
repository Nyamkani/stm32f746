/*
 * PCV80.h
 *
 *  Created on: Oct 12, 2022
 *      Author: studio3s
 */

#ifndef MODULE_OPTICAL_SENSOR_PCV80_H_
#define MODULE_OPTICAL_SENSOR_PCV80_H_


#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include <vector>
#include <string>
#include <queue>
#include <optical_sensor/PNFPosSensor/PNFPosSensor.h>


enum PCV80_cmd
{
	//--------------------------------------------------PGV100 Commands
	//Write Comm. cmd
	PCV80PosRequest = 0,                         //for Reqeusting messages    from head to receive POSITON
};

class PCV80 : public PNFPosSensor
{
	public:
		PCV80();

		PCV80(uint16_t index, uint16_t commtype, UART_HandleTypeDef *huartx,
			   uint16_t Unit, double X_Offset, double Y_Offset);

		PCV80(uint16_t index, uint16_t commtype, UART_HandleTypeDef *huartx,
			   GPIO_TypeDef* GPIO, uint16_t dir_pin_no, uint16_t Unit, double X_Offset, double Y_Offset);

		virtual ~PCV80();


	private:
		//Limitation
		int32_t pos_area_max_ = 1000000;      // Max. range of tape value                                       // PCV센서 범위 Maximum(mm)
		int32_t pos_area_min_ = (-100);   // Min. range of tape value

		//--------------------------------------------------------------------------- Inherited Module
		//Inherited
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
		uint32_t GetErrStatus() const;

		double GetXOffset() const;
		double GetYOffset() const;
		double GetUnit() const;
		uint16_t GetCommtype() const;

		//-----------------------------------------------------------------------Change parameters

		//void ChangeBuffersize(uint16_t Buffer_Size);
		PCV80& SetXOffset(double X_Offset);
		PCV80& SetYOffset(double Y_Offset);
		PCV80& SetAngleOffset(double Angle_Offset);
		PCV80& SetUnit(double unit);

		//----------------------------------------------------------------------Requset command
		void RequestGetPCV80Pos();


		//overriding functions
		//virtual for each sensors
	private:
		void DriveInit() override;
		void DriveComm() override;
		void DriveAnalysis() override;

	public:
		//main functions
		void Initialization() override;
};




#endif /* MODULE_OPTICAL_SENSOR_PCV80_H_ */
