/******************************************************************************\
**  ��    Ȩ :  �����кͶ�̩���ܿ��ƹɷ����޹�˾���У�2020��
**  �ļ����� :  HET_ClifeProtocol.c
**  �������� :  CPͨѶ�����������������ͨѶ����ʷ���ݽ����Լ���������
**  ��    �� :  vincent
**  ��    �� :  2020.07.01
**  ��    �� :  V0.0.1
**  �����¼ :  V0.0.1/2020.07.01
                1 �״δ���  
				
\******************************************************************************/


/******************************************************************************\
                                 Includes
\******************************************************************************/
#include "HET_ClifeProtocol.h"
/******************************************************************************\
                             Variables definitions
\******************************************************************************/
//���ز�������
typedef enum
{
    CMD_FLG_IDLE        = 0x00000000,		//����
    CMD_FLG_HEARTBEAT   = 0x00000001,       //����
    CMD_FLG_DEVICEINFO  = 0x00000002,		//��ѯ��Ʒ��Ϣ
    CMD_FLG_GETDATE     = 0x00000004,		//��ȡʱ����Ϣ
    CMD_FLG_BINDING     = 0x00000008,		//��
    CMD_FLG_TEST        = 0x00000010,		//����
    CMD_FLG_TESTRESULT  = 0x00000020,		//����������
    CMD_FLG_DOWNLOAD    = 0x00000040,		//�����·�
    CMD_FLG_UPLOAD      = 0x00000080,		//�����ϱ�
    CMD_FLG_DATASYNC    = 0x00000100,		//����ͬ��
    CMD_FLG_RESET       = 0x00000200,		//��λ
    CMD_FLG_UNBIND      = 0x00000400,       //�����
}TE_HET_LOCAL_CP_CMD;

//CPЭ��ṹ
typedef enum
{
    CP_INDEX_PACKHEAD,                                      //��ͷ
    CP_INDEX_CMD,                                           //�����ָ��ֽ�
    CP_INDEX_DATA_LEN_H,						            //���ݳ��ȸ��ֽ�
    CP_INDEX_DATA_LEN_L,						            //���ݳ��ȵ��ֽ�
    CP_INDEX_DATA,                                          //��������
}TE_HET_CPDATAINDEX;
//------------------------------------------------------------------------------
//CP���ڽ���״̬
typedef enum
{
    HISTORY_SEND_IDLE,                                      //���Ϳ���
    HISTORY_SEND_BUSY,                                      //������
    HISTORY_SEND_SUCESS,                                    //���ͳɹ�
    HISTORY_SEND_FAIL,                                      //����ʧ��
}TE_CP_HISTORY_SEND_STATUS;

//------------------------------------------------------------------------------
static TS_HET_CP    *s_HetCP[HET_PLUGIN_CP_NUM] = { 0 };
static uint8_t      s_HetCPNum = 0;
//------------------------------------------------------------------------------
//�豸��Ϣ
#pragma pack(1)
#if CP_MODE == MASTER_MODE
const TS_HET_CP_DEVICEINFO s_DeviceInfo =
{
    SOFT_VERSION,
    HARD_VERSION,
    { DEVICE_MODEL },
    { DEVICE_RESERVED }
};
#else
TS_HET_CP_DEVICEINFO s_DeviceInfo;
#endif

//ϵͳʱ��
TS_HET_CP_TIME s_SysTime;

#pragma pack()

/******************************************************************************\
                             Functions definitions
\******************************************************************************/


