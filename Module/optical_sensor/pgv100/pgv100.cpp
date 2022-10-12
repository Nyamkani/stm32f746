/*
 * PNFPosSensor.cpp
 *
 *  Created on: Jul 5, 2022
 *      Author: studio3s
 */

#include <optical_sensor/pgv100/pgv100.h>

/* Global Variables ------------------------------------------------------------------*/


/* Local Variables ------------------------------------------------------------------*/



//--------------------------------------------------------------------------------------------------class PNFPosSensor:
//Consturctors
PGV100::PGV100()
{
	x_offset_ = 0.0;
	y_offset_ = 0.0;
	angle_offset_ = 0.0;
	//Total_PNF_Sensor_Num++;
}

PGV100::PGV100(uint16_t index, uint16_t commtype, UART_HandleTypeDef *huartx,
				uint16_t Unit, double X_Offset, double Y_Offset, double Angle_Offset)
{
	this->index_ = index;
	this->comm_type_ = commtype;
	this->x_offset_ = X_Offset;
	this->y_offset_ = Y_Offset;
	this->huartx_= huartx;
	this->unit_ = Unit;
	this->angle_offset_ = Angle_Offset;
	this->comm_dir_available_ = false;
}

PGV100::PGV100(uint16_t index,  uint16_t commtype, UART_HandleTypeDef *huartx,
				GPIO_TypeDef* GPIO, uint16_t dir_pin_no, uint16_t Unit, double X_Offset, double Y_Offset, double Angle_Offset)
{
	this->index_ = index;
	this->comm_type_ = commtype;
	this->x_offset_ = X_Offset;
	this->y_offset_ = Y_Offset;
	this->huartx_= huartx;
	this->unit_ = Unit;
	this->angle_offset_ = Angle_Offset;
	this->GPIO_ = GPIO;
	this->dir_pin_no_ = dir_pin_no;
	this->comm_dir_available_ = true;
}

PGV100::~PGV100(){/*Total_PNF_Sensor_Num--;*/}

//----------------------------------------------------------------------------------------Functions
/*main frame*/
/*Construct level - network, first parmas. declation -> Construct_xxx*/
/*Initialize level - init buffers, check cmd -> Init_xxx*/
/*Work level - write and read -> Work_xxx*/
/*Parsing and Store level - parsing and check errors->Process_xxx*/
/*---------------------------------------------------*/
/*Queue - queue systems ->Queue*/

//--------------------------------------------------------------Reset all data
void PGV100::ResetAllData()
{
	this->xpos_ = 0;
	this->ypos_ = 0;
	this->angle_ = 0;
	this->tagNo_ = 0;
}



//--------------------------------------------------------------Construct level - network, first parmas. declation
void PGV100::RegisterRequsetCmd()
{
	RequestCmd.reserve(10);
	RequestCmd.emplace_back(0xEC);   //0
	RequestCmd.emplace_back(0xE8);   //1
	RequestCmd.emplace_back(0xE4);   //2
	RequestCmd.emplace_back(0x90);   //3
	RequestCmd.emplace_back(0x88);   //4
	RequestCmd.emplace_back(0xC4);   //5
	RequestCmd.emplace_back(0xC8);   //6
	RequestCmd.emplace_back(0xA0);   //7
}

void PGV100::RegisterDefaultParam()
{
	//Change_XOffset(1);
	//Change_YOffset(1);
	//Change_Angle_Offset(1);
	ResetAllData();
	RequestChangeDirstraight();
}


//Initialization for work-loop
void PGV100::ReadBufferInit()
{
	int BufferLength = RequestQueue.front();
	switch(BufferLength)
	{
		//position
		case PGV100PosRequest: this->max_read_buf_size_ = PGV100Pos; break;
		//Directions
		case PGV100StraightRequest: this->max_read_buf_size_ = PGV100Dir; break;
		case PGV100LeftRequest: this->max_read_buf_size_ = PGV100Dir; break;
		case PGV100RightRequest: this->max_read_buf_size_ = PGV100Dir; break;
		//Colors
		case PGV100RedRequest: this->max_read_buf_size_ = PGV100Color; break;
		case PGV100GreenRequest: this->max_read_buf_size_ = PGV100Color; break;
		case PGV100BlueRequest: this->max_read_buf_size_ = PGV100Color; break;
	}
	//size initialize
	this->pos_buf_.clear();
}

