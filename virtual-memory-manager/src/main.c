/**
 * @file main.c
 * @author Matheus Vila Real (matheus.pires.vila@uel.br)
 * @brief Simulador de gerenciador de memória virtual
 * @version 0.1
 * @date 2022-11-12
 * 
 * @copyright Copyright (c) 2022
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "memman.h"

typedef int address_t;

int main(int argc, char *argv[]) {
    if(argc < 4) {
        printf("Uso: ./%s address.txt <quadro> <algoritmo>",
            argv[0]);
        return 1;
    }

    FILE* addresses = fopen(argv[1], "r");
    if(addresses == NULL) {
        printf("Erro ao abrir arquivo de endereços.");
        return -1;
    }

    char* frame = argv[2];
    char* algorithm = argv[3];
    char line[10];
    
    address_t address;
    memory_manager memman;
    memory mem;
    tlb_entry e;

    unsigned long counter = 0;

    int page_faults = 0,
        tlb_hits = 0,
        tlb_misses = 0,
        page_num = 0,
        offset = 0,
        physical_page = 0,
        fp = 0;

    int remaining_frames = atoi(frame);

    memory_init(&mem, atoi(frame));
    memman_init(&memman);

    while(fgets(line, 10, addresses) != NULL && strcmp(line, "\n") != 0) {
        address = atoi(line);
        ++counter;

        if(address >= 0){
            page_num = get_page_num(address);
            offset = get_offset(address);
            physical_page = get_frame_from_tlb(&memman, page_num);

            if(physical_page != -1) {
                ++tlb_hits;             // TLB HIT!
            }
            else {
                ++tlb_misses;           // TLB MISS!
                physical_page = get_frame_from_pagetable(&memman, 
                    page_num);

                if(physical_page == -1) {
                    ++page_faults;      // PAGE FAULT!
                    if(strcmp(algorithm, "fifo") == 0)
                        physical_page = fp++;
                    else if(strcmp(algorithm, "lru") == 0) {
                        physical_page = memman.time[0];
                        for(int t = 0; t < memman.tlb_index; t++) {
                            if(memman.time[t] < physical_page) {
                                physical_page = memman.time[t];
                                printf("%d\n", physical_page);
                            }
                        }
                    }

                    /* Copia página de backing store para memória */
                    fseek(mem.backing_store, page_num * PAGE_SIZE, 
                        SEEK_SET);
                    fread(&mem.main[physical_page * PAGE_SIZE],
                        PAGE_SIZE, sizeof(byte), mem.backing_store);

                    if(remaining_frames > 0) {
                        memman.page_table[page_num].frame_num = physical_page;
                        memman.page_table[page_num].page_num = page_num;
                        --remaining_frames;
                    }
                    else {
                        memman.page_table[page_num].frame_num = 
                            memman.page_table[physical_page].frame_num;
                        memman.page_table[physical_page].frame_num = -1;
                        memman.page_table[physical_page].page_num = -1;
                        remaining_frames = atoi(frame);
                    }
                }

                if(strcmp(algorithm, "fifo") == 0) {
                    add_to_tlb_fifo(&memman, page_num, physical_page);
                }
                else if(strcmp(algorithm, "lru") == 0) {
                    add_to_tlb_lru(&memman, page_num, physical_page);
                }
            }

            printf("Virtual address: %d Physical address: %d Value: %d\n",
                address, (physical_page << 8) | offset, mem.main[physical_page * PAGE_SIZE + offset]);
        }
        else {
            switch(address) {
                case -1:
                    display_tlb(&memman);
                    break;
                case -2:
                    display_invalid_pages(&memman);
                    break;
                case -3:
                    display_valid_pages(&memman);
                    break;
                default:
                    break;
            }
        }
    }

    printf("Falhas de página: %d\n", page_faults);
    printf("TLB hits: %d\n", tlb_hits);
    printf("TLB misses: %d\n", tlb_misses);
    printf("Taxa de acerto da TLB: %.2f%%\n", 
        (float) tlb_hits / (tlb_hits + tlb_misses) * 100);
    
    fclose(mem.backing_store);
    fclose(addresses);

    return 0;
}