/******************************************************************************\
**  ��    Ȩ :  �����кͶ�̩���ܿ��ƹɷ����޹�˾���У�2020��
**  �ļ����� :  HET_ClifeProtocol.h
**  �������� :  CPͨѶ�����������������ͨѶ���Լ���������
**  ��    �� :  vincent
**  ��    �� :  2020.07.01
**  ��    �� :  V0.0.1
**  �����¼ :  V0.0.1/2020.07.01
                1 �״δ���                 
\******************************************************************************/

#ifndef _HET_CLIFEPROTOCOL_H_
#define _HET_CLIFEPROTOCOL_H_

/******************************************************************************\
                             Includes
\******************************************************************************/	
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
/******************************************************************************\
                          Macro definitions
\******************************************************************************/
#define CP_PACK_HEAD                0xA5                    //Э���ͷ

#define CP_RECV_LEN				    250                     //���ջ������鳤��
#define CP_SEND_LEN				    512                     //���ͻ������鳤��
#define CP_SEND_TEMP_LEN			512                     //���ͻ������鳤��


#define BIT_CP_BIND				    0x10					//WiFi״̬�ְ󶨱�־λ
#define BIT_CP_ROUTER				0x20                    //WiFi״̬��·�������ӱ�־λ
#define BIT_CP_SERVICE				0x40					//WiFi״̬�ַ��������ӱ�־λ
#define BIT_CP_ALL					0x70					//WiFi״̬��ȫ�����ӱ�־λ

#define CP_TEST_TIMEOUT			    3                       //��������ͳ�ʱ����
#define CP_TEST_MAX_TIMES			60						//���ⳬʱʱ��(1����)
#define COMMAND_ERROR_RESET			6						//ͨѶ�쳣����

#define HET_CP_DELAY_MS(x)			(x)                     //�����ӳ�
#define HET_CP_DELAY_S(x)			((uint32_t)100*(x))     //���ӳ�

#define	HET_CP_SEND_IDLE			0                       //���Ϳ���
#define	HET_CP_SEND_CYCLE			10                     //���ͼ��,���ʱ��Ϊ��HET_CP_SEND_CYCLE*10ms

//�豸��Ϣ
#define DEVICE_INFO_LEN             32                          //�豸��Ϣ�ֽڳ���
#define SOFT_VERSION                0x01                        //����汾
#define HARD_VERSION                0x01                        //������Ӳ���汾
#define DEVICE_MODEL                "NULL"                      //�����ͺ�
#define DEVICE_RESERVED             "NULL"                      //Ԥ��

#define CP_MASTER_MODE                 0 //����ͨ��ָ����MCU
#define CP_SLAVE_MODE                  1 //�ӻ�ͨ��ָWiFiģ�顢GUI���
#define CP_MODE                     CP_SLAVE_MODE//MASTER_MODE//

#ifndef HET_PLUGIN_CP_NUM
#define HET_PLUGIN_CP_NUM 1
#endif 

#define DBG_LOG_EN 1

#if DBG_LOG_EN
#define DEBUG(format, ...) printf (format, ##__VA_ARGS__)
#else
#define DEBUG
#endif


/******************************************************************************\
                         Typedef definitions
\******************************************************************************/
#pragma pack(1)
//------------------------------------------------------------------------------

//CMD����
typedef enum
{
    CMD_HEARTBEAT  = 0x01,          	//����
    CMD_DEVICEINFO = 0x02,          	//��ѯ��Ʒ��Ϣ
    CMD_GETDATE    = 0x03,          	//��ȡʱ����Ϣ
    CMD_BINDING    = 0x04,          	//��
    CMD_TEST       = 0x05,          	//����
    CMD_TESTRESULT = 0x06,             //����������
    CMD_DOWNLOAD   = 0x07,             //�����·�
    CMD_UPLOAD     = 0x08,             //�����ϱ�
    CMD_DATASYNC   = 0x09,             //����ͬ��
    CMD_RESET      = 0x0A,             //��λ
    CMD_UNBIND     = 0x0B,             //�����

}TE_HET_CP_CMD;

//CP����״̬
typedef enum
{
    HET_ONLINE,                                     //����
    HET_OFFLINE,                                    //����
}TE_HET_NET_STATUS;

//������
typedef enum
{
    FACTORY_IDLE,                                   //δ����
    FACTORY_TESTING,                                //������
    FACTORY_FAIL,                                   //����ʧ��
    FACTORY_PASS,                                   //����ɹ�
}TE_HET_FACTORYTEST_RESULT;

