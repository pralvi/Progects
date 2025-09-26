#include "flash.h"
#include "stm32.h"

uint16_t* CopyMemory(void* src, void* dst, uint32_t size) {
  size = (size + 1) >> 1; //Aling data to word and div 2;

  uint16_t* srcW = (uint16_t*)src;
  volatile uint16_t* dstW = (uint16_t*)dst;

  while (size) {
    *dstW = *srcW;
    while ((FLASH->SR & FLASH_SR_BSY) != 0);
    if (*dstW++ != *srcW++) //Verify
      break;
    size--;
  }
  return (!size) ? (uint16_t*)dstW : 0;
}
 
static inline void LockFlash() {
  FLASH->CR |= FLASH_CR_LOCK;
}

#ifndef RDP_KEY
    #define RDP_KEY              ((uint16_t)0x00A5)
#endif
#ifndef FLASH_KEY1
    #define FLASH_KEY1           ((uint32_t)0x45670123)
#endif
#ifndef FLASH_KEY2
    #define FLASH_KEY2           ((uint32_t)0xCDEF89AB)
#endif

static inline void UnlockFlash() {
  FLASH->KEYR = FLASH_KEY1;
  FLASH->KEYR = FLASH_KEY2;
}

void EnableReadOutProtection() {
  if (!(FLASH->OBR & FLASH_OBR_RDPRT)) {
    UnlockFlash();
    
    //Unlock option bytes
    FLASH->OPTKEYR = FLASH_KEY1;
    FLASH->OPTKEYR = FLASH_KEY2;
    FLASH->CR |= FLASH_CR_OPTER;  //Start Option Byte Erase
    FLASH->CR |= FLASH_CR_STRT; 
    while ((FLASH->SR & FLASH_SR_BSY) != 0);     //Wait until done

    FLASH->CR &= ~FLASH_CR_OPTER; //Disable the OPTER Bit
    FLASH->CR |= FLASH_CR_OPTPG;  //Enable the Option Bytes Programming operation
    OB->RDP = 0x00;               //Enable Readout Protections
    //OB->RDP = RDP_KEY;            //Disable Readout Protection
    while ((FLASH->SR & FLASH_SR_BSY) != 0); //Wait until done
    FLASH->CR &= ~FLASH_CR_OPTPG; //Disable the Option Bytes Programming operation

    LockFlash();
  }
}

static void EraseMemory(void* first_page, uint_fast16_t page_count) {
  FLASH->CR |= FLASH_CR_PER; //Enable Page Erase mode
  for(uint_fast16_t page = page_count; page > 0; page--) {
	  FLASH->AR = (uint32_t)first_page; 
	  FLASH->CR |= FLASH_CR_STRT; //Start erase
	  while ((FLASH->SR & FLASH_SR_BSY) != 0); //Wait end of eraze
	  first_page = (char*)first_page + MEM_PAGE_SIZE;
  }
  FLASH->CR &= ~(FLASH_CR_PER | FLASH_CR_STRT); //Disable Page Erase mode
}

static uint16_t* WriteFlash(void* src, void* dst, uint32_t size) {
  FLASH->CR |= FLASH_CR_PG; //Enable Programm Flash mode
  uint16_t* res = CopyMemory(src, dst, size);
  FLASH->CR &= ~FLASH_CR_PG; //Disable Programm Flash mode
  return res;
}

#define DATA_START_TOKEN        0xEFBE
#define DATA_END_TOKEN          0xFECA
static uint16_t* WriteFlashWithTokens(void* src, void* dst, uint32_t size) {
  if ((uint32_t)dst & 1 || *(uint16_t*)dst != 0xFFFF) //Check align & emptiness
    return 0;
  
  volatile uint16_t* dstW = (uint16_t*)dst;

  FLASH->CR |= FLASH_CR_PG;                             //Enable Programm Flash mode
  *dstW = DATA_START_TOKEN;                             //Write start token
  while ((FLASH->SR & FLASH_SR_BSY) != 0);              //Whait for complete
  if (*dstW == DATA_START_TOKEN) {                      //Verify start token
    dstW = CopyMemory(src, (void*)(++dstW), size);      //Write Data
    if (dstW) {
      *dstW = DATA_END_TOKEN;                           //Write end token
      while ((FLASH->SR & FLASH_SR_BSY) != 0);          //Whait for complete
    }      
  }
  FLASH->CR &= ~FLASH_CR_PG;                            //Disable Programm Flash mode

  return (*dstW == DATA_END_TOKEN) ? (uint16_t*)dstW : 0;
}

