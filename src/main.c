
// #include <zephyr/sys/printk.h>
/* kokoos */
#include <zephyr/device.h>
#include <zephyr/drivers/pwm.h>

#define PWM_NODE DT_ALIAS(&pwm0)

int main(void)
{
    const struct device *pwm_dev;
    const struct pwm_dt_spec pwm_spec = PWM_DT_SPEC_GET(PWM_NODE);

    pwm_dev = device_get_binding(pwm_spec.dev_name);
    if (!pwm_dev) {
        printk("Cannot find PWM device %s\n", pwm_spec.dev_name);
        return;
    }

    int err = pwm_pin_set_usec(pwm_dev, pwm_spec.channel, 100, 50, 0); // 10 kHz frequency, 50% duty cycle
    if (err) {
        printk("Error setting PWM parameters: %d\n", err);
        return 1;
    }

    printk("PWM configured successfully\n");

    while (1) {
        // Your main application code here
    }
	// return 0;
}
