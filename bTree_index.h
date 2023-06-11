/* Vicenzo D'Arezzo Zilio - 13671790 */
/* INFORMACOES JOAO */
/* Files Organization - 2023 */

#ifndef bTree_index_h
#define bTree_index_h

#include <stdio.h>
#include <stdlib.h>

// -----------------------------------
// -----------------------------------
/* IMLEMENTATION INFORMATION */
// -----------------------------------
// -----------------------------------


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
} BTree;



BT_header_t * bt_header_create(void);

BT_header_t * bt_header_read(FILE * src);

void bt_header_write(BT_header_t * h, FILE * src);

BT_node_t * bt_node_read(FILE * src, int value_RNN);

void bt_node_delete(BT_node_t ** node);

#endif /* bTree_index_h */