//--------------------------------------------------------------Set parameters
//Common use
//void PNFPosSensor::Change_Buffer_size(uint16_t Buffer_Size) {this->Buffer_Size_ = Buffer_Size;}

//param use
PGV100& PGV100::SetXOffset(double X_Offset)
{
	this->x_offset_ = X_Offset;
	return* this;
}

PGV100& PGV100::SetYOffset(double Y_Offset)
{
	this->y_offset_ = Y_Offset;
	return* this;
}

//pgv100 param only
PGV100& PGV100::SetAngleOffset(double Angle_Offset)
{
	this->angle_offset_ = Angle_Offset;
	return* this;
}

PGV100& PGV100::SetUnit(double unit)
{
	this->unit_ = unit;
	return* this;
}

//---------------------------------------------------------------Request command
//pgv100 only
void PGV100::RequestChangeDirstraight() {QueueSaveRequest(PGV100StraightRequest);}
void PGV100::RequestChangeDirleft() {QueueSaveRequest(PGV100LeftRequest);}
void PGV100::RequestChangeDirright() {QueueSaveRequest(PGV100RightRequest);}

void PGV100::RequestChangeColoryellow() {QueueSaveRequest(PGV100RedRequest);}
void PGV100::RequestChangeColorred() {QueueSaveRequest(PGV100GreenRequest);}
void PGV100::RequestChangeColorblue() {QueueSaveRequest(PGV100BlueRequest);}

void PGV100::RequestGetPGV100Pos() {QueueSaveRequest(PGV100PosRequest);}




//---------------------------------------------------------------Command queue functions
//queue system functions
void PGV100::QueueRepeatPosReqeust()
{
	if(RequestQueue.empty()) RequestGetPGV100Pos();
}
//---------------------------------------------------------------return value functions

double PGV100::GetXPos() const {return xpos_;}
double PGV100::GetYPos() const {return ypos_;}
double PGV100::GetAngle() const {return angle_;}
uint16_t PGV100::GetTagNo() const {return tagNo_;}
uint16_t PGV100::GetDir() const {return dir_;}
uint16_t PGV100::GetColor() const {return color_;}
uint32_t PGV100::GetErrStatus() const {return err_code_;}


double PGV100::GetXOffset() const {return x_offset_;}
double PGV100::GetYOffset() const {return y_offset_;};
double PGV100::GetAngleOffset() const {return angle_offset_;}
double PGV100::GetUnit() const {return unit_;}
uint16_t PGV100::GetCommtype() const {return comm_type_;}


uint32_t PGV100::ProcessGetERRInfo(std::vector<uint16_t> temp_buf)
{
	uint32_t err_data = ((temp_buf)[5]) & 0xFF;
	(err_data)|=((temp_buf)[4] << 7) & 0xFF;
	(err_data)|=((temp_buf)[3] << 14) & 0xFF;
	(err_data)|=(((temp_buf)[2]&0x07) << 21) & 0xFF;
	return err_data;
}

//for getting data
bool PGV100::ProcessIsTagDetected(std::vector<uint16_t> temp_buf)
{
	if((temp_buf)[1] & 0x40) return true;
	else return false;
}

uint16_t PGV100::ProcessGetTagNumber(std::vector<uint16_t> temp_buf)
{
	 uint16_t tag_num = 0;
	 (tag_num)=((temp_buf)[17]) & 0xFF;
	 (tag_num)|=((temp_buf)[16]<<7) & 0xFF;
	 (tag_num)|=((temp_buf)[15]<<14) & 0xFF;
	 (tag_num)|=((temp_buf)[14]<<21) & 0xFF;
	 return tag_num;
}

double PGV100::ProcessGetAngleInfo(std::vector<uint16_t> temp_buf)
{
	 uint16_t angle = ((temp_buf)[11]) & 0xFF;
	 (angle) |= ((temp_buf)[10] << 7) &0xFF;

	 double angle_d = (double)angle/10;
	 if((angle_d)> 180.0f) angle_d-=360.0f; //makes x-axis zero centered
	 return angle_d + this->angle_offset_;
}

