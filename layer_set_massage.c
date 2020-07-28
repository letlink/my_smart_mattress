#include <assert.h>
#include "scene.h"

#include "clife_lite/GUI_ClifeProtocol.h"
#include "clife_lite/GUI_DataConfig.h"

#define MASSAGE_TIME_5_MIN        (10*1000)
#define MASSAGE_TIME_10_MIN       (10*60*1000)
#define MASSAGE_TIME_15_MIN       (15*60*1000)


static ITURadioBox* setMassageTime1RadioBox;
static ITURadioBox* setMassageTime2RadioBox;
static ITURadioBox* setMassageTime3RadioBox;


/* widgets:
setMassageLayer
setMassageBackground
setMassageTimeText
setMassageTime3RadioBox
setMassageTime2RadioBox
setMassageTime1RadioBox
setMassageBackButton
*/

extern void MassageStart(void);
extern void MassageStop(void);


int GetMassageSetTime(void)
{
    if((g_GUI_Data.massage_time_set == 0) || (g_GUI_Data.massage_time_set > MASSAGE_TIME_15_MIN))
    {
        g_GUI_Data.massage_time_set = MASSAGE_TIME_5_MIN;
    }
    return g_GUI_Data.massage_time_set;
}

bool SetMassageLayerOnEnter(ITUWidget* widget, char* param)
{
    setMassageTime1RadioBox = ituSceneFindWidget(&theScene, "setMassageTime1RadioBox");
    assert(setMassageTime1RadioBox);

    setMassageTime2RadioBox = ituSceneFindWidget(&theScene, "setMassageTime2RadioBox");
    assert(setMassageTime2RadioBox);

    setMassageTime3RadioBox = ituSceneFindWidget(&theScene, "setMassageTime3RadioBox");
    assert(setMassageTime3RadioBox);

    
    return true;
}

bool SetMassageTimeRadioBoxOnPress(ITUWidget* widget, char* param)
{
    uint32_t set_massage_time = 0; //Ä¬ÈÏÊ±¼ä
    if (ituRadioBoxIsChecked(setMassageTime1RadioBox))
    {
        set_massage_time = MASSAGE_TIME_5_MIN;
    }
    else if (ituRadioBoxIsChecked(setMassageTime2RadioBox))
    {
        set_massage_time = MASSAGE_TIME_10_MIN;
    }
    else
    {
        set_massage_time = MASSAGE_TIME_15_MIN;
    }
    g_GUI_Data.massage_time_set = set_massage_time;
    
    return true;
}

