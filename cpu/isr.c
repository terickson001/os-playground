#include "isr.h"
#include "idt.h"
#include "port.h"
#include "timer.h"

#include "../drivers/screen.h"
#include "../drivers/keyboard.h"

#include "../libc/string.h"

#define PIC1     0x20     // Base address of the master PIC
#define PIC1_CMD PIC1
#define PIC1_DAT (PIC1+1)

#define PIC2     0xA0     // Base address of the slave PIC
#define PIC2_CMD PIC2
#define PIC2_DAT (PIC2+1)

#define ICW1_ICW4       0x01   // ICW4 (not) needed
#define ICW1_SINGLE     0x02   // Single (cascade) mode
#define ICW1_INTERVAL4  0x04   // Call address interval 4 (8)
#define ICW1_LEVEL      0x08   // Level triggered (edge) mode
#define ICW1_INIT       0x10   // Initialization - required!

#define ICW4_8086       0x01   // 8086/88 (MCS-80/85) mode
#define ICW4_AUTO       0x02   // Auto (normal) EOI
#define ICW4_BUF_SLAVE  0x08   // Buffered mode/slave
#define ICW4_BUF_MASTER 0x0C   // Buffered mode/master
#define ICW4_SFNM       0x10   // Soecual full nested (not)

ISR *interrupt_handlers[256];


void remap_PIC(int master_offset, int slave_offset)
{
    unsigned char a1, a2;
    a1 = port_byte_in(PIC1_DAT); // Store masks
    a2 = port_byte_in(PIC2_DAT);
    
    // Remap the PIC
    port_byte_out(PIC1_CMD, ICW1_INIT | ICW1_ICW4); // Starts init process (in cascade mode)
    port_byte_out(PIC2_CMD, ICW1_INIT | ICW1_ICW4);
    
    port_byte_out(PIC1_DAT, master_offset);         // ICW2: Master PIC offset
    port_byte_out(PIC2_DAT, slave_offset);          // ICW2: Slave PIC offset
    
    port_byte_out(PIC1_DAT, 0x04);                  // Tell Master PIC there is a slave at IRQ2 (0000 0100)
    port_byte_out(PIC2_DAT, 0x02);                  // Tell Slave PIC its cascade identity (0000 0010)
    
    port_byte_out(PIC1_DAT, ICW4_8086);
    port_byte_out(PIC2_DAT, ICW4_8086);
    
    port_byte_out(PIC1_DAT, 0x0);
    port_byte_out(PIC2_DAT, 0x0);

    port_byte_out(PIC1_DAT, a1); // Restore masks
    port_byte_out(PIC2_DAT, a2);
}

/* Can't do this with a loop because we need the address
 * of the function names */
void isr_install() {
    set_idt_gate(0, (u32)isr0);
    set_idt_gate(1, (u32)isr1);
    set_idt_gate(2, (u32)isr2);
    set_idt_gate(3, (u32)isr3);
    set_idt_gate(4, (u32)isr4);
    set_idt_gate(5, (u32)isr5);
    set_idt_gate(6, (u32)isr6);
    set_idt_gate(7, (u32)isr7);
    set_idt_gate(8, (u32)isr8);
    set_idt_gate(9, (u32)isr9);
    set_idt_gate(10, (u32)isr10);
    set_idt_gate(11, (u32)isr11);
    set_idt_gate(12, (u32)isr12);
    set_idt_gate(13, (u32)isr13);
    set_idt_gate(14, (u32)isr14);
    set_idt_gate(15, (u32)isr15);
    set_idt_gate(16, (u32)isr16);
    set_idt_gate(17, (u32)isr17);
    set_idt_gate(18, (u32)isr18);
    set_idt_gate(19, (u32)isr19);
    set_idt_gate(20, (u32)isr20);
    set_idt_gate(21, (u32)isr21);
    set_idt_gate(22, (u32)isr22);
    set_idt_gate(23, (u32)isr23);
    set_idt_gate(24, (u32)isr24);
    set_idt_gate(25, (u32)isr25);
    set_idt_gate(26, (u32)isr26);
    set_idt_gate(27, (u32)isr27);
    set_idt_gate(28, (u32)isr28);
    set_idt_gate(29, (u32)isr29);
    set_idt_gate(30, (u32)isr30);
    set_idt_gate(31, (u32)isr31);

    // Remap the PIC
    remap_PIC(0x20, 0x28);

    // Install IRQs
    set_idt_gate(32, (u32)irq0);
    set_idt_gate(33, (u32)irq1);
    set_idt_gate(34, (u32)irq2);
    set_idt_gate(35, (u32)irq3);
    set_idt_gate(36, (u32)irq4);
    set_idt_gate(37, (u32)irq5);
    set_idt_gate(38, (u32)irq6);
    set_idt_gate(39, (u32)irq7);
    set_idt_gate(40, (u32)irq8);
    set_idt_gate(41, (u32)irq9);
    set_idt_gate(42, (u32)irq10);
    set_idt_gate(43, (u32)irq11);
    set_idt_gate(44, (u32)irq12);
    set_idt_gate(45, (u32)irq13);
    set_idt_gate(46, (u32)irq14);
    set_idt_gate(47, (u32)irq15);
    
    set_idt(); // Load with ASM
}

/* To print the message which defines every exception */
char *exception_messages[] = {
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Into Detected Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",

    "Double Fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment Not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Unknown Interrupt",

    "Coprocessor Fault",
    "Alignment Check",
    "Machine Check",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",

    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
};

void isr_handler(Registers r)
{
    kprint("received interrupt: ");
    char s[3];
    int_to_ascii(r.int_no, s);
    kprint(s);
    kprint("\n");
    kprint(exception_messages[r.int_no]);
    kprint("\n");
}

void register_interrupt_handler(u8 n, ISR *handler)
{
    interrupt_handlers[n] = handler;
}

void irq_handler(Registers *r)
{
    /* After every interrupt we need to send an EOIO to the PICs
     * or they will not send another interrupt again */
    if (r->int_no >= 0x28) port_byte_out(PIC2_CMD, 0x20); // Slave
    port_byte_out(PIC1_CMD, 0x20); // Master

    // Handle interrupt in a more modular way
    if (interrupt_handlers[r->int_no])
    {
        ISR *handler = interrupt_handlers[r->int_no];
        handler(r);
    }
}

void iqr_install()
{
    // Enable interrupts
    asm volatile("sti");
    
    // IRQ 0
    init_timer(50);
    // IRQ 1
    init_keyboard();
}