/*
* �������� : Het_CP_Drive_CheckSum
* �������� : CPЭ������У��
* ��    �� : pBuf:����ָ��
             Len:���ݳ���
             pChecksum:У����
* ����ֵ   : ��
* ʾ    �� : Het_CP_Drive_CheckSum(pBuf,Len,&pChecksum);
*/
/******************************************************************************/
static void Het_CP_Drive_CheckSum(uint8_t *pBuf,uint16_t Len,uint8_t *pChecksum)
/******************************************************************************/
{
	uint16_t i;
	uint8_t checksum = 0;
	for(i=0; i<Len; i++)
	{
		checksum += *pBuf++;
	}
	*pChecksum = (uint8_t)checksum;
}

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
void HET_CP_Drive_UsartRecvISR(TS_HET_CP *pThis, uint8_t *pBuf, uint16_t Len)
/******************************************************************************/
{
	static uint16_t data_len = 0;
    // �������ݴ��
	if(Len == 1)                                    //���ֽڽ���
	{
        switch (pThis->RX.ReceiveFlow)
		{
			//�ж�������ʼ֡
			case FLOW_UART_RECV_BEGIN:
				if(*pBuf == CP_PACK_HEAD)           //���յ�֡ͷ
				{
                    pThis->RX.ReceiveBuf[pThis->RX.ReceiveLen++] = *pBuf;
                    pThis->RX.ReceiveFlow = FLOW_UART_RECV_FINISH;
				}
				break;
			//�жϴ��ڽ����Ƿ���� 
			case FLOW_UART_RECV_FINISH:
                pThis->RX.ReceiveBuf[pThis->RX.ReceiveLen++] = *pBuf;
                if (pThis->RX.ReceiveLen == CP_INDEX_DATA)    //��ȡ�����ݳ���
				{
                    data_len = (((uint16_t)(pThis->RX.ReceiveBuf[CP_INDEX_DATA_LEN_H])) << 8) + *pBuf;
				}
                if (pThis->RX.ReceiveLen > CP_INDEX_DATA)
				{
                    if (pThis->RX.ReceiveLen >= data_len + 1)
					{
                        pThis->RX.ReceiveFlow = FLOW_UART_RECV_BEGIN;
                        pThis->RX.ReceiveF = true;          //�������
                        pThis->CpError = false;               //CPͨѶ����
					}
				}
				break;
				
			default:break;	
		}
	}	
	else//��֡����                                                        
	{
		if(pBuf[CP_INDEX_PACKHEAD] == CP_PACK_HEAD)         //���յ�֡ͷ
		{
            memcpy(pThis->RX.ReceiveBuf, pBuf, Len);     //�����ڴ��е�����
            pThis->RX.ReceiveLen = Len;                //ȡ��CRCУ������ݳ���
            pThis->RX.ReceiveF = true;                 //�������
		}
	}
}

/*
* �������� : Het_CP_Drive_UpdateSendTempBuf
* �������� : �������ݸ��£�����Ҫ���͵����ݸ��µ�����
* ��    �� : pThis    - ���ָ��
             DataId   - ����ID
             pDataBuf - ����ָ��
             DataLen  - ���ݳ���
* ����ֵ   : ��
* ʾ    �� : ��
*/
/******************************************************************************/
static uint8_t Het_CP_Drive_UpdateSendTempBuf(TS_HET_CP *pThis, uint8_t DataId, uint8_t *pDataBuf, uint8_t DataLen)
/******************************************************************************/
{
    uint8_t Cmd = 0;
    #if (CP_MODE == MASTER_MODE)
    Cmd = CMD_UPLOAD;
    #else
    Cmd = CMD_DOWNLOAD;
    #endif

    if(pDataBuf == 0 || DataLen == 0) return false;             //����Ϊ��
    if(pThis->TX.UpdateIndex >= CP_SEND_TEMP_LEN) return false; //������������
    
    pThis->TX.SendTempBuff[pThis->TX.UpdateIndex++] = DataId;               //��������ID
    pThis->TX.SendTempBuff[pThis->TX.UpdateIndex++] = DataLen;              //�������ݳ���
    memcpy(&pThis->TX.SendTempBuff[pThis->TX.UpdateIndex],pDataBuf,DataLen);//������������
    pThis->TX.UpdateIndex += DataLen;
    pThis->CmdFlag |= (uint32_t)1<<(Cmd-1); //��¼CMD�����д��ڷ��ͷ�ʱ������ֹ������Դ��ͻ�����ϱ����ݵ�ʱ�򴥷��˰�
    return true;
}


