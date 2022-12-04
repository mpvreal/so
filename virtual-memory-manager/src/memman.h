/**
 * @file memman.h 
 * @author Matheus Vila Real (matheus.pires.vila@uel.br)
 * @brief Arquivo de cabeçalho do gerenciador virtual de
 *      memória
 * @version 0.1
 * @date 2022-11-12
 * 
 * @copyright Copyright (c) 2022
 */

#ifndef MEMMAN_H
#define MEMMAN_H

#include <stdio.h>

#define PAGES 256
#define PAGE_SIZE 256
#define TLB_SIZE 16
#define FRAME_SIZE 256
#define MAX_MEM_SIZE 65536

typedef char byte;

/**
 * @brief Struct que representa uma entrada da TLB.
 */
typedef struct tlb_entry {
    int page_num,
        frame_num;
} tlb_entry;

/**
 * @brief Struct que representa uma página da memória.
 */
typedef struct mem_entry {
    byte frame_num,
         offset;
} mem_entry;

/**
 * @brief Struct que representa o gerenciador de memória
 */
typedef struct memory_manager {
    struct tlb_entry tlb[TLB_SIZE];
    int time[PAGES];
    int tlb_index;
    struct tlb_entry page_table[PAGES];
} memory_manager;

/**
 * @brief Struct que representa a memória principal.
 */
typedef struct memory {
    FILE* backing_store;
    byte main[MAX_MEM_SIZE];
} memory;

/**
 * @brief Obtém o elemento da TLB a partir do número de página
 * lido.
 * 
 * @param e Referência à entrada da TLB que será retornada
 * @param address Endereço lido do arquivo address.txt
 * @return tlb_entry* Ponteiro para a entrada da TLB
 */
tlb_entry* get_tlb_entry(tlb_entry* e, int address);

/**
 * @brief Extrai o número da página a partir do endereço de
 * 32-bits lido do arquivo address.txt
 * 
 * @param address Endereço de 32-bits
 * @return (int) Número da página
 */
int get_page_num(int address);

/**
 * @brief Extrai o offset a partir do endereço de 32-bits
 * lido do arquivo address.txt
 * 
 * @param address Endereço de 32-bits
 * @return (int) Offset
 */
int get_offset(int address);

/**
 * @brief Obtém o quadro da tábela de páginas a partir do número
 * de página lido.
 * 
 * @param mm Referência ao gerenciador de memória que contém a 
 * tabela de páginas.
 * @param page_num Número da página.
 * @retval >=0 Número do quadro;
 * @retval -1 Se a página não estiver na tabela de páginas.
 */
int get_frame_from_pagetable(memory_manager* mm, int page_num);

/**
 * @brief Obtém o quadro da TLB a partir do número
 * de página lido.
 * 
 * @param mm Referência ao gerenciador de memória que contém a
 * TLB.
 * @param page_num Número da página.
 * @retval >=0 Número do quadro;
 * @retval -1 Se a página não estiver na TLB.
 */
int get_frame_from_tlb(memory_manager* mm, int page_num);

/**
 * @brief Insere uma entrada na TLB utilizando FIFO.
 * 
 * @param mm Referência ao gerenciador de memória
 * @param page_num Número da página
 * @param frame_num Número do quadro
 */
void add_to_tlb_fifo(memory_manager* mm, int page_num, int frame_num);

/**
 * @brief Insere uma entrada na TLB utilizando LRU.
 * 
 * @param mm Referência ao gerenciador de memória
 * @param page_num Número da página
 * @param frame_num Número do quadro
 */
void add_to_tlb_lru(memory_manager* mm, int page_num, int frame_num);

/**
 * @brief Inicializa o gerenciador de memória
 * 
 * @param memman Referência ao gerenciador de memória a ser
 * inicializado.
 */
void memman_init(memory_manager* memman);

/**
 * @brief 
 * 
 * @param mem Referência à memória a ser inicializada.
 */
void memory_init(memory* mem, int mem_size);

/**
 * @brief Exibe o conteúdo da TLB.
 * 
 * @param mm Referência ao gerenciador de memória que contém a TLB.
 */
void display_tlb(memory_manager* mm);

/**
 * @brief Exibe as páginas válidas da tabela de páginas.
 * 
 * @param mm Referência ao gerenciador de memória que contém a 
 * tabela de páginas.
 */
void display_valid_pages(memory_manager* mm);

/**
 * @brief Exibe as páginas inválidas da tabela de páginas.
 * 
 * @param mm Referência ao gerenciador de memória que contém a 
 * tabela de páginas.
 */
void display_invalid_pages(memory_manager* mm);

#endif