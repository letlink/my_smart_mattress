#include <sys/ioctl.h>
#include <assert.h>
#include <mqueue.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "ite/ith.h"
#include "ite/itp.h"
//#include "openrtos/FreeRTOS.h"

#define BEEP_TYPE           1 //0为IO控制的有缘蜂有源，1为PWM控制的无源蜂鸣器
#define BEEP_GPIO_PIN	    18
#define BEEP_GPIO_MODE	    ITH_GPIO_MODE2    //ITH_GPIO_MODE3
#define BEEP_PWM_NUM 		ITH_PWM2	      //ITH_PWM1
/* IT970 sdk中timer預設使用狀況。
1: IT976 backlight use
2: free
3: VCD :itp_stats_tascvcd.c
4: IT972 backlight use. Or RTC use for IT976
5: RTC use for IT972. Or freeRtos task use if define configCHECK_CRITICAL_TIME.
6: free
7: Uart use only in sleep mode
8: freeRtos portTimer
*/


#define BEEP_FREQ			2700				// 
#define BEEP_DUTY_CYCLE	    50					// 


static timer_t beepTimerId;
static void BeepFinishHandler(timer_t timerid, int arg)
{
#if BEEP_TYPE
	ithPwmDisable(BEEP_PWM_NUM, BEEP_GPIO_PIN);
#else
    ithGpioClear(BEEP_GPIO_PIN);
#endif
}

void BuzzerInit(void)
{
#if BEEP_TYPE
	ithPwmInit(BEEP_PWM_NUM, BEEP_FREQ, BEEP_DUTY_CYCLE);		// beep init
	ithPwmReset(BEEP_PWM_NUM, BEEP_GPIO_PIN, BEEP_GPIO_MODE);
	ithPwmDisable(BEEP_PWM_NUM, BEEP_GPIO_PIN);
#else
    ithGpioSetOut(BEEP_GPIO_PIN);
    ithGpioSetMode(BEEP_GPIO_PIN, BEEP_GPIO_MODE);
#endif
    beepTimerId         = 0;
    timer_create(CLOCK_REALTIME, NULL, &beepTimerId);
    timer_connect(beepTimerId, BeepFinishHandler, 0);
}

void BuzzerExit(void)
{
#if BEEP_TYPE
	ithPwmDisable(BEEP_PWM_NUM, BEEP_GPIO_PIN);
#else
    ithGpioClear(BEEP_GPIO_PIN);
#endif
    if (beepTimerId)
    {
        timer_delete(beepTimerId);
        beepTimerId = 0;
    }
}

void BuzzerPlay(uint32_t msBeepInterval, uint32_t duty)
{
    struct itimerspec value;
	// set timeout
    value.it_value.tv_sec   = msBeepInterval/1000;
    value.it_value.tv_nsec  = (msBeepInterval%1000)*1000*1000;
    value.it_interval.tv_sec = value.it_interval.tv_nsec = 0;    
    timer_settime(beepTimerId, 0, &value, NULL);
#if BEEP_TYPE
	ithPwmSetDutyCycle(BEEP_PWM_NUM, duty);
	ithPwmEnable(BEEP_PWM_NUM, BEEP_GPIO_PIN,BEEP_GPIO_MODE);
#else
    ithGpioSet(BEEP_GPIO_PIN);
#endif
}

void PlayKeyBeep(void)
{
	BuzzerPlay(100, BEEP_DUTY_CYCLE);
    
}

void PlayConfigBeep(void)
{
	BuzzerPlay(100, BEEP_DUTY_CYCLE);
}

void PlayWarningBeep(void)
{
	// to implement
}

void PlayDelBeep(void)
{
	// to implement
}


