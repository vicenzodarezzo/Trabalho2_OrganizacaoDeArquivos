/* Vicenzo D'Arezzo Zilio - 13671790 */
/* João Gabriel Manfré Nazar - 13733652 */
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
    long long int byteOffset;
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
}BTree;

typedef union branching_value{
    int next_RRN;
    BT_key finded_key;
}Branching_value;

typedef enum path_running{
    LEFT, // signifys that the searching path should follow the left pointer;
    RIGHT, // signifys that the searching path should follow the right pointer;
}Path_running;

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

/*
 * Is valid to say that the writing procedures do not manually move the read/write
 * file pointer inside the functions, so the program ha to set the right
 * position in the index file by it-self before the calling.
 */

void bt_header_write(BT_header_t * h, FILE * src);

void bt_node_write(BT_node_t * node, FILE * src);

// -----------------------------------
// -----------------------------------
/* GENERAL PURPOSE FUNCTIONS */
// -----------------------------------
// -----------------------------------

/**
 * Returns the integer value associated to the field represented by the string passed.
 * In that way, with this function, we are able to verify what field was indexed in the input files.
 */
int index_crimeField_pairing(char str[]);

void print_node(BT_node_t * no);

// -----------------------------------
// -----------------------------------
/* SEARCH FUNCTIONS */
// -----------------------------------
// -----------------------------------

/**
 * The functions that implements a search in the tree depth. The integer filter value representes the value of the searched key
 * in the file. In terms of memory, it receives the inicial node, the root, and, as it runs the recursion and access new nodes,
 * liberates theese in the recursion tail, accessing, in the worst case, N nodes simultaneously with N = tree height. In the end of the
 * procedure, the only node maintained in main memory will be the root.
 * The returned value can be the byteOffset of the finded key, or -1, indicating that the key isn't in the tree.
 */
long long int bTree_id_search(FILE * index_file, BT_node_t * node, int filter_value);

/**
 * A binary search implemented in a key list of a node. The returns indicates the result of the search, being the position
 * of the filter_value in the list or -1, representing the case where the searched key isn't in the list. For auxiliar information
 * in other functions, the last id accessed in the list is stored, such as the direction of the branching of the search.
 * In that way, we can use theese auxiliar information for obtain where the key should be in that list,
 */
int key_binary_search(BT_key * list, int inicial_id, int final_id, int filter_value, int * last_id, Path_running * path);

// -----------------------------------
// -----------------------------------
/* INSERTION FUNCTIONS */
// -----------------------------------
// -----------------------------------

/*
 * The next functions assumes that the inserted information is a Key and a
 * RRN pointer, that can maybe be valid, in the case of an propagated information
 * by the split2-3.
 *
 * The next functions receives the parameters nodes already read in main memory,
 * and can read at most more 3 nodes in its implamentation.
 *
 * In the end of the overflow procedures, the functions writes in the index file
 * the nodes that has been changed and liberates the memory alocated except for the
 * father node. For him, the liberation has to be done in the insertion recursion.
 *
 *
 *
 */

#endif /* bTree_index_h */
