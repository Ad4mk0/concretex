#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/uuid.h>
#include <string.h>
#include <stdint.h>


#define DEVICE_NAME CONFIG_BT_DEVICE_NAME
#define DEVICE_NAME_LEN (sizeof(DEVICE_NAME) - 1)

#define HISTORICAL_DATA_UUID BT_UUID_DECLARE_128(BT_UUID_128_ENCODE(0x00001524, 0x1212, 0xefde, 0x1524, 0x785feabcd124))
#define RANGE_UUID           BT_UUID_DECLARE_128(BT_UUID_128_ENCODE(0x00001525, 0x1212, 0xefde, 0x1525, 0x785feabcd125))
#define CURRENT_PAGE_UUID    BT_UUID_DECLARE_128(BT_UUID_128_ENCODE(0x00001525, 0x1212, 0xefde, 0x1525, 0x785feabcd126))
#define CURRENT_LINE_UUID    BT_UUID_DECLARE_128(BT_UUID_128_ENCODE(0x00001525, 0x1212, 0xefde, 0x1525, 0x785feabcd127))


struct ble_request {
    uint8_t page;
    uint8_t line;
    uint8_t num_lines;
};


static struct ble_request request = {.page=0, .line=0, .num_lines=1};


ssize_t write_range(struct bt_conn *conn, const struct bt_gatt_attr *attr, const void *buf, uint16_t len, uint16_t offset, uint8_t flags);


ssize_t read_historical_data(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, uint16_t len, uint16_t offset);


ssize_t read_line_data(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, uint16_t len, uint16_t offset);


ssize_t read_page_data(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, uint16_t len, uint16_t offset);


/*
initial ble setup
*/
uint8_t ble_init();


/*
starts ble transmission
*/
uint8_t ble_start();


static const struct bt_data ad[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, DEVICE_NAME_LEN),
};

static const struct bt_data sd[] = {
    // Here you can put additional data you want available in the scan response
    BT_DATA_BYTES(BT_DATA_UUID128_ALL, 0x25, 0x15, 0xde, 0xef, 0x12, 0x12, 0x00, 0x00, 0x24, 0x78, 0x5f, 0xea, 0xbc, 0xad, 0xd1, 0x24),
    // More entries can be added here
};

