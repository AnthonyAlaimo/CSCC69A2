#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include "pagetable.h"
#include "sim.h"



extern int debug;

extern struct frame *coremap;

  
typedef struct linked_list{
	addr_t address;
	struct linked_list *next_node;
}node;

node *head;

/* Page to evict is chosen using the optimal (aka MIN) algorithm. 
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */
int opt_evict() {
	int i=0;
	int output = -1;
	int max = -1;
	int found;
	int address_cell;
	int j;
	while(i < memsize) {
			found = 0;
			j = 0;
			address_cell = 0;
			node *curr = head;
			while (curr != NULL) {
				if (curr->address == coremap[i].address) {
					address_cell = j;
					found = 1;
					break;
				}
				j++;
				curr = curr->next_node;
			}


		// If the frame will never appear again, just return it
			if (found == 0) {
				return i;
			} else if (address_cell > max) {
				output = i;
				max = address_cell;
			}

			i = i+1;
	}

	return output;
}

/* This function is called on each access to a page to update any information
 * needed by the opt algorithm.
 * Input: The page table entry for the page that is being accessed.
 */
void opt_ref(pgtbl_entry_t *p) {
	node *cur = head;
	head = head->next_node;
	free(cur);
}

/* Initializes any data structures needed for this
 * replacement algorithm.
 */
void opt_init() {
	char buf[MAXLINE];
	addr_t vaddr = 0;
	char type;
	FILE* trace_file;

	if(( trace_file= fopen(tracefile, "r")) == NULL) {
		perror("frace file does not exist");
		exit(1);
	}

	node *prev_node;

	// Load in the tracefile using modified same code via sim.c
	while(fgets(buf, MAXLINE, trace_file) != NULL) {
		if(buf[0] != '=') {
			sscanf(buf, "%c %lx", &type, &vaddr);

			node *new_trace = (node*) malloc(sizeof(node));
			new_trace->address = vaddr;
			new_trace->next_node = NULL;

			// If it's the first trace read, it's now the head of LL
			if (head != NULL) {
				head = new_trace;
			} else {
				prev_node->next_node = new_trace;
			}

			prev_node = new_trace;
		} else {
			continue;
		}
	}

}

