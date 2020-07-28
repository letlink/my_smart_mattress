#include "ite/itu.h"

extern bool MainLayerOnEnter(ITUWidget* widget, char* param);
extern bool MainLayerOnTimer(ITUWidget* widget, char* param);
extern bool MainStopSnoreCheckBoxOnPress(ITUWidget* widget, char* param);
extern bool MainLampCheckBoxOnPress(ITUWidget* widget, char* param);
extern bool MainSleepAidCheckBoxOnPress(ITUWidget* widget, char* param);
extern bool MainMassageCheckBoxOnPress(ITUWidget* widget, char* param);
extern bool MainMattressModeRadioBoxOnPress(ITUWidget* widget, char* param);
extern bool MassageLayerOnEnter(ITUWidget* widget, char* param);
extern bool MassageLayerOnTimer(ITUWidget* widget, char* param);
extern bool MassageModeRadioBox(ITUWidget* widget, char* param);
extern bool MassageLevelRadioBox(ITUWidget* widget, char* param);
extern bool MassageStartCheckBoxOnPress(ITUWidget* widget, char* param);
extern bool SleepAidLayerOnEnter(ITUWidget* widget, char* param);
extern bool SleepAidLampOnOffCheckBoxOnPress(ITUWidget* widget, char* param);
extern bool SleepAidMusicOnOffCheckBoxOnPress(ITUWidget* widget, char* param);
extern bool SleepAidMusicRadioBoxOnPress(ITUWidget* widget, char* param);
extern bool SleepAidLevelRadioBoxOnPress(ITUWidget* widget, char* param);
extern bool SleepAidStartCheckBoxOnPress(ITUWidget* widget, char* param);
extern bool SetMassageLayerOnEnter(ITUWidget* widget, char* param);
extern bool SetMassageTimeRadioBoxOnPress(ITUWidget* widget, char* param);
extern bool ScreensaverOnEnter(ITUWidget* widget, char* param);
extern bool SetSysLayerOnEnter(ITUWidget* widget, char* param);
extern bool SetSysBacklightTrackBarChanged(ITUWidget* widget, char* param);
extern bool SetSysKeySoundOnOffCheckBoxOnPress(ITUWidget* widget, char* param);

ITUActionFunction actionFunctions[] =
{
    "MainLayerOnEnter", MainLayerOnEnter,
    "MainLayerOnTimer", MainLayerOnTimer,
    "MainStopSnoreCheckBoxOnPress", MainStopSnoreCheckBoxOnPress,
    "MainLampCheckBoxOnPress", MainLampCheckBoxOnPress,
    "MainSleepAidCheckBoxOnPress", MainSleepAidCheckBoxOnPress,
    "MainMassageCheckBoxOnPress", MainMassageCheckBoxOnPress,
    "MainMattressModeRadioBoxOnPress", MainMattressModeRadioBoxOnPress,
    "MassageLayerOnEnter", MassageLayerOnEnter,
    "MassageLayerOnTimer", MassageLayerOnTimer,
    "MassageModeRadioBox", MassageModeRadioBox,
    "MassageLevelRadioBox", MassageLevelRadioBox,
    "MassageStartCheckBoxOnPress", MassageStartCheckBoxOnPress,
    "SleepAidLayerOnEnter", SleepAidLayerOnEnter,
    "SleepAidLampOnOffCheckBoxOnPress", SleepAidLampOnOffCheckBoxOnPress,
    "SleepAidMusicOnOffCheckBoxOnPress", SleepAidMusicOnOffCheckBoxOnPress,
    "SleepAidMusicRadioBoxOnPress", SleepAidMusicRadioBoxOnPress,
    "SleepAidLevelRadioBoxOnPress", SleepAidLevelRadioBoxOnPress,
    "SleepAidStartCheckBoxOnPress", SleepAidStartCheckBoxOnPress,
    "SetMassageLayerOnEnter", SetMassageLayerOnEnter,
    "SetMassageTimeRadioBoxOnPress", SetMassageTimeRadioBoxOnPress,
    "ScreensaverOnEnter", ScreensaverOnEnter,
    "SetSysLayerOnEnter", SetSysLayerOnEnter,
    "SetSysBacklightTrackBarChanged", SetSysBacklightTrackBarChanged,
    "SetSysKeySoundOnOffCheckBoxOnPress", SetSysKeySoundOnOffCheckBoxOnPress,
    NULL, NULL
};
