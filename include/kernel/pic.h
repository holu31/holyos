#ifndef _PIC_H
#define _PIC_H

#include <stdint.h>

void pic_remap();
void pic_mask_irq(uint8_t irq);
void pic_unmask_irq(uint8_t irq);
void pic_send_eoi(uint8_t irq);

#endif // _PIC_H
