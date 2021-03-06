


#define ADC2_PD3_CHN        4
#define ADC3_PD6_CHN        6
#define ADC_PD2_CHN         3//AIN3
#define ADC_PC4_CHN         2//AIN2
#define ADC_PD3_CHN        4
#define ADC_PD5_CHN        5
#define ADC_PD6_CHN        6

#define	ACC_X_DIR		ADC_PD3_CHN
#define	ACC_Y_DIR		ADC_PD2_CHN
#define	ACC_Z_DIR		ADC_PD5_CHN

//sample vol system from adc
#define		VOLTAGE_SYSTEM_ADC_PORT				ADC_PD5_CHN
#define		VOLTAGE_SYSTEM_VERIFY_ADC_PORT		ADC_PD3_CHN
#define		CURRENT_VERIFY_ADC_PORT				ADC_PD6_CHN
#define		TEMPERATURE_ADC_PORT				ADC_PD2_CHN

#define		OFF	0
#define		ON	1

enum BLINK_MODE
{
    BLINK_NONE,
	BLINK_STOP,
    BLINK_NORMAL,
    BLINK_SLOW,
    BLINK_FAST,
    BLINK_FAST_4,
    BLINK_FAST_5,
    BLINK_OFF,
};


#define	VOLTAGE_OLD		0
#define	VOLTAGE_NEW		1

#define VOLTAGE_SYSTEM_12V_TYPE     12
#define VOLTAGE_SYSTEM_24V_TYPE     24 
#define VOLTAGE_SYSTEM_UNKNOWN_TYPE 0

#define	VOLTAGE_SYSTEM_TYPE		VOLTAGE_SYSTEM_12V_TYPE//VOLTAGE_SYSTEM_24V_TYPE//VOLTAGE_SYSTEM_12V_TYPE


#define		VERIFY_BY_MANUAL_TYPE		1 // 校正电流值


#define	VOLTAGE_ADC_PD6_6A_VAL		2680
#define	VOLTAGE_ADC_PD6_12A_VAL		2920
#define	VOLTAGE_ADC_PD6_15A_VAL		3040
#define	VOLTAGE_ADC_PD6_18A_VAL		3160


//==========================================以下为12V系统=============
//系统电压检测ADC最大值和最小值范围
#define VOLTAGE_SYSTEM_12V_MIN    2300//1180//1170//900//1500 //MV
#define VOLTAGE_SYSTEM_12V_MAX    2650//1540//1640//2700 //MV

#define VOLTAGE_SYSTEM_12V_POWERON_MIN    VOLTAGE_SYSTEM_12V_MIN//1200//1960 //MV
#define VOLTAGE_SYSTEM_12V_POWERON_MAX    VOLTAGE_SYSTEM_12V_MAX+10 //MV


#define	VOLTAGE_SYSTEM_12V_POWEROFF_MIN		(VOLTAGE_SYSTEM_12V_MIN-10)
#define	VOLTAGE_SYSTEM_12V_POWEROFF_MAX		(VOLTAGE_SYSTEM_12V_MAX+10)//1650

#define		VOLTAGE_ADC_PD3_6A_MIN		VOLTAGE_SYSTEM_12V_POWERON_MIN//1160
#define		VOLTAGE_ADC_PD3_6A_MAX		2350//1250//1210

#define		VOLTAGE_ADC_PD3_12A_TO_6A_VALUE		2320

#define		VOLTAGE_ADC_PD3_12A_MIN		(VOLTAGE_ADC_PD3_6A_MAX+10)
#define		VOLTAGE_ADC_PD3_12A_MAX		2650//1540

#define	VOLTAGE_ADC_PD2_MIN		800
#define	VOLTAGE_ADC_PD2_MAX		4500

#define	VOLTAGE_ADC_PD2_HEATER_MIN		800
#define	VOLTAGE_ADC_PD2_HEATER_MAX		4500


#define		VOLTAGE_ADC_PC4_MIN		3000//2000
#define		VOLTAGE_ADC_PC4_MAX		3360//2400

// 12V system voltage
#define		VOLTAGE_ADC_PD5_6A_POWEROFF_VAL		550//500
#define		VOLTAGE_ADC_PD5_NOT6A_POWEROFF_VAL		1000//1000

#if VERIFY_BY_MANUAL_TYPE == 1
#undef CURRENT_VOL_STANDARD_VAL
#define	CURRENT_VOL_STANDARD_VAL	2760//800//760//2680
#define	VOLTAGE_ADC_PD6_PA1PA3_HIGH_VAL		2640//2590//700//670//2590

#undef	VOLTAGE_ADC_PD6_6A_VAL
#define	VOLTAGE_ADC_PD6_6A_VAL		CURRENT_VOL_STANDARD_VAL//760

#undef	VOLTAGE_ADC_PD6_12A_VAL
#define	VOLTAGE_ADC_PD6_12A_VAL		3040//1030//1000

#undef	VOLTAGE_ADC_PD6_15A_VAL
#define	VOLTAGE_ADC_PD6_15A_VAL		3240//1155//1120

#undef	VOLTAGE_ADC_PD6_18A_VAL
#define	VOLTAGE_ADC_PD6_18A_VAL		3440//1275//1240


#elif VERIFY_BY_MANUAL_TYPE == 2
#define	CURRENT_VOL_STANDARD_VAL	2680
#define	VOLTAGE_ADC_PD6_PA1PA3_HIGH_VAL		2590


#else
#define	CURRENT_VOL_STANDARD_VAL	2680
#define	VOLTAGE_ADC_PD6_PA1PA3_HIGH_VAL		2590


#endif




u16 current_to_voltage_table[] = {0,VOLTAGE_ADC_PD6_6A_VAL,VOLTAGE_ADC_PD6_12A_VAL,VOLTAGE_ADC_PD6_15A_VAL,VOLTAGE_ADC_PD6_18A_VAL};


