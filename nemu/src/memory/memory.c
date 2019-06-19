#include "nemu.h"
#include "device/mmio.h"
#include "memory/mmu.h"

#define PMEM_SIZE (128 * 1024 * 1024)

#define pmem_rw(addr, type) *(type *)({\
    Assert(addr < PMEM_SIZE, "physical address(0x%08x) is out of bound", addr); \
    guest_to_host(addr); \
    })

uint8_t pmem[PMEM_SIZE];

/* Memory accessing interfaces */

uint32_t paddr_read(paddr_t addr, int len) {

  int mmio_id = is_mmio(addr);
  if (mmio_id != -1) {
	  return mmio_read(addr, len, mmio_id);
  }
  else return pmem_rw(addr, uint32_t) & (~0u >> ((4 - len) << 3));
}

void paddr_write(paddr_t addr, int len, uint32_t data) {
  int mmio_id = is_mmio(addr);
  if (mmio_id != -1) {
	  mmio_write(addr, len, data, mmio_id);
  }
  else memcpy(guest_to_host(addr), &data, len);
}

#define cross_page(addr, len) \
	((((addr) + (len) - 1) & ~PAGE_MASK) != ((addr) & ~PAGE_MASK))

paddr_t page_translate(vaddr_t addr, bool is_write) {
	if (cpu.cr0.paging == 0) {
		return addr;
	}

	uint32_t pd;//page directory base
	uint32_t dir=(((uint32_t)(addr)>>22)&0x3ff);//pd index
	uint32_t pnode=(((uint32_t)(addr)>>12)&0x3ff);//pt index
	PDE pde;
	PTE pte;
	pd = cpu.cr3.page_directory_base << 12;
	pde.val = paddr_read(pd + (dir << 2), 4);
	assert(pde.present);

	pte.val = paddr_read((pde.page_frame << 12) + (pnode << 2), 4);
	assert(pte.present);
	uint32_t address_page=(pte.page_frame << 12) + ((uint32_t)(addr) & PAGE_MASK);
	return address_page;

	/*Log("vaddr: %x", addr);
	PDE pde, *pd;//page directory entry
	PTE pte, *pt;//page table entry

	//uint32_t pd;//page directory base
	//uint32_t pt;//page table base
	paddr_t paddr;
	
	pd = (PDE *)(intptr_t)(cpu.cr3.page_directory_base << 12);
	Log("base: %x", cpu.cr3.page_directory_base);
	pde.val = paddr_read((intptr_t)&pd[((addr >> 22) & 0x3ff) << 2], 4);
	Log("addr: %x", (intptr_t)&pd[(addr >> 22) & 0x3ff]);
	//pd = cpu.cr3.page_directory_base << 12;
	//pde.val = paddr_read((pd + ((addr >> 22) & 0x3ff)), 4);
	Log("pde: %x\tpresent: %d", pde.val, pde.present);
	assert(pde.present);
	pde.accessed = 1;
	//paddr_write((intptr_t)&pd[(addr >> 22) & 0x3ff], 4, pte.val);
	//paddr_write((pd + ((addr >> 22) & 0x3ff)), 4, pde.val);

	pt = (PTE *)(intptr_t)(pde.page_frame << 12);
	pte.val = paddr_read((intptr_t)&pt[((addr >> 12) & 0x3ff) << 2], 4);
	//pt = pde.page_frame << 12;
	//pte.val = paddr_read((pt + ((addr >> 12) & 0x3ff)), 4);
	Log("pte: %x\tpresent: %d", pte.val, pte.present);
	assert(pte.present);
	pte.accessed = 1;
	pte.dirty = is_write ? 1 : pte.dirty;
	//paddr_write((intptr_t)&pt[(addr >> 12) & 0x3ff], 4, pte.val);
	//paddr_write((pt + ((addr >> 12) & 0x3ff)), 4, pte.val);

	paddr = (pte.page_frame << 12) | (addr & PAGE_MASK);
	return paddr;*/
}

uint32_t vaddr_read(vaddr_t addr, int len) {
  if (cpu.cr0.paging) {
	  if (cross_page(addr, len)) {
		  //assert(0);
		  paddr_t paddr;
		  union {
			  uint8_t byte[4];
			  uint32_t dword;
		  } data = {0};
		  for (int i = 0; i < len; i++) {
			  paddr = page_translate(addr + i, false);
			  data.byte[i] = (uint8_t)paddr_read(paddr, 1);
		  }
		  return data.dword;
	  }
	  else {
		  paddr_t paddr = page_translate(addr, false);
		  return paddr_read(paddr, len);
	  }
  }
  else 
	  return paddr_read(addr, len);
}

void vaddr_write(vaddr_t addr, int len, uint32_t data) {
  if (cpu.cr0.paging) {
	  if (cross_page(addr, len)) {
		  //assert(0);
		  paddr_t paddr;
		  for (int i = 0; i < len; i++) {
			  paddr = page_translate(addr, true);
			  paddr_write(paddr, 1, data);
			  data >>= 8;
			  addr++;
		  }
	  }
	  else {
		  paddr_t paddr = page_translate(addr, true);
		  return paddr_write(paddr, len, data);
	  }
  }
  else
	  paddr_write(addr, len, data);
}
