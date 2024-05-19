#include "inc/flash.h"

static struct flash_pagination pagination;

const struct device *flash_dev = TEST_PARTITION_DEVICE;


int init_flash() {
    if (!device_is_ready(flash_dev)) {
        printf("Flash device not ready\n");
        return -1;
    }

    if (flash_init(flash_dev) != 1) {
        printf("Flash init failed\n");
        return -1;
    }
    return 1;
}


struct flash_pagination get_pages() {
    return pagination;
}


uint8_t flash_init(const struct device *flash_dev) {
    for (int page_num = 0; page_num < FLASH_PAGE_COUNT; page_num++) {
        uint32_t offset = TEST_PARTITION_OFFSET + (page_num * FLASH_PAGE_SIZE);
        if (flash_erase(flash_dev, offset, FLASH_PAGE_SIZE) != 0) {
            printf("Flash erase failed at page %d!\n", page_num);
            continue;
        }
        printf("Flash erase succeeded at page %d!\n", page_num);
    }
    return 1;
}


uint8_t write_entry(void *buffer, uint8_t *page_num, uint8_t *line) {
    if (page_num != NULL && line != NULL) {
        pagination.last_page_num = *page_num;
        pagination.last_line = *line;
        return 1;
    }
    
    uint32_t write_offset = TEST_PARTITION_OFFSET + (pagination.last_page_num * FLASH_PAGE_SIZE) + (pagination.last_line * BUFFER_SIZE);
    if (flash_write(flash_dev, write_offset, buffer, BUFFER_SIZE) != 0) {
        printf("Flash write failed at offset 0x%X!\n", write_offset);
        return 0;
    }
    pagination.last_line++;
    if (pagination.last_line == FLASH_LINE_COUNT) {
        pagination.last_page_num++;
        pagination.last_line = 0;
        if (pagination.last_page_num == FLASH_PAGE_COUNT) {
            pagination.last_page_num = 0;
        }
    }
    return 1;
}


void read_entry(void *data, uint8_t page_num, uint8_t line) {
    __off_t offset = TEST_PARTITION_OFFSET + (page_num * FLASH_PAGE_SIZE) + ((line-1) * BUFFER_SIZE);
    if (flash_read(flash_dev, offset, data, BUFFER_SIZE) != 0) {
        printf("Failed to read flash at page %d!\n", 0);
        return 0;
    }
    return 1;
}


void read_entry_many(void *data, uint8_t page_num, uint8_t line, uint8_t num_lines) {
    __off_t offset = TEST_PARTITION_OFFSET + (page_num * FLASH_PAGE_SIZE) + ((line-1) * BUFFER_SIZE);
    if (flash_read(flash_dev, offset, data, num_lines) != 0) {
        printf("Failed to read flash at page %d!\n", 0);
        return 0;
    }
    return 1;
}
