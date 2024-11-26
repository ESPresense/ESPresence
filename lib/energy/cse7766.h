#pragma once
#ifdef USE_ENERGY_SENSOR
#ifdef USE_CSE7766

#define XNRG_02             2

#define CSE_MAX_INVALID_POWER       128        // Number of invalid power readings before deciding active power is zero

#define CSE_NOT_CALIBRATED          0xAA

#define CSE_PULSES_NOT_INITIALIZED  -1

#define CSE_PREF                    1000
#define CSE_UREF                    100

#define CSE_BUFFER_SIZE             25

struct CSE {
    volatile uint32_t cf_pulse_length = 0;
    volatile uint32_t cf_pulse_last_time = 0;
    volatile uint32_t cf_summed_pulse_length = 0;
    volatile uint32_t cf_pulse_counter = 0;
    uint32_t cf_power_pulse_length  = 0;

    int byte_counter = 0;
    uint8_t *rx_buffer = nullptr;
    uint8_t power_invalid = 0;
    bool received = false;
};

void CseEvery200ms(void);
void CseEverySecond(void);
void CseSnsInit(void);
void CseDrvInit(void);
bool CseCommand(void);

bool Xnrg02(uint32_t function);

#endif  // USE_CSE7766
#endif  // USE_ENERGY_SENSOR
