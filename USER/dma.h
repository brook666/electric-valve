#ifndef _DMA_H_
#define _DMA_H_
#include "stm8l15x.h"
# define USART1_DMA_BUFFER_SIZE 50
#define MAX_APP_MSG1 100
#define MAX_DATA_LEN   60
#define FRM_HEAD       0x68
#define FRM_END        0x16
typedef struct
{
  uint8_t precnt;   //DMAһ֡�е���ʼλ��
  uint8_t curcnt;   //DMAһ֡�еĽ���λ��
  volatile uint8_t  recvFlag;  
  volatile uint8_t  recvfrmcnt;  //���յ�֡������
  uint8_t  RecvData[50];
}DMA_RecvData_t;

typedef struct
{
  uint8_t  rxbuf[50];   //��������С
  uint8_t  rxbuf_vld;   //����
  uint16_t rxbuf_len;    //����������
  uint16_t rxbuf_payload_len; //����  
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

typedef struct
{
  uint8_t  frm_head[3];//֡ͷ
  uint8_t  type;//�Ǳ�����
  uint8_t  addr[7];//��ַ��
  uint8_t ctr_code;//������
  uint8_t data_len;//���ݳ���    
  uint8_t  msg[MAX_APP_MSG1];//������   
  uint8_t  cs; //У����
  uint8_t  frm_end;//֡β

} app_frm_t;
void bui_app_frm(uint8_t *pIn_msg, app_frm_t *pOut_pkg);
void uart1_frm_proc(uint8_t *str,uint16_t len);
void usart_dma_init(void) ;
void read_Usart1_DMA_FIFO(usart_buffer_t* usart1_rbuf);
app_usart_proc_t check_usart1_frame_head(uint8_t * pBegin, uint8_t * pEnd );
app_usart_proc_t check_usart1_frame_end_and_crc( uint8_t *beginIndex);
void Iterates_usart1_buffer(void);
uint8_t check_crc(uint8_t *frm, uint16_t len);
#endif