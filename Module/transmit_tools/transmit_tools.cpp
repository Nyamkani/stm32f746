/*
 * LLUart.cpp
 *
 *  Created on: Jul 6, 2022
 *      Author: studio3s
 */

#include "transmit_tools.h"

//these four-values must be in stm32f7xx_it.h or .c


//----------------------------------------------------------------------------------------UART(No interrupt)
int HAL_UsartTransmit(UART_HandleTypeDef* huartx, uint8_t* todata, int datalength)
{
	//return HAL_UART_Transmit_IT(huartx, todata, datalength);
    /* Process Locked */
    // __HAL_LOCK(huartx);

	return HAL_UART_Transmit(huartx, todata, datalength, CommTimeOut_1ms);
}

int HAL_UsartReceive(UART_HandleTypeDef* huartx, uint8_t* fromdata, int datalength)
{
	//return HAL_UART_Receive_IT(huartx, fromdata, datalength);
    /* Process Locked */
    // __HAL_LOCK(huartx);

	return HAL_UART_Receive(huartx, fromdata, datalength, CommTimeOut_1ms);
}


//----------------------------------------------------------------------------------------CANOpen(No interrupt)
int HAL_CANTransmit(CAN_HandleTypeDef* hcanx, const CAN_TxHeaderTypeDef *pHeader,
					const uint8_t *aData, uint32_t *pTxMailbox)
{
	int state = HAL_ERROR;
	//uint32_t now_ = HAL_GetTick();
	//uint16_t wait_time_ = CommTimeOut_1ms;

	state =  HAL_CAN_AddTxMessage(hcanx, pHeader, aData, pTxMailbox);

	/* Monitor the Tx mailboxes availability until at least one Tx mailbox is free*/
	while(HAL_CAN_GetTxMailboxesFreeLevel(hcanx) != 3);

	//waiting for message to leave
	while((HAL_CAN_IsTxMessagePending((hcanx) , *pTxMailbox)));

	//waiting for transmission request to be completed by checking RQCPx
	while( !(hcanx->Instance->TSR & ( 0x1 << (7 * ( (*pTxMailbox) - 1 )))));

	return state;
}

int HAL_CANReceive(CAN_HandleTypeDef *hcanx, CAN_RxHeaderTypeDef *pHeader, uint8_t *aData)
{
	int state = HAL_ERROR;
	uint32_t now_= HAL_GetTick();
	uint16_t wait_time_ = CommTimeOut_1ms;

	while(1)
	{
		/* Monitoring queue until at least one message is received */
		if(HAL_CAN_GetRxFifoFillLevel(hcanx, CAN_RX_FIFO0) >= 1)
		{
			state = HAL_CAN_GetRxMessage(hcanx, CAN_RX_FIFO0, pHeader, aData);
			break;
		}

		/*for TimeOut*/
		if(HAL_GetTick() - now_ >= wait_time_) state = HAL_TIMEOUT; break;
	}
	return state;
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *CanHandle)
{
	CAN_RxHeaderTypeDef RxHeader;
	uint8_t RxData[8] = {0,};

  /* Get RX message */
  if (HAL_CAN_GetRxMessage(CanHandle, CAN_RX_FIFO0, &RxHeader, RxData) != HAL_OK)
  {
    /* Reception Error */
    Error_Handler();
  }
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

	if(HAL_UsartReceive(this->huartx_, tempdata, total_buf_size_) == HAL_OK)
	{
		this->receive_data_.push(tempdata);
		return HAL_OK;
	}
	else {return HAL_TIMEOUT;}
}


int HALModbusRtu::HALModebusRtuSendData()
{
	return HAL_UsartTransmit(this->huartx_, this->send_data_.front(),
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




