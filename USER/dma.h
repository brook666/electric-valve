#ifndef _DMA_H_
#define _DMA_H_
#include "stm8l15x.h"
# define USART1_DMA_BUFFER_SIZE 50
#define FRM_HEAD       0x69
typedef struct
{
  uint8_t precnt;   //DMA一帧中的起始位置
  uint8_t curcnt;   //DMA一帧中的结束位置
  volatile uint8_t  recvFlag;  
  volatile uint8_t  recvfrmcnt;  //接收到帧的数量
  uint8_t  RecvData[50];
}DMA_RecvData_t;

typedef struct
{
  uint8_t  rxbuf[50];   //缓冲区大小
  uint8_t  rxbuf_vld;   //保留
  uint16_t rxbuf_len;    //缓冲区长度
  uint16_t rxbuf_payload_len; //保留  
}usart_buffer_t;
enum app_usart_proc{
  APP_USART_FRAME_NOT_COMPLETE,
  APP_USART_FRM_HEAD_SUCCESS,
  APP_USART_FRM_END_SUCCESS,
  APP_USART_FRM_HEAD_ERR,
  APP_USART_FRM_END_ERR,  
  APP_USART_FRM_ERR,
  APP_USART_CRC_ERR,
  APP_USART_LEN_ERR,
  APP_USART_BUFFER_FULL_ERR,
};
typedef enum app_usart_proc app_usart_proc_t;

void usart_dma_init(void) ;
void read_Usart1_DMA_FIFO(usart_buffer_t* usart1_rbuf);
app_usart_proc_t check_usart1_frame_head(uint8_t * pBegin, uint8_t * pEnd );
app_usart_proc_t check_usart1_frame_end_and_crc( uint8_t *beginIndex);
void Iterates_usart1_buffer(void);
#endif