double PGV100::ProcessGetXPosInfo(std::vector<uint16_t> temp_buf)
{
	int32_t XPosition_DATA=((temp_buf)[5]) & 0xFF;
	(XPosition_DATA)|=((temp_buf)[4] << 7) & 0xFF;
	(XPosition_DATA)|=((temp_buf)[4] << 14) & 0xFF;
	(XPosition_DATA)|=(((temp_buf)[2]&0x07) << 21) & 0xFF;

	 //for making X-axis center to zero
	if(XPosition_DATA>=(100000)) XPosition_DATA = (XPosition_DATA-((double)(pow(2,24)-1))-((this->x_offset_)*(this->unit_)));
	else XPosition_DATA = (XPosition_DATA-((this->x_offset_)*(this->unit_)));


	double xpos = ((XPosition_DATA/(double)(this->unit_)));                   //To make units milimeters to meters

	if(xpos >= this->pos_area_min_ && xpos <= this->pos_area_max_) return xpos;
	else return this->xpos_;  //	  else { state |= 0x0010;} //Out of Range
}

double PGV100::ProcessGetYPosInfo(std::vector<uint16_t> temp_buf)
{
	int32_t YPosition_DATA = ((temp_buf)[7]) & 0xFF;//Y Buf
	(YPosition_DATA) |= (((temp_buf)[6]) << 7) & 0xFF;

	//for making Y-axis center to zero
	if(YPosition_DATA>=(1000))
	YPosition_DATA = (YPosition_DATA-(16383)) - ((this->y_offset_)*(this->unit_));
	else YPosition_DATA = (YPosition_DATA-(this->y_offset_*(this->unit_)));

	double ypos = ((YPosition_DATA/(double)(this->unit_)));

	return ypos;
}

uint16_t PGV100::ProcessGetDirectionInfo(std::vector<uint16_t> temp_buf)
{
	return (uint16_t)(temp_buf)[1]&0x03;
}

uint16_t PGV100::ProcessGetColorInfo(std::vector<uint16_t> temp_buf)
{
	if((temp_buf)[0]&0x07 && (temp_buf)[1]&0x07) return (uint16_t)(temp_buf)[1]&0x07;
	else return this->color_;
}

//---------------------------------------------------------------Processing data
//for Error checking
uint16_t PGV100::ProcessChecksumData(std::vector<uint16_t> temp_buf)
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

uint16_t PGV100::ProcessCheckErr(std::vector<uint16_t> temp_buf)
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

	//If Comm Err is not, check others
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
uint16_t PGV100::ProcessGetTotalInfo()
{
	//0 .Copying temp. data from received data
	std::vector<uint16_t> temp_buf_ = this->pos_buf_;

	//1. check color response -> no error check
	if(max_read_buf_size_ == PGV100Color)  //response for changing colors
	{
		this->color_ = ProcessGetColorInfo(temp_buf_);
		return this->err_code_;
	}

	//2. Timeout or other error handling
	uint16_t now_err = ProcessCheckErr(temp_buf_);

	//3. simplized error handle error >=1, good = 0 and if error occur, error count is up
	//4. when the Error count reached max count, return err code
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

	//5. Processing data
	switch(this->max_read_buf_size_)
	{
		case PGV100Dir:
			this->dir_ = ProcessGetDirectionInfo(temp_buf_);
			break;
		case PGV100Pos:
			if(ProcessIsTagDetected(temp_buf_)) this->tagNo_ = ProcessGetTagNumber(temp_buf_);
			else this->tagNo_ = 0;
			this->dir_ = ProcessGetDirectionInfo(temp_buf_);
			this->angle_ = ProcessGetAngleInfo(temp_buf_); 	 			  //--- Get ANGLE INFO
			this->xpos_ = ProcessGetXPosInfo(temp_buf_); 	 				  //--- Get X POSITION
			this->ypos_ = ProcessGetYPosInfo(temp_buf_); 					  //--- Get Y POSITION
			break;
	}
	FilterCountReset();
	this->err_code_ = now_err;
	return this->err_code_;
}


void PGV100::DriveInit()
{
	QueueRepeatPosReqeust();
	ReadBufferInit();
}

void PGV100::DriveComm()
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

void PGV100::DriveAnalysis()
{
	ProcessGetTotalInfo();
	QueueDeleteRequest();
}

//main functions
void PGV100::Initialization()
{
	RegisterRequsetCmd();
	RegisterDefaultParam();
}



