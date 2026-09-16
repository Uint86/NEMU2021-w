#ifndef __MONITOR_ELF_H__
#define __MONITOR_ELF_H__

#include "common.h"

void load_elf_tables(int argc, char *argv[]);

/*
 * 在 ELF 符号表中查找类型为 STT_OBJECT 的符号。
 * 找到时将符号地址写入 addr，并返回 true。
 */
bool find_object_symbol(const char *name, uint32_t *addr);

#endif