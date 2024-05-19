#include "inc/ble.h"
#include "inc/flash.h"
#include "inc/sensors.h"
#include "inc/power.h"

int main(void) {

    init_flash();
    init_sensors();
    ble_init();

    ble_start();

    printk("Bluetooth initialized and advertising\n");

    uint8_t nul = 0;
    uint8_t buffer[BUFFER_SIZE] = {0};

    // this is just initialization
    write_entry(buffer, &nul, &nul);

    while (1) {
      struct measurements measure;

      // power_on_nrf52();
      measure = measurement_cycle();

      serialize_measurements(measure, buffer);
      write_entry(buffer, NULL, NULL);


      // power_off_nrf52();
      printk("bye  temperature is: %.1f  and voltage is:  %d mV  \n", measure.temperature, measure.mV);
      k_sleep(K_MINUTES(1));

    }
}