/*
* �������� : Het_CP_Drive_DataSync
* �������� : ����ͬ������һ���Է���ȫ�����ݣ�����ID
* ��    �� : pThis    - ���ָ��
             Cmd      - ������
             DataId   - ����ID
             pDataBuf - ����ָ��
             DataLen  - ���ݳ���
* ����ֵ   : ��
* ʾ    �� : ��
*/
/******************************************************************************/
static uint8_t Het_CP_Drive_DataSync(TS_HET_CP *pThis, TE_HET_CP_CMD Cmd, uint8_t DataId, uint8_t *pDataBuf, uint8_t DataLen)
/******************************************************************************/
{
    uint16_t send_len = 0;
	uint8_t checksum = 0;

    

    if (pThis->TX.SendDelay == HISTORY_SEND_IDLE)
    {
        memset(pThis->TX.SendBuff, 0, CP_SEND_LEN);                    //����������������
        //��ͷ
        pThis->TX.SendBuff[send_len++] = CP_PACK_HEAD;
        //������
        pThis->TX.SendBuff[send_len++] = Cmd;
        //�����ܳ���
        pThis->TX.SendBuff[send_len++] = (uint8_t)((DataLen+2) >> 8);
        pThis->TX.SendBuff[send_len++] = (uint8_t)((DataLen+2));

        //����ID
        pThis->TX.SendBuff[send_len++] = 0;
        //���ݳ���
        pThis->TX.SendBuff[send_len++] = DataLen;
        //��������
        memcpy(pThis->TX.SendBuff + send_len, pDataBuf, DataLen);
        send_len += DataLen;
        

        //���� checksum
        Het_CP_Drive_CheckSum(pThis->TX.SendBuff + 1, send_len - 1, &checksum);
        pThis->TX.SendBuff[send_len++] = checksum;
        //��������
        pThis->TX.SendDelay = HET_CP_SEND_CYCLE;              //��֤ÿ֡��������֮���м��,����������������  

        if(Cmd == CMD_DATASYNC)
        {
            pThis->CmdFlag &= ~(uint32_t)1<<(Cmd-1);              //���CMD��־
        }
        
        
        pThis->Func.UartSendCallback(pThis->TX.SendBuff, send_len); //���ô��ڵײ㷢�ͺ���
        
        return true; 
    }   
    else
    {
        return false;
    }
}

/*
* �������� : Het_CP_Drive_DataSend
* �������� : CPЭ����������,������
* ��    �� : pThis    - ���ָ��
             Cmd      - ������
             pDataBuf - ����ָ��
             DataLen  - ���ݳ���
* ����ֵ   : ��
* ʾ    �� : ��
*/
/******************************************************************************/
static uint8_t Het_CP_Drive_DataSend(TS_HET_CP *pThis,TE_HET_CP_CMD Cmd, uint8_t *pBuf, uint16_t Len)
/******************************************************************************/
{
	uint16_t send_len = 0;
	uint8_t checksum = 0;
    
    if (pThis->TX.SendDelay == HISTORY_SEND_IDLE)
    {
        memset(pThis->TX.SendBuff, 0, CP_SEND_LEN);                    //����������������
        //��ͷ
        pThis->TX.SendBuff[send_len++] = CP_PACK_HEAD;
        //������
        pThis->TX.SendBuff[send_len++] = Cmd;
        //�����ܳ���
        pThis->TX.SendBuff[send_len++] = (uint8_t)(Len >> 8);
        pThis->TX.SendBuff[send_len++] = (uint8_t)(Len);
         
        if (pBuf!=0 && Len>0)
        {
            memcpy(pThis->TX.SendBuff+send_len, pBuf, Len);
            if(Cmd == CMD_UPLOAD) 
            {
                memset(pThis->TX.SendTempBuff,0,CP_SEND_TEMP_LEN);//�����ϱ�������������������������
                pThis->TX.UpdateIndex = 0;
            }
            
            send_len += Len;
        }

        //���� checksum
        Het_CP_Drive_CheckSum(pThis->TX.SendBuff + 1, send_len - 1, &checksum);
        pThis->TX.SendBuff[send_len++] = checksum;
        //��������
        pThis->TX.SendDelay = HET_CP_SEND_CYCLE;              //��֤ÿ֡��������֮���м��,����������������  
        pThis->CmdFlag &= ~(uint32_t)1<<(Cmd-1);              //���CMD��־
        
        pThis->Func.UartSendCallback(pThis->TX.SendBuff, send_len); //���ô��ڵײ㷢�ͺ���
        
        return true; 
    } 
    else
    {
        return false;
    }    
	 
}



