#ifndef __UTILS_H__
#define __UTILS_H__

// atrocious, but i love it
#define p *(uint32_t*)
#define vp *(volatile uint32_t*)

// le typicals
#define dmb() asm volatile("dmb\n\t" ::: "memory")
#define dsb() asm volatile("dsb\n\t" ::: "memory")
#define wfe() asm volatile("wfe\n\t")
#define wfi() asm volatile("wfi\n\t")
#define sev() asm volatile("sev\n\t") // not a multicore system so ig doesnt apply?

#define BITF(n) (~(-1 << n))
#define BITN(n) (1 << (n))
#define BITNVAL(n, val) ((val) << (n))
#define BITNVALM(n, val, mask) (((val) & (mask)) << (n))

// funcs
void delay(int n);

// get compile timestamp
__attribute__((noinline)) uint32_t get_build_timestamp(void);

#endif