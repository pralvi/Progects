#ifdef __cplusplus
extern "C" {
#endif
//------------------------------------------------------------------------------
#ifndef _FLASH_H_
#define _FLASH_H_
//------------------------------------------------------------------------------
#include <stdint.h>

#define STORAGE_PTR(ADDR, ...) (uint8_t*)(ADDR)
#define STORAGE_PLACE_AFTER_(STORAGE_ADDR, STORAGE_SIZE, SIZE) STORAGE_ADDR + MEM_PAGE_SIZE * STORAGE_SIZE, SIZE
#define STORE_DATA_(FIRST_PAGE, PAGE_COUNT, DATA) StoreData(STORAGE_PTR(FIRST_PAGE), PAGE_COUNT, &DATA, sizeof(DATA))
#define READ_DATA_(FIRST_PAGE, PAGE_COUNT, DATA) ReadData(STORAGE_PTR(FIRST_PAGE), PAGE_COUNT, &DATA, sizeof(DATA))

#define STORE_DATA(...) STORE_DATA_(__VA_ARGS__)
#define READ_DATA(...) READ_DATA_(__VA_ARGS__)
#define STORAGE_PLACE_AFTER(...) STORAGE_PLACE_AFTER_(__VA_ARGS__)

  
#ifndef MEM_PAGE_SIZE
  #define MEM_PAGE_SIZE  1024
#endif
  
uint16_t* CopyMemory(void* src, void* dst, uint32_t size);
uint16_t* StoreData(void* first_page, uint_fast16_t page_count, void* data, uint32_t size);
uint16_t* ReadData(void* first_page, uint_fast16_t page_count, void* data, uint32_t size);
void EnableReadOutProtection();

//-----------------------------------------------------------------------------
#endif /* _FLASH_H_ */
//-----------------------------------------------------------------------------
#ifdef __cplusplus
}
#endif /* _cplusplus*/