/*
* �������� : Het_CP_Drive_Process
* �������� : CPͨѶ���̿���״̬��
* ��    �� : pThis    - ���ָ��
* ����ֵ   : ��
* ʾ    �� : Het_CP_Drive_Process();
*/
/******************************************************************************/
static void Het_CP_Drive_Process(TS_HET_CP *pThis)
/******************************************************************************/
{
    uint8_t checksum;
    uint8_t cmd;
//    uint8_t *pbuf;
    uint16_t len;
    
    if(pThis->RX.ReceiveF == true)
    {
        pThis->RX.ReceiveF = false;

        
        //���г�ȥ��ͷ������У��
        Het_CP_Drive_CheckSum(pThis->RX.ReceiveBuf + 1, pThis->RX.ReceiveLen - 2, &checksum);

        if (checksum == pThis->RX.ReceiveBuf[pThis->RX.ReceiveLen - 1])
        {
            //��ȡ������ID
            cmd = pThis->RX.ReceiveBuf[CP_INDEX_CMD];
            //��ȡ���ݳ���
            len = ((uint16_t)pThis->RX.ReceiveBuf[CP_INDEX_DATA_LEN_H] << 8) 
                | pThis->RX.ReceiveBuf[CP_INDEX_DATA_LEN_L];
            //��鳤��
            if (pThis->RX.ReceiveLen != (len + 5)) return; 

            switch(cmd)
            {
                //default֮ǰ��case��Ϊ������������ݣ����������Զ�Ӧ��ʽ
                //����
                case CMD_HEARTBEAT:
                    
                    #if CP_MODE == MASTER_MODE
                    //MCU�յ�ģ������
                    pThis->CmdFlag |= CMD_FLG_HEARTBEAT;
                    pThis->CpStatus = pThis->RX.ReceiveBuf[CP_INDEX_DATA];//��ȡCP������״̬
                    #else
                    //ģ���յ�MCUӦ��
                    DEBUG("\nReceived heartbeat ACK!");
                    #endif
                    
                break;
                
                //��ѯ��Ʒ��Ϣ
                case CMD_DEVICEINFO:
                    
                    #if CP_MODE == MASTER_MODE
                    //MCU�յ�ģ���ѯ��Ʒ��Ϣָ��
                    pThis->CmdFlag |= CMD_FLG_DEVICEINFO;
                    #else
                     //ģ���յ�MCU�������豸��Ϣ
                    memcpy((uint8_t*)&s_DeviceInfo, &pThis->RX.ReceiveBuf[CP_INDEX_DATA], sizeof(TS_HET_CP_DEVICEINFO));
                    pThis->Func.DevInfoRxCallback(&s_DeviceInfo);
                    #endif
                    
                    break;  
                    
                //ͬ��ʱ��
                case CMD_GETDATE:
                    
                    #if CP_MODE == MASTER_MODE
                   //MCU�յ�ģ��Ӧ���ʱ����Ϣ
                    pThis->Func.SyncTimeRxCallback(&pThis->RX.ReceiveBuf[CP_INDEX_DATA],len);
                    #else
                    //ģ���յ�MCU�����Ļ�ȡʱ����Ϣָ��
                    pThis->CmdFlag |= CMD_FLG_GETDATE;
                    #endif

                break;
                
                //��
                case CMD_BINDING:
                     
                    #if CP_MODE == MASTER_MODE
                    
                    #else
                    //ģ���յ�MCU�����İ�ָ��
                    //DoBindProcess()
                    pThis->CmdFlag |= CMD_FLG_BINDING;  
                    #endif
                    
                    break;
                
                //����
                case CMD_TEST:
                    
                    #if CP_MODE == MASTER_MODE
                    
                    #else
                    //ģ���յ�MCU�����Ĳ���ָ��
                    //DoFactoryTestProcess()
                    pThis->CmdFlag |= CMD_FLG_TEST;  
                    #endif
                    
                    break;
                
                //����������
                case CMD_TESTRESULT:
                    
                    #if CP_MODE == MASTER_MODE
                    //��ȡ������
                    pThis->CpFactoryTestStatus = pThis->RX.ReceiveBuf[CP_INDEX_DATA];
                    pThis->CmdFlag |= CMD_FLG_TESTRESULT;
                    #else

                    
                    #endif
                    
                    break;

                //�����·�
                case CMD_DOWNLOAD:
                    
                    #if CP_MODE == MASTER_MODE
                    //MCU�յ�ģ���·����ݴ���
                    pThis->Func.DownloadRxCallback(&pThis->RX.ReceiveBuf[CP_INDEX_DATA], len);
                    pThis->CmdFlag |= CMD_FLG_DOWNLOAD;
                    #else

                    #endif

                    break;
                
                //�����ϱ�
                case CMD_UPLOAD:
                    
                    #if CP_MODE == MASTER_MODE
                    
                    #else
                    //ģ���յ�MCU�����ϱ�ָ��
                    pThis->Func.UploadRxCallback(&pThis->RX.ReceiveBuf[CP_INDEX_DATA], len);
                    pThis->CmdFlag |= CMD_FLG_UPLOAD;
                    #endif
                    
                    break;
                
                //����ͬ��
                case CMD_DATASYNC:
                    
                    #if CP_MODE == MASTER_MODE
                    //MCU�յ�ģ�鷢��������ͬ��ָ��
                    pThis->CmdFlag |= CMD_FLG_DATASYNC;
                    #else
                    //ģ���յ�MCU����������ͬ������
                    pThis->Func.SyncDataRxCallback(&pThis->RX.ReceiveBuf[CP_INDEX_DATA], len);
                    #endif
                    
                    break;
                
                //��λ
                case CMD_RESET:
                    
                    #if CP_MODE == MASTER_MODE
                    
                    #else
                    //ģ���յ�MCU�����ĸ�λģ��ָ��
                    pThis->CmdFlag |= CMD_FLG_RESET;
                    #endif
                    
                    break;
                
                default:
                    
                break;
            }    
        }    
        memset(pThis->RX.ReceiveBuf,0,CP_RECV_LEN);
    }
    else
    {
        //�����ϱ�
        if ((pThis->CmdFlag & CMD_FLG_UPLOAD) == CMD_FLG_UPLOAD)
        {
            #if CP_MODE == MASTER_MODE
            Het_CP_Drive_DataSend(pThis, CMD_UPLOAD, pThis->TX.SendTempBuff, pThis->TX.UpdateIndex);        
            #else
            
            //ģ��Ӧ��MCU�����ϱ�
            Het_CP_Drive_DataSend(pThis,CMD_UPLOAD,0,0);
            #endif
            
        }
        //����Ӧ��
        else if ((pThis->CmdFlag & CMD_FLG_HEARTBEAT) == CMD_FLG_HEARTBEAT)
        {
            
            #if CP_MODE == MASTER_MODE
            //mcuӦ��
            Het_CP_Drive_DataSend(pThis,CMD_HEARTBEAT,0,0);
            #else

            #endif
        }
        //Ӧ���Ʒ��Ϣ
        else if ((pThis->CmdFlag & CMD_FLG_DEVICEINFO) == CMD_FLG_DEVICEINFO)
        {
            
            #if CP_MODE == MASTER_MODE
            //mcuӦ��
            Het_CP_Drive_DataSend(pThis,CMD_DEVICEINFO, (uint8_t*)&s_DeviceInfo, sizeof(s_DeviceInfo));
             #else

            #endif
        }
        //��ȡʱ����Ϣ
        else if ((pThis->CmdFlag & CMD_FLG_GETDATE) == CMD_FLG_GETDATE)
        {
            
            #if CP_MODE == MASTER_MODE
            
            
            #else
            //WiFiģ��Ӧ��MCU�Ļ�ȡʱ����Ϣָ��
            if(pThis->TX.SendDelay == HISTORY_SEND_IDLE)
            {
                pThis->Func.GetSysTimeInfo(&s_SysTime);
                Het_CP_Drive_DataSend(pThis, CMD_GETDATE, (uint8_t*)&s_SysTime, sizeof(TS_HET_CP_TIME));
            }
            
            
            #endif
        }
        //��
        else if ((pThis->CmdFlag & CMD_FLG_BINDING) == CMD_FLG_BINDING)
        {
            #if CP_MODE == MASTER_MODE
                
            #else
            //ģ��Ӧ��MCU�İ�ָ��
            Het_CP_Drive_DataSend(pThis, CMD_BINDING, 0, 0);
            
            #endif
            
        }
        //����
        else if ((pThis->CmdFlag & CMD_FLG_TEST) == CMD_FLG_TEST)
        {
            #if CP_MODE == MASTER_MODE

            #else
            //ģ��Ӧ��MCU�Ĳ���ָ��
            if(pThis->TX.SendDelay == HISTORY_SEND_IDLE)
            {
                Het_CP_Drive_DataSend(pThis, CMD_TEST, 0, 0);
                pThis->Func.FactoryTest();//ִ�в��⹦��
            }
            
            #endif
            
        }
        //Ӧ����Խ��
        else if ((pThis->CmdFlag & CMD_FLG_TESTRESULT) == CMD_FLG_TESTRESULT)
        {
            #if CP_MODE == MASTER_MODE
            //MCUӦ��ģ��Ĳ�����
            Het_CP_Drive_DataSend(pThis, CMD_TESTRESULT, 0, 0);      
            #else

            #endif
        }
        //Ӧ�������·�
        else if ((pThis->CmdFlag & CMD_FLG_DOWNLOAD) == CMD_FLG_DOWNLOAD)
        {
            
            #if CP_MODE == MASTER_MODE
            //MCUӦ��ģ�������·�ָ��
            Het_CP_Drive_DataSend(pThis, CMD_DOWNLOAD, 0, 0);     
            #else
            Het_CP_Drive_DataSend(pThis, CMD_UPLOAD, pThis->TX.SendTempBuff, pThis->TX.UpdateIndex);        
            #endif
        }
        
        //����ͬ��
        else if ((pThis->CmdFlag & CMD_FLG_DATASYNC) == CMD_FLG_DATASYNC)
        {
            
            #if CP_MODE == MASTER_MODE
            //MCUӦ��ģ������ͬ��ָ��
            pThis->Func.SyncDataTxCallback();        
             #else
            
            #endif
        }
        //��λ
        else if ((pThis->CmdFlag & CMD_FLG_RESET) == CMD_FLG_RESET)
        {
            #if CP_MODE == MASTER_MODE
                    
            #else
            if(pThis->TX.SendDelay == HISTORY_SEND_IDLE)
            {
                Het_CP_Drive_DataSend(pThis, CMD_RESET, 0, 0);
                if(pThis->Func.ResetProcess)
                {
                    pThis->Func.ResetProcess();
                }
            }
            
            #endif
        }
    }        
}