static uint16_t* FindFirstEmptySlot(void* first_page, uint_fast16_t page_count, uint32_t size) {
  size = (size + 1 + 4) >> 1; //Add start and end tockens and aling data to word and div 2;
  uint16_t* start = first_page;
  void* end = (char*)first_page + page_count * MEM_PAGE_SIZE;
  while(start < end) {
    if (*start == 0xFFFF && (start + size) < end)
      return start;
    start += size;
  }
  return 0;
}

static uint16_t* StoreDataEx(void* first_page, uint_fast16_t page_count, void* data, uint32_t size, void* last_entry) {
  uint16_t* end_of_storage = (uint16_t*)((char*)first_page + page_count * MEM_PAGE_SIZE) - 1;
  uint16_t* slot = FindFirstEmptySlot(last_entry, page_count, size);
  UnlockFlash();
  if (!slot) {//All affordable space is used
    if (page_count > 1 && *end_of_storage == 0xFFFF) {//Mark erase memory operation
      uint16_t marker = DATA_END_TOKEN;
      WriteFlash(&marker, end_of_storage, 2);
    }
    
    EraseMemory(first_page, (page_count > 1) ? page_count - 1 : 1);     //Erase pages
    slot = WriteFlashWithTokens(data, first_page, size);                           //Write slot
    if (slot && page_count > 1) EraseMemory((char*)first_page + (page_count - 1) * MEM_PAGE_SIZE, 1); //Erase last one page
  } else
      slot = WriteFlashWithTokens(data, slot, size);
  
  //Check last overrun completed correctly
  if (slot && slot < (uint16_t*)((char*)end_of_storage - MEM_PAGE_SIZE) && *end_of_storage != 0xFFFF)
    EraseMemory((char*)first_page + (page_count - 1) * MEM_PAGE_SIZE, 1);
  
  LockFlash();
  return slot;
}

uint16_t* StoreData(void* first_page, uint_fast16_t page_count, void* data, uint32_t size) {
  return StoreDataEx(first_page, page_count, data, size, first_page);
}

static uint_fast8_t IsSlotValid(uint16_t const* slot, const uint32_t size) {
  return *slot == DATA_START_TOKEN && *(slot + size - 1) == DATA_END_TOKEN;
}

static uint16_t* FineLastValidSlot(void* first_page, uint16_t* slot, uint32_t size) {
  uint_fast8_t isValid;
  do {
    slot -= size;
    isValid = IsSlotValid(slot, size);
  } while(slot > first_page && !isValid);
  return (slot >= first_page && isValid) ? slot : 0;
}

uint16_t* ReadData(void* first_page, uint_fast16_t page_count, void* data, uint32_t size) {
  uint32_t sz = (size + 1 + 4) >> 1; //Add start and end tockens and aling data to word and div 2;
  uint16_t* slot = FindFirstEmptySlot(first_page, page_count, size);
  if (slot)
    slot = FineLastValidSlot(first_page, slot, sz);
  
  if (!slot) {
    uint32_t storage_size = page_count * MEM_PAGE_SIZE;
    slot = (uint16_t*)((char*)first_page + storage_size) - (storage_size >> 1) % sz; //Calulate last slot location
    slot = FineLastValidSlot(first_page, slot, sz);
  }
  
  return (slot) ? CopyMemory(slot + 1, data, size) ? slot : 0 : 0;
}
