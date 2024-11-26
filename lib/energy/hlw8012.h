#pragma once
#ifdef USE_ENERGY_SENSOR
#ifdef USE_HLW8012

#define XNRG_01                 1

// Energy model type 0 (GPIO_HLW_CF) - HLW8012 based (Sonoff Pow, KMC70011, HuaFan, AplicWDP303075)
#define HLW_PREF            10000    // 1000.0W
#define HLW_UREF             2200    // 220.0V
#define HLW_IREF             4545    // 4.545A

// Energy model type 1 (GPIO_HJL_CF) - HJL-01/BL0937 based (BlitzWolf, Homecube, Gosund, Teckin)
#define HJL_PREF             1362
#define HJL_UREF              822
#define HJL_IREF             3300

#define HLW_POWER_PROBE_TIME   10    // Number of seconds to probe for power before deciding none used
#define HLW_SAMPLE_COUNT       10    // Max number of samples per cycle

struct HLW {
    volatile uint32_t cf_pulse_length = 0;
    volatile uint32_t cf_pulse_last_time = 0;
    volatile uint32_t cf_summed_pulse_length = 0;
    volatile uint32_t cf_pulse_counter = 0;
    uint32_t cf_power_pulse_length  = 0;

    volatile uint32_t cf1_pulse_length = 0;
    volatile uint32_t cf1_pulse_last_time = 0;
    volatile uint32_t cf1_summed_pulse_length = 0;
    volatile uint32_t cf1_pulse_counter = 0;
    uint32_t cf1_voltage_pulse_length  = 0;
    uint32_t cf1_current_pulse_length = 0;

    volatile uint32_t energy_period_counter = 0;

    uint32_t power_ratio = 0;
    uint32_t voltage_ratio = 0;
    uint32_t current_ratio = 0;

    uint8_t model_type = 0;
    volatile uint8_t cf1_timer = 0;
    uint8_t power_retry = 0;
    bool select_ui_flag = false;
    bool ui_flag = true;
    volatile bool load_off = true;
};

#ifndef USE_WS2812_DMA  // Collides with Neopixelbus but solves exception
void HlwCfInterrupt(void) IRAM_ATTR;
void HlwCf1Interrupt(void) IRAM_ATTR;
#endif  // USE_WS2812_DMA

void HlwCfInterrupt(void);
void HlwCf1Interrupt(void);
void HlwEvery200ms(void);
void HlwEverySecond(void);
void HlwSnsInit(void);
void HlwDrvInit(void);
bool HlwCommand(void);

bool Xnrg01(uint32_t function);

#endif  // USE_HLW8012
#endif  // USE_ENERGY_SENSOR
