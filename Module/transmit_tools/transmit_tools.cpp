/*
 * LLUart.cpp
 *
 *  Created on: Jul 6, 2022
 *      Author: studio3s
 */

#include "transmit_tools.h"

//these four-values must be in stm32f7xx_it.h or .c


//----------------------------------------------------------------------------------------UART
int HALUsartTransmit(UART_HandleTypeDef* huartx, uint8_t* todata, int datalength)
{
	//return HAL_UART_Transmit_IT(huartx, todata, datalength);
	return HAL_UART_Transmit(huartx, todata, datalength, 10);
}

int HALUsartReceive(UART_HandleTypeDef* huartx, uint8_t* fromdata, uint16_t datalength)
{
	//return HAL_UART_Receive_IT(huartx, fromdata, datalength);
	return HAL_UART_Receive(huartx, fromdata, datalength, 10);
}



//----------------------------------------------------------------------------------------CANOpen
int HALCANTransmit(CAN_HandleTypeDef* hcanx, const CAN_TxHeaderTypeDef *pHeader,
					const uint8_t *aData, uint32_t *pTxMailbox)
{
	return HAL_CAN_AddTxMessage(hcanx, pHeader, aData, pTxMailbox);
}

int HALCANReceive(CAN_HandleTypeDef *hcan, CAN_RxHeaderTypeDef *pHeader, uint8_t *aData)
{
	return HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, pHeader, aData);
}



//----------------------------------------------------------------------------------------MODBUS RTU



HALModbusRtu::HALModbusRtu(UART_HandleTypeDef* huartx)
{
	this->huartx_= huartx;
}
HALModbusRtu::~HALModbusRtu(){}


void HALModbusRtu::HALModbusRtuSingleRequest(int ids, int da, char data)
{
	int arr_size_ = 0;
	uint8_t buf_data_[255] = {0,};   //size ???
	uint8_t data_array_[arr_size_] = {0,};
	int crc_val_=0;

    //0. Select slave id's
	buf_data_[arr_size_++] = ids;

	//1. Select 'function call', Only Read = 0x04, Read/Write Register = 0x03,0x06,0x10 =16
	buf_data_[arr_size_++] = 6;

	//2. Select Data address which is requested
	buf_data_[arr_size_++] = (uint8_t)(da >> 8 ); 					//HIGH
	buf_data_[arr_size_++] = (uint8_t) da;							//LOW

	//3. Determine the data size.
	buf_data_[arr_size_++] = 2; 				     				//2bytes for single

	//4. Input data to send
	buf_data_[arr_size_++] = (uint8_t)(data >> 8 ); 		//HIGH
	buf_data_[arr_size_++] = (uint8_t)data;					//LOW


	//5. CRC for Error Checking. but, It is litte-endian
	crc_val_ = CalcCRC((unsigned char*)buf_data_, (unsigned short)(arr_size_));
	buf_data_[arr_size_++] = (uint8_t) crc_val_;					//LOW
	buf_data_[arr_size_++] = (uint8_t)(crc_val_  >> 8 ); 			//HIGH

	//6. Enqueue!
	for(int i = 0; i<arr_size_; i++) data_array_[i] = buf_data_[i];

	send_data_.push(data_array_);

	//HALUsartTransmit(this->huartx_, data_array_, (sizeof(data_array_)/sizeof(data_array_[0])));
}

void HALModbusRtu::HALModbusRtuMultiRequest(int ids, int da, char* data, int datasize)
{
	int arr_size_ = 0;
	uint8_t buf_data_[255] = {0,};   //size ???
	uint8_t data_array_[arr_size_] = {0,};
	int crc_val_=0;
	int num_of_registers = datasize;

    //0. Select slave id's
	buf_data_[arr_size_++] = ids;

	//1. Select 'function call', Only Read = 0x04, Read/Write Register = 0x03,0x06,0x10 =16
	buf_data_[arr_size_++] = 16;

	//2. Select Data address which is requested
	buf_data_[arr_size_++] = (uint8_t)(da  >> 8 ); 				//HIGH
	buf_data_[arr_size_++] = (uint8_t) da;							//LOW

	//3. Determine the data size.
	buf_data_[arr_size_++] = 2 * num_of_registers; 				//2bytes each

	//4. Input data to send
	for(int i = 0; i<=num_of_registers; i++)
	{
		buf_data_[arr_size_++] = (uint8_t)(data[i]  >> 8 ); 		//HIGH
		buf_data_[arr_size_++] = (uint8_t)data[i];					//LOW
	}

	//5. CRC for Error Checking. but, It is litte-endian
	crc_val_ = CalcCRC((unsigned char*)buf_data_, (unsigned short)(arr_size_));
	buf_data_[arr_size_++] = (uint8_t) crc_val_;					//LOW
	buf_data_[arr_size_++] = (uint8_t)(crc_val_  >> 8 ); 			//HIGH

	//6. Enqueue!
	for(int i = 0; i<arr_size_; i++) data_array_[i] = buf_data_[i];

	send_data_.push(data_array_);

	//HALUsartTransmit(this->huartx_, data_array_, (sizeof(data_array_)/sizeof(data_array_[0])));
}

int HALModbusRtu::HALModbusRtuReceive()
{
	//0. Calculate a mount of data
	int total_buf_size_ = 0;
	int num_of_registers = this->data_size_*2;
	total_buf_size_ = 1 + 1 + 1 + num_of_registers + 2; //ids + fc + number of data + data + crc
	uint8_t tempdata[total_buf_size_] = {0,};

	if(HALUsartReceive(this->huartx_, tempdata, total_buf_size_) == HAL_OK)
	{
		this->receive_data_.push(tempdata);
		return HAL_OK;
	}
	else {return HAL_TIMEOUT;}
}


int HALModbusRtu::HALModebusRtuSendData()
{
	return HALUsartTransmit(this->huartx_, this->send_data_.front(),
			(sizeof(&send_data_.front())/sizeof(send_data_.front()[0])));
}





/*
void LLUsartTransmit(USART_TypeDef *USARTx, std::string data)
{
	uint16_t datalength = data.size();
	for(uint16_t i=0;i<datalength;i++){
		LL_USART_TransmitData8(USARTx, (char) data[i]);
		while(!LL_USART_IsActiveFlag_TXE(USARTx));
	}
}
*/
/*

void LLUsartTransmit(USART_TypeDef *USARTx, std::string data)
{
	uint16_t datalength = data.size();
	for(uint16_t i=0;i<datalength;i++){
		//LL_USART_TransmitData8(&USARTx, data[i]);
		//while(!LL_USART_IsActiveFlag_TXE(&USARTx));
		LL_USART_TransmitData8(USARTx, data[i]);
		while(!LL_USART_IsActiveFlag_TXE(USARTx));
	}
}

*/
/*
void LLUsartReceive(USART_TypeDef *USARTx)
{
	LL_USART_EnableIT_RXNE(USARTx);
	LL_USART_EnableIT_IDLE(USARTx);
}
*/




