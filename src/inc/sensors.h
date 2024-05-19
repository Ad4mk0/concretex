#include <zephyr/drivers/pwm.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/device.h>

// SQUAREWAVE INPUT
static const struct pwm_dt_spec pwm_led0 = PWM_DT_SPEC_GET(DT_ALIAS(pwm_led0));

// ADC READOUT
// Calculate period for 50kHz frequency
#define PWM_FREQ_HZ 50000
#define PWM_PERIOD_NS (NSEC_PER_SEC / PWM_FREQ_HZ)
static const struct adc_dt_spec adc_channel = ADC_DT_SPEC_GET(DT_PATH(zephyr_user));

// i2c TEMPERATURE
#define I2C_SLAVE_ADDR 0x44
#define I2C0_NODE DT_NODELABEL(mysensor)
static const struct i2c_dt_spec dev_i2c = I2C_DT_SPEC_GET(I2C0_NODE);


struct measurements {
    float temperature;
    int mV;
};


/*
    Performs a measurement cycle and returns the measurements.

    Returns:
        struct measurements: Struct containing the measurements obtained.
*/
struct measurements measurement_cycle();


/*
    Initializes sensors.

    Returns:
        int: Status code indicating success (0) or failure (-1).
*/
int init_sensors();


/*
    Serializes measurements data into a buffer.

    Args:
        data (struct measurements): Measurements data to be serialized.
        buffer (uint8_t*): Pointer to the buffer where serialized data will be stored.
*/
void serialize_measurements(struct measurements data, uint8_t *buffer);
