#ifndef __COMMON_H
#define __COMMON_H

#define vi *( volatile unsigned int * )

#define set_zero(addr, bit) ( (vi addr) &= ( ~ ( 1 << (bit) ) ) )
#define set_one(addr, bit) ( (vi addr) |= ( 1 << ( bit ) ) )

#define set_bit(addr, bit, val) ( (vi addr) = (( vi addr)&=(~(1<<(bit))) ) | ( (val)<<(bit) ) )

#define set_2bit(addr, bit, val) ( (vi addr) = (( vi addr)&(~(3<<(bit))) ) | ( (val)<<(bit) ) )

#define set_nbit(addr, bit, len, val) \
    ( (vi addr) = ((( vi addr)&(~(( ((1<<(len))-1) )<<(bit))))  | ( (val)<<(bit) ) ))

#define get_bit(addr, bit) ( (( vi addr ) & ( 1 << (bit) )) > 0  )

#define get_val(addr, val) ( (val) = vi addr )
#define read_val(addr) ( vi ( addr ) )
#define set_val(addr, val) ( (vi addr) = (val) )
#define or_val(addr, val) ( (vi addr) |= (val) )

#define GPKCON0     (*((volatile unsigned long *)0x7F008800))
#define GPKDATA     (*((volatile unsigned long *)0x7F008808))
#define GPNCON      (*((volatile unsigned long *)0x7F008830))
#define GPNDAT      (*((volatile unsigned long *)0x7F008834))
#define EINT0CON0   (*((volatile unsigned long *)0x7F008900))
#define EINT0MASK   (*((volatile unsigned long *)0x7F008920))
#define EINT0PEND   (*((volatile unsigned long *)0x7F008924))
#define GPFCON      (*((volatile unsigned long *)0x7F0080A0))
#define GPFDAT      (*((volatile unsigned long *)0x7F0080A4))

///////////////////////////////

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

// function declare

int delay(int);

#endif /* __COMMON_H */
