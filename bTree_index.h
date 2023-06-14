/* Vicenzo D'Arezzo Zilio - 13671790 */
/* INFORMACOES JOAO */
/* Files Organization - 2023 */

#ifndef bTree_index_h
#define bTree_index_h

#include <stdio.h>
#include <stdlib.h>
#include "registers.h"


// -----------------------------------
// -----------------------------------
/* IMLEMENTATION INFORMATION */
// -----------------------------------
// -----------------------------------

# define DISK_PAGE_SIZE 76

# define INDEX_IDENTIFIER 0
# define TREE_ORDER 5


// OCCUPANCY RATE IN THE TREE NODES:
// --> Root Node : at least 2
// --> Inside Node : (2 * m - 1)/3
// --> Leaf Node : floor((2 * m - 1)/3)

// -----------------------------------
// -----------------------------------
/* TREE REGISTRY STRUCTURES */
// -----------------------------------
// -----------------------------------


typedef struct tree_header{
    char status;
    int root_RRN;
    int prox_RRN;
    int height;
    int n_indexed_keys;
}BT_header_t;

typedef struct tree_key{
    int value;
    unsigned long int byteOffset;
}BT_key;

typedef struct tree_node{
    int level;
    int occupancy_rate;
    BT_key keys [TREE_ORDER - 1];
    int descendants_RRN [TREE_ORDER];
}BT_node_t;

typedef struct tree{
    BT_header_t * header;
    BT_node_t * root;
    FILE * index_file;
} BTree;

typedef enum search_purpose{
    INSERTION, // will determinate the return of the RRN value for insertion;
    ACCESSING, // will determinate the return of the byteOffset for the acessing;
}Search_purpose;

// -----------------------------------
// -----------------------------------
/* SETUP FUNCTIONS */
// -----------------------------------
// -----------------------------------

void bt_node_delete(BT_node_t ** node);

void bt_header_delete(BT_header_t ** header);

/**
 * Liberates the memory associated to the tree pointer correctly and sets the header status to valid again in second
 * memory. Is valid to remember that the file stored in the registry is not closed by this function, that task is not in the
 * scope of the bTree TAD in our implementation.
 */
void bTree_closing(BTree ** tree);

// -----------------------------------
// -----------------------------------
/* READ FUNCTIONS */
// -----------------------------------
// -----------------------------------

/*
 * It is worth to explain that theese reading functions are responsable
 * to create the space in the memory and inicilize it by the information
 * in the file passed as the parameter.
 */
    
/*
 * In that way, we also indicates that the files mentioned above have to
 * be opened by the implementation, The BTree TAD is not responsable for
 * opening or closing they.
 */

BT_header_t * bt_header_read(FILE * src);

BT_node_t * bt_node_read(FILE * src, int value_RNN);

BTree * bTree_initializing(FILE * index_file);

// -----------------------------------
// -----------------------------------
/* WRITE FUNCTIONS */
// -----------------------------------
// -----------------------------------

void bt_header_write(BT_header_t * h, FILE * src);

// -----------------------------------
// -----------------------------------
/* GENERAL PURPOSE FUNCTIONS */
// -----------------------------------
// -----------------------------------

int index_crimeField_pairing(char str[]);


// -----------------------------------
// -----------------------------------
/* SEARCH FUNCTIONS */
// -----------------------------------
// -----------------------------------

long long int bTree_id_search(BTree * tree, int filter_value,
    Search_purpose purpose);

void print_no(BT_node_t * no);


#endif /* bTree_index_h */