//�������ݷ���״̬
typedef enum
{
    HET_SEND_IDLE,                                  //����״̬
    HET_SEND_BUSY,                                  //����������
}TE_HET_SENDSTATUS;
//------------------------------------------------------------------------------
typedef struct
{
    uint8_t 				    Id;				//����ID
    uint8_t 				    Len;			//���ݳ���
    uint8_t* 				    pData;           //����ָ��
}TS_HET_DATA_UNIT;

//------------------------------------------------------------------------------

//�豸��Ϣ
typedef struct
{
    uint8_t 				    SoftwareVer;				//����汾��Ϣ
    uint8_t 				    HardwareVer;				//Ӳ���汾��Ϣ
    uint8_t 				    DeviceProductModel[15];     //��Ʒ�ͺ�
    uint8_t 				    Reserve2[15];               //������
}TS_HET_CP_DEVICEINFO;

//ϵͳʱ��
typedef struct
{
    uint8_t 				    Year;				//��
    uint8_t 				    Month;				//��
    uint8_t 				    Date;               //��
    uint8_t 				    Hours;              //ʱ
    uint8_t 				    Minutes;            //��
    uint8_t 				    Seconds;            //��
    uint8_t 				    Weekday;            //����
}TS_HET_CP_TIME;

//------------------------------------------------------------------------------
//CP���ڽ���״̬
typedef enum
{
    FLOW_UART_RECV_BEGIN,                                   //���տ�ʼ
    FLOW_UART_RECV_FINISH,                                  //�������
}TE_CP_RECV_FLOW;
    
//�������ݽṹ��
typedef struct
{
    uint8_t					    ReceiveBuf[CP_RECV_LEN];	//MCU����WIFI���ݵĻ���	
    uint8_t					    ReceiveF;                   //���ձ�־λ
    uint16_t             	    ReceiveLen;                 //����WIFI���ݵĳ���
    TE_CP_RECV_FLOW		        ReceiveFlow;                //����WIFI���ݵ�����ö��
    
}TS_CPCOMM_RECV;

//�������ݽṹ��
typedef struct
{
    uint8_t					    SendTempBuff[CP_SEND_TEMP_LEN];//������ʱ����
    uint8_t					    SendBuff[CP_SEND_LEN];      //���ͻ���
    uint16_t                    UpdateIndex;                //���͸���������������ɺ�����
    uint16_t                    SendDelay;                  //���ͼ��
}TS_CPCOMM_SEND;
//------------------------------------------------------------------------------
//����ָ��ṹ��
typedef struct
{
    void (*UartSendCallback) (uint8_t *pBuf, uint16_t Len);           // ���ڷ��ͻص�

    #if CP_MODE == MASTER_MODE
    void (*DownloadRxCallback)(uint8_t *pBuf, uint16_t Len);            // MCU���������·�����ص�
    void (*SyncTimeRxCallback)(uint8_t *pBuf, uint16_t Len);            // MCU��������ʱ�䴦��ص�
    void (*SyncDataTxCallback)(void);                                   // MCU��������ͬ������ص�
    #else
    void (*UploadRxCallback)(uint8_t *pBuf, uint16_t Len);              // ģ���յ������ϴ�����ص�
    void (*DevInfoRxCallback)(TS_HET_CP_DEVICEINFO* pThis);             // ģ���յ��豸��Ϣ����ص�
    void (*SyncDataRxCallback)(uint8_t *pBuf, uint16_t Len);            // ģ���յ�����ͬ������ص�
    void (*GetSysTimeInfo)(TS_HET_CP_TIME* pTime);                      // ģ���ȡϵͳʱ��ص�
    void (*FactoryTest)(void);                                          // ģ�����ص�
    void (*ResetProcess)(void);                                         // ģ�鸴λ����
    
    #endif
}TS_CPCOMM_FUNC;
//------------------------------------------------------------------------------
//����ṹ��
typedef struct
{
    TS_CPCOMM_RECV              RX;
    TS_CPCOMM_SEND              TX;
    TS_CPCOMM_FUNC              Func;                       // ����ָ��

    uint8_t 				    CpStatus;					//����״̬
    uint8_t                     CpError;                    //ͨѶ�쳣
    uint8_t                     CpFactoryTestStatus;        //����״̬
    uint32_t                    LocalDataType;              //�����ϱ���������
    uint32_t                    CmdFlag;

}TS_HET_CP;

