/**
 * @file memman.c 
 * @author Matheus Vila Real (matheus.pires.vila@uel.br)
 * @brief Implementação do gerenciador virtual de memória
 * @version 0.1
 * @date 2022-11-12
 * 
 * @copyright Copyright (c) 2022
 */

#include <stdio.h>
#include <string.h>

#include "memman.h"

#define BACKING_STORE "BACKING_STORE.bin"

static int age = 1;

static int max(int a, int b) {
    return a > b ? a : b;
}

int get_page_num(int address) {
    return (address >> 8) & 0xFF;
}

int get_offset(int address) {
    return address & 0xFF;
}

int get_frame_from_pagetable(memory_manager* mm, int page_num) {
    for(int i = 0; i < PAGES; i++) {
        if(mm->page_table[i].page_num == page_num) {
            mm->time[i] = age++;
            return mm->page_table[i].frame_num;
        }
    }
    return -1;
}

int get_frame_from_tlb(memory_manager* mm, int page_num) {
    for(int i = 0; i < mm->tlb_index; i++) {
        if(mm->tlb[i].page_num == page_num) {
            return mm->tlb[i].frame_num;
        }
    }

    return -1;
}

void add_to_tlb_fifo(memory_manager* mm, int page_num, int frame_num) {
    int index;
    for(index = 0; index < mm->tlb_index; index++) {
        if(mm->tlb[index].page_num == page_num){               
            for(int j = index; j < mm->tlb_index - 1; j++)
                mm->tlb[j] = mm->tlb[j + 1];
            break;
        }
    }
    if(index == mm->tlb_index) {
        for(int j = 0; j < index; j++)
            mm->tlb[j] = mm->tlb[j + 1];

    }
    mm->tlb[index].page_num = page_num;
    mm->tlb[index].frame_num = frame_num;

    if(mm->tlb_index < TLB_SIZE - 1) {
        mm->tlb_index++;
    }  
}

void add_to_tlb_lru(memory_manager* mm, int page_num, int frame_num) {
    int index = 0, 
        smallest = mm->time[0];
    for(int t = 0; t < mm->tlb_index; t++) {
        if(mm->time[t] < smallest) {
            smallest = mm->time[t];
            index = t;
        }
    }
    for(int j = index; j < mm->tlb_index - 1; j++)
                mm->tlb[j] = mm->tlb[j + 1];
    if(index == mm->tlb_index) {
        for(int j = 0; j < index; j++)
            mm->tlb[j] = mm->tlb[j + 1];

    }
    mm->tlb[index].page_num = page_num;
    mm->tlb[index].frame_num = frame_num;

    if(mm->tlb_index < TLB_SIZE - 1) {
        mm->tlb_index++;
    }  
}

void memman_init(memory_manager* memman) {
    memman->tlb_index = 0;
    memset(memman->page_table, -1, PAGES * sizeof(tlb_entry));
    memset(memman->tlb, 0, TLB_SIZE * sizeof(tlb_entry));
    memset(memman->time, 0, PAGES * sizeof(int));
}

void memory_init(memory* mem, int mem_size) {
    mem->backing_store = fopen(BACKING_STORE, "r");
    memset(mem->main, 0, mem_size * sizeof(byte));
}

void display_tlb(memory_manager* mm) {
    puts("+---------------+----------------+");
    printf("TLB:\n");
    puts("+---------------+----------------+");
    for(int i = 0; i < TLB_SIZE; i++) {
        printf("Página: %d\t|\tFrame: %d\n", 
            mm->tlb[i].page_num, mm->tlb[i].frame_num);
    }
}

void display_valid_pages(memory_manager* mm) {
    puts("+---------------+");
    printf("Páginas válidas:\n");
    puts("+---------------+");
    for(int i = 0; i < PAGES; i++) {
        if(mm->page_table[i].frame_num != -1) {
            printf("Página: %d\n", i);
        }
    }
}

void display_invalid_pages(memory_manager* mm) {
    puts("+---------------+");
    printf("Páginas inválidas:\n");
    puts("+---------------+");
    for(int i = 0; i < PAGES; i++) {
        if(mm->page_table[i].frame_num == -1) {
            printf("Página: %d\n", i);
        }
    }
}