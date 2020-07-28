/****************************************************************************************
**  版    权 :  深圳市和而泰智能控制股份有限公司所有（2020）
**  文件名称 :  GUI_DataConfig.c
**  功能描述 :  基于C-LIFE简化版协议的GUI数据配置
**  作    者 :  vincent
**  日    期 :  2020.07.01
**  版    本 :  V0.0.1
**  变更记录 :  V0.0.1/2020.07.01
                1 首次创建   
*****************************************************************************************/
#ifndef _GUI_DATA_CONFIG_H
#define _GUI_DATA_CONFIG_H
/*****************************************************************************************
								  Includes
*****************************************************************************************/
#include <stdbool.h>
#include <stdint.h>
/*****************************************************************************************
								Macro definitions
*****************************************************************************************/
#define         DATA_ENDIAN         0   //设备大小端，0，小端，1，大端（RAM芯片通常为小端，51芯片通常为大端）
/*****************************************************************************************
								Typedef definitions
*****************************************************************************************/
#pragma pack(1)

//协议数据
typedef struct
{
    uint8_t     mattress_mode;
    
    uint8_t     massage_on_off;
    uint8_t     massage_mode;
    uint8_t     massage_level;
    uint32_t    massage_time_set;
    uint32_t    massage_time_left;
    
    uint8_t     sleep_aid_on_off;
    uint8_t     sleep_aid_music_on_off;
    uint8_t     sleep_aid_music_index;
    uint8_t     sleep_aid_light_on_off;
    uint8_t     sleep_aid_light_level;
    uint8_t     sleep_aid_time;
    uint8_t     sleep_aid_mode;
    
    uint8_t     lamp_on_off;
    uint8_t     lamp_level;
    uint8_t     lamp_colour_temperature;

    uint8_t     alarm_music_on_off;
    uint8_t     alarm_music_index;
    uint8_t     alarm_other_ways;
    
    uint8_t     alarm1_on_off;
    uint8_t     alarm1_hour;
    uint8_t     alarm1_minute;
    uint8_t     alarm1_weekday;

    uint8_t     alarm2_on_off;
    uint8_t     alarm2_hour;
    uint8_t     alarm2_minute;
    uint8_t     alarm2_weekday;

    uint8_t     alarm3_on_off;
    uint8_t     alarm3_hour;
    uint8_t     alarm3_minute;
    uint8_t     alarm3_weekday;
    
    
    uint8_t     stop_snore_on_off;

    uint8_t     smart_set_in_bed;
    uint8_t     smart_set_after_sleep;
    uint8_t     smart_set_out_bed;


    uint8_t     temperature;                  //温度
    uint8_t     humidity;                     //湿度
    
    uint8_t	    key_sound_on_off;			  //按键声音开关，1：开/0：关
    uint8_t	    backlight;			          //背光亮度,
    uint32_t    time_stamp;                   //设置系统时间，使用时间戳
    uint32_t    error_code;                   //错误代码              

}TS_GUI_DATA;
#pragma pack()

//数据ID
typedef enum
{
    ID_ALL,

    ID_MATTRESS_MODE,
    
    ID_MASSAGE_ON_OFF,//2
    ID_MASSAGE_MODE,
    ID_MASSAGE_LEVEL,
    ID_MASSAGE_TIME_SET,
    ID_MASSAGE_TIME_LEFT,
    
    ID_SLEEP_AID_ON_OFF,//7
    ID_SLEEP_AID_MUSIC_ON_OFF,
    ID_SLEEP_AID_MUSIC_INDEX,
    ID_SLEEP_AID_LIGHT_ON_OFF,
    ID_SLEEP_AID_LIGHT_LEVEL,
    ID_SLEEP_AID_TIME,
    ID_SLEEP_AID_MODE,
    
    ID_LAMP_ON_OFF,//14
    ID_LAMP_LEVEL,
    ID_LAMP_COLOUR_TEMPERATURE,

    ID_ALARM_MUSIC_ON_OFF,
    ID_ALARM_MUSIC_INDEX,
    ID_ALARM_OTHER_WAYS,
    
    ID_ALARM1_ON_OFF,
    ID_ALARM1_HOUR,
    ID_ALARM1_MINUTE,
    ID_ALARM1_WEEKDAY,

    ID_ALARM2_ON_OFF,
    ID_ALARM2_HOUR,
    ID_ALARM2_MINUTE,
    ID_ALARM2_WEEKDAY,

    ID_ALARM3_ON_OFF,
    ID_ALARM3_HOUR,
    ID_ALARM3_MINUTE,
    ID_ALARM3_WEEKDAY,
    
    
    ID_STOP_SNORE_ON_OFF,//32

    ID_SMART_SET_IN_BED,
    ID_SMART_SET_AFTER_SLEEP,
    ID_SMART_SET_OUT_BED,

    ID_TEMPERATURE,
    ID_HUMIDITY,

    ID_KEY_SOUND_ON_OFF,
    ID_BACKLIGHT,	
    ID_TIME_STAMP, 
    ID_ERROR_CODE,
    
    ID_MAX
}TE_GUI_DATA_ID;