#pragma pack()
/******************************************************************************\
                    Global variables and functions
\******************************************************************************/
/*
* �������� : HET_CP_Drive_UsartRecvISR
* �������� : CP���ݽ�������
* ��    �� : pThis- ���ָ��
             pBuf - ����ָ��
             Len  - ���ݳ���
* ����ֵ   : ��
* ʾ    �� : ��
*/
/******************************************************************************/
extern void HET_CP_Drive_UsartRecvISR(TS_HET_CP *pThis, uint8_t *pBuf, uint16_t Len);
/******************************************************************************/


/*
* �������� : HET_CP_Task
* �������� : CP����ÿ10ms����һ��
* ��    �� : ��
* ����ֵ   : ��
* ʾ    �� : HET_CP_Task();
*/
/******************************************************************************/
extern void HET_CP_Task(TS_HET_CP *pThis);
/******************************************************************************/


/*
* �������� : HET_CP_SendDataFrame
* �������� : �û����ݷ��ͺ���
* ��    �� : pThis    - ���ָ��
             Cmd      - ������
             DataId   - ����ID
             pDataBuf - ����ָ��
             DataLen  - ���ݳ���
* ����ֵ   : ��
* ʾ    �� : ��
*/
/******************************************************************************/
extern uint8_t HET_CP_SendDataFrame(TS_HET_CP *pThis, TE_HET_CP_CMD Cmd, uint8_t DataId, uint8_t* pDataBuf, uint8_t DataLen);
/******************************************************************************/


/*
* �������� : HET_CP_GetConnectStatus
* �������� : ��ȡ����״̬�ֽ�
* ��    �� : pThis    - ���ָ��
* ����ֵ   : Status - ����״̬�ֽ�
* ʾ    �� : HET_CP_GetConnectStatus(&Status);
*/
/******************************************************************************/
extern uint8_t HET_CP_GetConnectStatus(TS_HET_CP *pThis);
/******************************************************************************/


/*
* �������� : HET_CP_GetNetStatus
* �������� : ��ȡ��������״̬
* ��    �� : ��
* ����ֵ   : TE_HET_NET_STATUS - HET_ONLINE:����
                                 HET_OFFLINE:����
* ʾ    �� : InterStatus = HET_CP_GetNetStatus();
*/
/******************************************************************************/
extern TE_HET_NET_STATUS HET_CP_GetNetStatus(TS_HET_CP *pThis);
/******************************************************************************/

/*
* �������� : HET_CP_GetSignalStrength
* �������� : ��ȡWifi�ź�ǿ��
* ��    �� : ��
* ����ֵ   : WiFi�ź�ǿ��0~10
* ʾ    �� : CPSignalStrength = HET_CP_GetSignalStrength();
*/
/******************************************************************************/
extern uint8_t HET_CP_GetSignalStrength(TS_HET_CP *pThis);
/******************************************************************************/

/*
* �������� : HET_CP_GetData
* �������� : ��ȡ���ݷ���״̬
* ��    �� : ��
* ����ֵ   : HET_SEND_IDLE - ����
             HET_SEND_BUSY - ����������
* ʾ    �� : Status = HET_CP_SendStatus();
*/
/******************************************************************************/
extern TE_HET_SENDSTATUS HET_CP_SendStatus(TS_HET_CP *pThis);
/******************************************************************************/

/*
* �������� : HET_CP_GetFactoryTestStatus
* �������� : ��ȡ����״̬
* ��    �� : ��
* ����ֵ   : FACTORY_IDLE(δ����)
             FACTORY_TESTING(������)
             FACTORY_FAIL(����ʧ��)
             FACTORY_PASS(����ɹ�)
* ʾ    �� : Status = HET_CP_GetFactoryTestStatus();
*/
/******************************************************************************/
extern uint8_t HET_CP_GetFactoryTestStatus(TS_HET_CP* pThis);
/******************************************************************************/


/*
* �������� : HET_CP_Open
* �������� : ʹ�����
* ��    �� : pThis - ���ָ��
* ����ֵ   : TE_HET_STA���������
*/
/******************************************************************************/
extern uint8_t HET_CP_Open(TS_HET_CP* pThis);
/******************************************************************************/


#endif
/******************************* End of File (H) ******************************/