/*
* �������� : HET_CP_Task
* �������� : CP����ÿ10ms����һ��
* ��    �� : ��
* ����ֵ   : ��
* ʾ    �� : HET_CP_Task();
*/
/******************************************************************************/
void HET_CP_Task(TS_HET_CP *pThis)
/******************************************************************************/
{
    
    // ���Ʒ��ͼ��
    if(pThis->TX.SendDelay)
    {
        pThis->TX.SendDelay--;          //�����������ͼ������,����������������
    }

    // ����ͨѶЭ��״̬��
    Het_CP_Drive_Process(pThis);
    
}

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
uint8_t HET_CP_SendDataFrame(TS_HET_CP *pThis, TE_HET_CP_CMD Cmd, uint8_t DataId, uint8_t* pDataBuf, uint8_t DataLen)
/******************************************************************************/
{
    uint8_t res = 0;

    if(Cmd == CMD_DOWNLOAD || Cmd == CMD_UPLOAD || Cmd == CMD_DATASYNC) //�����ϱ�
    {
        if(DataId == 0) //ȫ������ģʽ,ֱ�Ӹ��µ�һ�����沢��������һ�δ��ڷ���
        {
            res = Het_CP_Drive_DataSync(pThis, Cmd, DataId, pDataBuf, DataLen);
        }
        else //������ģʽ���������ݵ��������棬�ñ��λ������������
        {
            res = Het_CP_Drive_UpdateSendTempBuf(pThis, DataId, pDataBuf, DataLen);
        }
    }
    else
    {
        res = Het_CP_Drive_DataSend(pThis, Cmd, pDataBuf, DataLen);
    }
    return res;
}