//数据长度
typedef enum
{
    LEN_ALL = 45,

    LEN_MATTRESS_MODE = 1,

    LEN_MASSAGE_ON_OFF = 1,
    LEN_MASSAGE_MODE = 1,
    LEN_MASSAGE_LEVEL = 1,
    LEN_MASSAGE_TIME_SET = 1,
    LEN_MASSAGE_TIME_LEFT = 2,


    LEN_SLEEP_AID_ON_OFF = 1,
    LEN_SLEEP_AID_MUSIC_ON_OFF = 1,
    LEN_SLEEP_AID_MUSIC_INDEX = 1,
    LEN_SLEEP_AID_LIGHT_ON_OFF = 1,
    LEN_SLEEP_AID_LIGHT_LEVEL = 1,
    LEN_SLEEP_AID_TIME = 1,
    LEN_SLEEP_AID_MODE = 1,

    LEN_LAMP_ON_OFF = 1,
    LEN_LAMP_LEVEL = 1,
    LEN_LAMP_COLOUR_TEMPERATURE = 1,

    LEN_ALARM_MUSIC_ON_OFF = 1,
    LEN_ALARM_MUSIC_INDEX = 1,
    LEN_ALARM_OTHER_WAYS = 1,

    LEN_ALARM1_ON_OFF = 1,
    LEN_ALARM1_HOUR = 1,
    LEN_ALARM1_MINUTE = 1,
    LEN_ALARM1_WEEKDAY = 1,

    LEN_ALARM2_ON_OFF = 1,
    LEN_ALARM2_HOUR = 1,
    LEN_ALARM2_MINUTE = 1,
    LEN_ALARM2_WEEKDAY = 1,

    LEN_ALARM3_ON_OFF = 1,
    LEN_ALARM3_HOUR = 1,
    LEN_ALARM3_MINUTE = 1,
    LEN_ALARM3_WEEKDAY = 1,


    LEN_STOP_SNORE_ON_OFF = 1,

    LEN_SMART_SET_IN_BED = 1,
    LEN_SMART_SET_AFTER_SLEEP = 1,
    LEN_SMART_SET_OUT_BED = 1,

    LEN_TEMPERATURE = 1,
    LEN_HUMIDITY = 1,

    LEN_KEY_SOUND_ON_OFF = 1,
    LEN_BACKLIGHT = 1,
    LEN_TIME_STAMP = 4,
    LEN_ERROR_CODE = 4,
}TE_GUI_DATA_LEN;


typedef enum
{
    WORK_OFF,
    WORK_ON,
}TE_GUI_WORK_STATUS;
/*
//数据单元
typedef struct
{
    uint8_t	 DataId;			                        //数据ID
    uint8_t  DataLen;                                   //数据长度
    uint8_t* pData;			                            //数据指针
}TS_GUI_DATA_UNIT;
*/

/*****************************************************************************************
								Global variables and functions
*****************************************************************************************/
extern TS_GUI_DATA g_GUI_Data;
//----------------------------------------------------------------------------------------
#endif
/***********************************END OF FILE*******************************************/
