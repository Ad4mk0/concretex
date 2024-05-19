#include "inc/power.h"


void power_on_nrf52() {
    NRF_POWER->TASKS_CONSTLAT = 1; // Enable constant latency mode
    NRF_POWER->TASKS_LOWPWR = 0;   // Exit low power mode
}

void power_off_nrf52() {
    NRF_POWER->TASKS_LOWPWR = 1; // Enter low power mode
    NRF_POWER->TASKS_CONSTLAT = 0; // Disable constant latency mode
}