/*
* �������� : HET_CP_GetConnectStatus
* �������� : ��ȡ����״̬�ֽ�
* ��    �� : pThis    - ���ָ��
* ����ֵ   : Status - ����״̬�ֽ�
* ʾ    �� : HET_CP_GetConnectStatus(&Status);
*/
/******************************************************************************/
uint8_t HET_CP_GetConnectStatus(TS_HET_CP *pThis)
/******************************************************************************/
{
    return pThis->CpStatus;
}

/*
* �������� : HET_CP_GetNetStatus
* �������� : ��ȡ��������״̬
* ��    �� : ��
* ����ֵ   : TE_HET_NET_STATUS - HET_ONLINE:����
                                 HET_OFFLINE:����
* ʾ    �� : InterStatus = HET_CP_GetNetStatus();
*/
/******************************************************************************/
TE_HET_NET_STATUS HET_CP_GetNetStatus(TS_HET_CP *pThis)
/******************************************************************************/
{     
    if ((pThis->CpStatus & 0x70) == 0x70)      //�ж��Ƿ�����
    {
        return HET_ONLINE;         //����    
    }
    else
    {
        return HET_OFFLINE;        //����
    }    
}
/*
* �������� : HET_CP_GetSignalStrength
* �������� : ��ȡWifi�ź�ǿ��
* ��    �� : ��
* ����ֵ   : WiFi�ź�ǿ��0~10
* ʾ    �� : CPSignalStrength = HET_CP_GetSignalStrength();
*/
/******************************************************************************/
uint8_t HET_CP_GetSignalStrength(TS_HET_CP *pThis)
/******************************************************************************/
{   
    return (pThis->CpStatus & 0x0f);
}
/*
* �������� : HET_CP_GetData
* �������� : ��ȡ���ݷ���״̬
* ��    �� : ��
* ����ֵ   : HET_SEND_IDLE - ����
             HET_SEND_BUSY - ����������
* ʾ    �� : Status = HET_CP_SendStatus();
*/
/******************************************************************************/
TE_HET_SENDSTATUS HET_CP_SendStatus(TS_HET_CP *pThis)
/******************************************************************************/
{
    //�ж��Ƿ��ڷ���������
    if (pThis->TX.SendDelay)
    {
        return HET_SEND_BUSY;
    }    
    else
    {
        return HET_SEND_IDLE;
    }    
}
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
uint8_t HET_CP_GetFactoryTestStatus(TS_HET_CP* pThis)
/******************************************************************************/
{   
    return pThis->CpFactoryTestStatus;    
}

