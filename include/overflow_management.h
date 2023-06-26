//
//  overflow_management.h
//  BS_Tree
//
//  Created by Vicenzo D’Arezzo on 19/06/23.
//

#ifndef overflow_management_h
#define overflow_management_h

#include <stdio.h>
#include <stdlib.h>
#include "bTree_index.h"

typedef struct overflow_lists{
    BT_key * key_list;
    int * rrn_list; //size = list_len + 1
    int ascended_id[2];
    int inserted_id;
    int list_len;
}Overflow_block;

typedef struct inserted_block{
    BT_key key;
    int right_RRN;
}Insertion_block;

typedef enum list_type{
    KEY, RRN
}List_type;


// ------------------------------
// ------------------------------
// GENERAL PURPOSE
// ------------------------------
// ------------------------------

/**
 * Receives a list related to the informaton stored in a BTree node (keys or RRNs) and shifts all the greater
 * positions starting in the one stored at new_key_position. The occupancy_rate set the end of the shift and
 * is valid to say that this value is related to the node that the list represents, and not the list itself.
 * The Enum - List type - determinates how the void array will be interpreted by the procedure.
 */
void node_list_shift(void * list, int new_key_position, int occupancy_rate, List_type type);

/**
 * The function responsable for passing information from a generic BTree node to one partition of the lists in
 * the block passed as a parameter. The gap for the stored information is determinated by the positions and,
 * using them as parameters, we can apply this implementation for the different splits and for the redistribuition.
 */
Overflow_block * overflow_inicializing_interval(Overflow_block * info_block,
     BT_node_t * src_node, int list_ini_position, int list_final_postion);

/**
 * Actualizes the information in a node according to one specific interval in a overflow block. The interval is determinated by
 * the initial id and the information limit. The size limit represents the size of the key vector allocated in the node, so that,
 * when the interation passes the information limit, a null value can be written in the memory.
 */
BT_node_t * overflow_atualizing_Node(BT_node_t * tree_node, Overflow_block * info_block,
     int inicial_overflowList_id, int information_limit);

// ------------------------------
// ------------------------------
// BUILDING THE OVERFLOW BLOCK
// ------------------------------
// ------------------------------

/**
 * Creates an Overflow_block according to the situation involving three nodes, such as the redistribuition and the split
 * 2-3. This function allocates the memory dynamically and returns the list with the nodes, the father and the inserted
 * information to the program.
 * It's valid to say that this case do not calculates the ascending positions. That information has to be obtained outside the
 * function.
 */

Overflow_block * create_oveflowBlock_3Nodes(BT_key father_key, BT_node_t * insertion_node,
     BT_node_t * sister_node, Insertion_block * block, Path_running sister_direction);

/**
 * Creates an Overflow_block according to the situation involving one node, such as the split 1-2. 
 * This function allocates the memory dynamically and returns the list with the nodes, the father 
 * and the inserted information to the program. 
 * It's valid to say that this case do not calculates the ascending positions
 */
Overflow_block * create_oveflowBlock_1Node(BT_node_t * insertion_node, Insertion_block block);

// ------------------------------
// ------------------------------
// REDISTRIBUITION
// ------------------------------
// ------------------------------

/**
* Returns the sister page of the overflowed one, indicating the direction of the redistribuition
* and storing its relative position in the Path_running adress (direction) passed as a parameter.
* If it isn't a case of redistribuition, the function will return a NULL value.
*/
BT_node_t * key_redistribuition_decision(BT_node_t * father_node, int overflowed_RRN_id,
     FILE * index_file, Path_running * direction);


// ------------------------------
// ------------------------------
// SPLIT 2 - 3
// ------------------------------
// ------------------------------
/**
* Returns the sister page of the overflowed one, indicating the direction of the split
* and storing its relative position in the Path_running adress (direction) passed as a parameter.
*/
BT_node_t * node_Split2_3_decision(BT_node_t * father_node, int overflowed_RRN_id,
                                   FILE * index_file, Path_running * direction);




#endif /* overflow_management_h */
