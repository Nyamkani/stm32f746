/*
 * PCV80.cpp
 *
 *  Created on: Oct 12, 2022
 *      Author: studio3s
 */

#include <optical_sensor/pcv80/PCV80.h>

//--------------------------------------------------------------------------------------------------class PNFPosSensor:
//Consturctors
PCV80::PCV80()
{
	x_offset_ = 0.0;
	y_offset_ = 0.0;
	//Total_PNF_Sensor_Num++;
}

PCV80::PCV80(uint16_t index, uint16_t commtype, UART_HandleTypeDef *huartx,
				uint16_t Unit, double X_Offset, double Y_Offset)
{
	this->index_ = index;
	this->comm_type_ = commtype;
	this->x_offset_ = X_Offset;
	this->y_offset_ = Y_Offset;
	this->huartx_= huartx;
	this->unit_ = Unit;
	this->comm_dir_available_ = false;
}

PCV80::PCV80(uint16_t index,  uint16_t commtype, UART_HandleTypeDef *huartx,
				GPIO_TypeDef* GPIO, uint16_t dir_pin_no, uint16_t Unit, double X_Offset, double Y_Offset)
{
	this->index_ = index;
	this->comm_type_ = commtype;
	this->x_offset_ = X_Offset;
	this->y_offset_ = Y_Offset;
	this->huartx_= huartx;
	this->unit_ = Unit;
	this->GPIO_ = GPIO;
	this->dir_pin_no_ = dir_pin_no;
	this->comm_dir_available_ = true;
}

PCV80::~PCV80(){/*Total_PNF_Sensor_Num--;*/}

//----------------------------------------------------------------------------------------Functions
/*main frame*/
/*Construct level - network, first parmas. declation -> Construct_xxx*/
/*Initialize level - init buffers, check cmd -> Init_xxx*/
/*Work level - write and read -> Work_xxx*/
/*Parsing and Store level - parsing and check errors->Process_xxx*/
/*---------------------------------------------------*/
/*Queue - queue systems ->Queue*/

//--------------------------------------------------------------Reset all data
void PCV80::ResetAllData()
{
	this->xpos_ = 0;
	this->ypos_ = 0;
}




//--------------------------------------------------------------Construct level - network, first parmas. declation
void PCV80::RegisterRequsetCmd()
{
	RequestCmd.reserve(10);
	RequestCmd.emplace_back(0xA0);   //PCV80 Pos Requeset
}

void PCV80::RegisterDefaultParam()
{
	//Change_XOffset(1);
	//Change_YOffset(1);
	//Change_Angle_Offset(1);
	//RequestChangeDirstraight();
}


//Initialization for work-loop
void PCV80::ReadBufferInit()
{
	switch(RequestQueue.front())
	{
		//position
		case PCV80PosRequest: this->max_read_buf_size_ = PCV80Pos; break;
	}

	//resize vector length
	this->pos_buf_.clear();
}

//--------------------------------------------------------------Set parameters
//Common use
//void PNFPosSensor::Change_Buffer_size(uint16_t Buffer_Size) {this->Buffer_Size_ = Buffer_Size;}

//param use
PCV80& PCV80::SetXOffset(double X_Offset)
{
	this->x_offset_ = X_Offset;
	return* this;
}

PCV80& PCV80::SetYOffset(double Y_Offset)
{
	this->y_offset_ = Y_Offset;
	return* this;
}

PCV80& PCV80::SetUnit(double unit)
{
	this->unit_ = unit;
	return* this;
}


//---------------------------------------------------------------Request command
//pcv80 only
void PCV80::RequestGetPCV80Pos() {QueueSaveRequest(PCV80PosRequest);}

//---------------------------------------------------------------Command queue functions
//queue system functions
void PCV80::QueueRepeatPosReqeust()
{
	if(RequestQueue.empty()) RequestGetPCV80Pos();
}
//---------------------------------------------------------------return value functions

double PCV80::GetXPos() const {return xpos_;}
double PCV80::GetYPos() const {return ypos_;}
uint32_t PCV80::GetErrStatus() const {return err_code_;}

double PCV80::GetXOffset() const {return x_offset_;}
double PCV80::GetYOffset() const {return y_offset_;};
double PCV80::GetUnit() const {return unit_;}
uint16_t PCV80::GetCommtype() const {return comm_type_;}


uint32_t PCV80::ProcessGetERRInfo(std::vector<uint16_t> temp_buf)
{
	uint32_t err_data = (temp_buf)[4];
	(err_data)|=(temp_buf)[3] << 7;
	(err_data)|=(temp_buf)[2] << 14;
	(err_data)|=((temp_buf)[1]&0x07) << 21;
	return err_data;
}

//for getting data
bool PCV80::ProcessIsTagDetected(std::vector<uint16_t> temp_buf)
{
	if((temp_buf)[1] & 0x40) return true;
	else return false;
}

double PCV80::ProcessGetXPosInfo(std::vector<uint16_t> temp_buf)
{
	int32_t XPosition_DATA=(temp_buf)[4];
	(XPosition_DATA)|=(temp_buf)[3] << 7;
	(XPosition_DATA)|=(temp_buf)[2] << 14;
	(XPosition_DATA)|=((temp_buf)[1]&0x07) << 21;

	 //for making X-axis center to zero
	if(XPosition_DATA>=(100000)) XPosition_DATA = (XPosition_DATA-((double)(pow(2,24)-1))-((this->x_offset_)*(this->unit_)));
	else XPosition_DATA = (XPosition_DATA-((this->x_offset_)*(this->unit_)));


	double xpos = ((XPosition_DATA/(double)(this->unit_)));                   //To make units milimeters to meters

	if(xpos >= this->pos_area_min_ && xpos <= this->pos_area_max_) return xpos;
	else return this->xpos_;  //	  else { state |= 0x0010;} //Out of Range
}