/*
* �������� : HET_CP_Open
* �������� : ʹ�����
* ��    �� : pThis - ���ָ��
* ����ֵ   : TE_HET_STA���������
*/
/******************************************************************************/
uint8_t HET_CP_Open(TS_HET_CP* pThis)
/******************************************************************************/
{
    uint8_t i;

    // �ж��Ƿ��п�ָ��ʹ��
    for (i = 0; i < HET_PLUGIN_CP_NUM; i++)
    {
        if (0 == s_HetCP[i])
        {
            break;
        }
    }

    // �������������
    if (i >= HET_PLUGIN_CP_NUM)
    {
        //HET_ASSERT_PERROR(HET_ERROR_MAX);
        return 1;
    }

    /* ������� */
    if ((pThis == 0)
        || (pThis->Func.UartSendCallback == 0)
#if CP_MODE == MASTER_MODE
        || (pThis->Func.DownloadRxCallback == 0)
        || (pThis->Func.SyncTimeRxCallback == 0)
        || (pThis->Func.SyncDataTxCallback == 0)
#else
        || (pThis->Func.UploadRxCallback == 0)
        || (pThis->Func.DevInfoRxCallback == 0)
        || (pThis->Func.SyncDataRxCallback == 0)
        || (pThis->Func.GetSysTimeInfo == 0)
#endif
        )
    {
        while (1);
    }


    // ָ�븳ֵ
    s_HetCP[i] = pThis;

    
    // ��������
    //Thread_Create(HET_CP_Task);

    s_HetCPNum++;

    //SYS_LOG_INFO("ID %d: Init OK!", i);

    return 0;
}

/******************************* End of File (C) ******************************/

