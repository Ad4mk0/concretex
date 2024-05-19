#include <zephyr/kernel.h>
#include <zephyr/drivers/flash.h>
#include <zephyr/storage/flash_map.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <stdio.h>

#define TEST_PARTITION storage_partition

#define TEST_PARTITION_OFFSET FIXED_PARTITION_OFFSET(TEST_PARTITION)
#define TEST_PARTITION_DEVICE FIXED_PARTITION_DEVICE(TEST_PARTITION)

#define FLASH_PAGE_SIZE 4096
#define FLASH_LINE_COUNT 512
#define FLASH_PAGE_COUNT 22

// Define buffer and repeat count
#define BUFFER_SIZE 8
#define REPEAT_COUNT (FLASH_PAGE_SIZE / BUFFER_SIZE)


struct flash_pagination
{
    uint32_t last_page_num;
    uint32_t last_line;
};


/*
    Initializes flash memory.

    Returns:
        flash_pagination
            uint32_t last_page_num;
            uint32_t last_line;
*/
struct flash_pagination get_pages();


/*
initializes flash memory
*/
uint8_t flash_init(const struct device *flash_dev);


/*
    Initializes flash memory.

    Returns:
        int: Status code indicating success (0) or failure (-1).
*/
int init_flash();


/*
    Initializes flash memory.

    Returns:
        int: Status code indicating success (0) or failure (-1).
*/
uint8_t write_entry(void *buffer, uint8_t *page_num, uint8_t *line);


/*
    Reads an entry from flash memory and puts it into data.

    Args:
        data (void*): Pointer to the data buffer where the entry will be stored.
        page_num (uint8_t): Page number from which to read the entry.
        line (uint8_t): Line number within the page from which to read the entry.
*/
void read_entry(void *data, uint8_t page_num, uint8_t line);


/*
    Reads multiple entries from flash memory and puts them into data.

    Args:
        data (void*): Pointer to the data buffer where the entries will be stored.
        page_num (uint8_t): Page number from which to start reading.
        line (uint8_t): Line number within the page from which to start reading.
        num_lines (uint8_t): Number of lines (entries) to read.
*/
void read_entry_many(void *data, uint8_t page_num, uint8_t line, uint8_t num_lines);