double PCV80::ProcessGetYPosInfo(std::vector<uint16_t> temp_buf)
{
	int32_t YPosition_DATA = (temp_buf)[7];//Y Buf
	(YPosition_DATA) |= ((temp_buf)[6]) << 7;

	//for making Y-axis center to zero
	if(YPosition_DATA>=(1000))
	YPosition_DATA = (YPosition_DATA-(16383)) - ((this->y_offset_)*(this->unit_));
	else YPosition_DATA = (YPosition_DATA-(this->y_offset_*(this->unit_)));

	double ypos = ((YPosition_DATA/(double)(this->unit_)));

	return ypos;
}


//---------------------------------------------------------------Processing data
//for Error checking
uint16_t PCV80::ProcessChecksumData(std::vector<uint16_t> temp_buf)
{
	uint16_t temp = 0;
	uint16_t ChkSum_Data = 0;
	uint16_t even_cnt[8]={0,};

	//1. the last byte is for chksum data
	temp_buf.pop_back();

	//2. check each bytes for calculating 'xor' value
	for(uint16_t i=0; i<8; i++)
	{
		for (auto& index : temp_buf)
		{
			temp = index;
			if((temp>>i)&0x01) even_cnt[i]+=1;//8bit, even
			temp = 0;
		}
		if(even_cnt[i]!=0) ChkSum_Data |= (even_cnt[i]%2) * (1<<i);
	}
	 return ChkSum_Data;
}

uint16_t PCV80::ProcessCheckErr(std::vector<uint16_t> temp_buf)
{
	  //------------------------------------------------------------------
	 //STATE
	 // 0x0000 = Good
	 // 0x0001 = Read head tilted 180°.(pcv80 only)
	 // 0x0002 = code condition error(code distance chk)
	 // 0x0004 = No DIR. decision(Set POS.Sensor DIR.)
	 // 0x0008 = No Color decision(Set Color choice)
	 // 0x0010 = Out of Range
	 // 0x0020 = No Position
	 // 0x0040 = Timeout(communication error)
	 // 0x0080 = chk_sum error ;
	 // 0x1000 = internal error (Recommend to change sensors)
	 // 0x2000 = reserved
	 // 0x4000 = reserved
	 // 0x8000 = reserved
	 //--------------------------------------------------------------------
	 uint16_t state = 0x0000;

	//Check Comm. Error
	if(!comm_status_)
	{
		state = CommTimeout;
		return state;
	}

	 if((temp_buf.back()) == ProcessChecksumData(temp_buf))    //Checksum error pass or not(POS_BUF[20] <--- check sum buffer)
	 {
		  if((temp_buf)[0] & 0x01)    //Err Occured
		  {
			   uint16_t errcode =  ProcessGetERRInfo(temp_buf);
			   if(errcode >= 1000) state = InternalFatal;        //Internal Fatal Error
			   else if(errcode==1) state = ReadHeadTilted;     //read head tilted 180°.(pcv80 only)
			   else if(errcode==2) state = CodeConditionErr;     //code condition error(code distance chk)
			   else if(errcode==5) state = NoDirectionDeclared;     //No clear position can be determined(�Ÿ�����)
			   else if(errcode==6) state = NoColorDeclared;     // No Color decision(Set Color choice)
		  }
		  else if((temp_buf)[0]&0x02) state = NoPosition;    //No Position Error
	 }
	 else state = CheckSumErr;        //check sum error

	 return state;
}



//finally we got combined function
uint16_t PCV80::ProcessGetTotalInfo()
{
	//0 .Copying temp. data from received data
	std::vector<uint16_t> temp_buf_ = this->pos_buf_;

	//1. Timeout or other error handling
	uint16_t now_err = ProcessCheckErr(temp_buf_);

	//2. simplized error handle error >=1, good = 0 and if error occur, error count is up
	//3. when the Error count reached max count, return err code
	if(now_err >= 1)
	{
		FilterCountUp();
		if(IsInfoFiltered())
		{
			ResetAllData();
			FilterCountReset();
			this->err_code_ = now_err;
		}
		return 1;  //error occur
	}


	//4. Processing data
	switch(this->max_read_buf_size_)
	{
		case PCV80Pos:
			this->xpos_ = ProcessGetXPosInfo(temp_buf_); 	 				  //--- Get X POSITION
			this->ypos_ = ProcessGetYPosInfo(temp_buf_); 					  //--- Get Y POSITION
			break;
	}
	FilterCountReset();
	this->err_code_ = now_err;
	return this->err_code_;
}


void PCV80::DriveInit()
{
	QueueRepeatPosReqeust();
	ReadBufferInit();
}
void PCV80::DriveComm()
{
	if(TransmitSendRequest()!=HAL_OK || TransmitReceiveResponse()!=HAL_OK)
	{
		this->comm_status_ = false;
	}
	else
	{
		this->comm_status_ = true;
	}
}
void PCV80::DriveAnalysis()
{
	QueueDeleteRequest();
	ProcessGetTotalInfo();
}

//main functions
void PCV80::Initialization()
{
	RegisterRequsetCmd();
	RegisterDefaultParam();
}


