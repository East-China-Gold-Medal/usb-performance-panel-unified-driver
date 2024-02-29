/* @file cpu_amd.c

    AMD-Specific CPU monitor, based AMD Ryzen Master SDK.
    SPDX-License-Identifier: WTFPL

*/

// WHY DO AMD USE CPLUSPLUS????
// We have to re-define it in C-style.

#include <common.h>

// ----- AMD has the copyright of the following definition code  ----- 

typedef struct EffectiveFreqData {
	unsigned int uLength;			///< Length of the Array  
	double* dFreq;					///< Address of the Array of Effective Frequency in Mhz
	double* dState;					///< Address of the Array of C0 State Residency in %
}EffectiveFreqData;

typedef union OCMode {
	unsigned int uOCMode;
	struct {
		unsigned uAuto : 1;				///< Auto or Default Mode
		unsigned uManual : 1;			///< Manual Overclocking Mode
		unsigned uPBOMode : 1;			///< PBO Mode
		unsigned uAutoOverclocking : 1;	///< AutoOverclocking Mode
		unsigned uEcoMode : 2;
		unsigned Reserved : 26;
	};

} OCMode;

typedef struct CPUParameters {
	OCMode eMode;					///< OCMode Structure
	EffectiveFreqData stFreqData;	///< EffectiveFreqData Structure
	double dPeakCoreVoltage;		///< Peak Core Voltage
	double dPeakCoreVoltage_1;		///< Peak Core Voltage_1
	double dSocVoltage;				///< Current SOC Voltage
	double dTemperature;			///< Current Temperature(C)
	double dAvgCoreVoltage;			///< Average Core Voltage of that Sample period
	double dAvgCoreVoltage_1;		///< Average Core Voltage_1 of that Sample period
	double dPeakSpeed;				///< Peak Speed of all the Active cores
	float fPPTLimit;				///< PPT Current Limit(W)
	float fPPTValue;				///< PPT Current value(W)
	float fTDCLimit_VDD;			///< TDC(VDD) Current Limit(A)
	float fTDCValue_VDD;			///< TDC(VDD) Current Value(A)
	float fTDCValue_VDD_1;			///< TDC(VDD)_1 Current Value(A)
	float fEDCLimit_VDD;			///< EDC(VDD) Current limit(A)
	float fEDCValue_VDD;			///< EDC(VDD) Current Value(A)
	float fEDCValue_VDD_1;			///< EDC(VDD)_1 Current Value(A)
	float fcHTCLimit;				///< cHTC Current Limit(celsius)
	float fFCLKP0Freq;				///< FCLK P0 Frequency
	float fCCLK_Fmax;				///< CCLK Fmax frequency(MHz)
	float fTDCLimit_SOC;			///< TDC(SOC) limit in ampere(A)
	float fTDCValue_SOC;			///< TDC(SOC) current value  in ampere(A)
	float fEDCLimit_SOC;			///< EDC(SOC) limit  in ampere(A)
	float fEDCValue_SOC;			///< EDC(SOC) Avg value  in ampere(A)
	float fVDDCR_VDD_Power;			///< VDDCR_VDD Power in watts(W)
	float fVDDCR_SOC_Power;			///< VDDCR_SOC Power in watts(W)
} CPUParameters;

// ----- AMD has the copyright of the above definition code  ----- 

extern int GetCPUParameters(CPUParameters* stInfo); // Platform.lib
extern int IsSupportedProcessor();

uint8_t amd_cpu_found;

void data_source_amd_cpu_prepare(void)
{

}