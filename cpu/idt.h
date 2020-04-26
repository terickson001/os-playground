#ifndef _CPU_IDT_H_
#define _CPU_IDT_H_

#include <cpu/types.h>

/* Segment Selectors */
#define KERNEL_CS 0x08

typedef struct IDT_Gate
{
    u16 low_offset;  /* Lower 16 bits of handler function address */
    u16 sel;         /* Kernel segment selector */
    u8 always0;
    /* First byte
       Bit  7  : "Interrupt is present"
       Bits 6-5: Privilege level of call (0=kernel..3=user)
       Bit  4  : Set to 0 for interrupt gates
       Bits 3-0: bits 1110 = decimal 14 = "32 bi interrupt gate" */
    u8 flags;
    u16 high_offset; /* Higher 16 bits of handler function address */
} __attribute((packed)) IDT_Gate;

/* A pointer to the array of interrupt handlers.
 * Assembly instruction 'lidt; will read it */
typedef struct IDT_Register
{
    u16 limit;
    u32 base;
} __attribute__((packed)) IDT_Register;

#define IDT_ENTRIES 256
IDT_Gate idt[IDT_ENTRIES];
IDT_Register idt_reg;

void set_idt_gate(int n, u32 handler);
void set_idt();

#endif
