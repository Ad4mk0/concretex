#include "inc/ble.h"
#include "inc/flash.h"


static struct ble_request request;
static struct bt_conn *my_conn = NULL;


// GATT Attribute Table
BT_GATT_SERVICE_DEFINE(historical_svc,
    BT_GATT_PRIMARY_SERVICE(HISTORICAL_DATA_UUID),
    BT_GATT_CHARACTERISTIC(HISTORICAL_DATA_UUID, BT_GATT_CHRC_READ, BT_GATT_PERM_READ, read_historical_data, NULL, NULL),
    BT_GATT_CHARACTERISTIC(RANGE_UUID, BT_GATT_CHRC_WRITE, BT_GATT_PERM_WRITE, NULL, write_range, &request),
    BT_GATT_CHARACTERISTIC(CURRENT_PAGE_UUID, BT_GATT_CHRC_READ, BT_GATT_PERM_READ, read_page_data, NULL, NULL),
    BT_GATT_CHARACTERISTIC(CURRENT_LINE_UUID, BT_GATT_CHRC_READ, BT_GATT_PERM_READ, read_line_data, NULL, NULL),
);


ssize_t read_page_data(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, uint16_t len, uint16_t offset) {
    struct flash_pagination x = get_pages();
    uint16_t page = x.last_page_num;
    return bt_gatt_attr_read(conn, attr, buf, len, offset, &page, sizeof(page));
}


ssize_t read_line_data(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, uint16_t len, uint16_t offset) {
    struct flash_pagination x = get_pages();
    uint16_t line = x.last_line;
    return bt_gatt_attr_read(conn, attr, buf, len, offset, &line, sizeof(line));
}


ssize_t write_range(struct bt_conn *conn, const struct bt_gatt_attr *attr, const void *buf, uint16_t len, uint16_t offset, uint8_t flags) {
    if (offset + len > sizeof(request)) {
        return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
    }
    memcpy((uint8_t*)&request + offset, buf, len);

    return len;
}


ssize_t read_historical_data(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, uint16_t len, uint16_t offset) {

    uint8_t *historical_data = k_malloc(request.num_lines*8);
    if (historical_data == NULL) {
        return BT_GATT_ERR(BT_ATT_ERR_INSUFFICIENT_RESOURCES);
    }

    read_entry_many(historical_data, request.page, request.line, request.num_lines*8);

    ssize_t read_len =  bt_gatt_attr_read(conn, attr, buf, len, offset, historical_data, request.num_lines * 8 * sizeof(uint8_t));

    k_free(historical_data);

    return read_len;

}


void on_connected(struct bt_conn *conn, uint8_t err) {
    if (err) {
        printk("Connection error %d\n", err);
        return;
    }
    printk("Connected\n");
    my_conn = bt_conn_ref(conn);
}


void on_disconnected(struct bt_conn *conn, uint8_t reason) {
    printk("Disconnected. Reason %d\n", reason);
    bt_conn_unref(my_conn);
    my_conn = NULL;
}


static struct bt_conn_cb connection_callbacks = {
    .connected = on_connected,
    .disconnected = on_disconnected
};


uint8_t ble_init() {
    int err;
    bt_conn_cb_register(&connection_callbacks);

    err = bt_enable(NULL);

    if (err) {
        printk("Bluetooth init failed (err %d)\n", err);
        return -1;
    }
    return 0;
}


uint8_t ble_start() {
    int err;
    err = bt_le_adv_start(BT_LE_ADV_CONN, ad, ARRAY_SIZE(ad), sd, ARRAY_SIZE(sd));
    if (err) {
        printk("Advertising start failed (err %d)\n", err);
        return -1;
    }
    return 0;
}

