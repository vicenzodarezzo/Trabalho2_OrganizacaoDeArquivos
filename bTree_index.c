/* Vicenzo D'Arezzo Zilio - 13671790 */
/* INFORMACOES JOAO */
/* Files Organization - 2023 */

#include "bTree_index.h"
#include "page_buffer.h"




// -----------------------------------
// -----------------------------------
/* IMPLEMENTATION REGISTRY */
// -----------------------------------
// -----------------------------------

typedef enum path_running{
    LEFT, // signifys that the searching path should follow the left pointer;
    RIGHT, // signifys that the searching path should follow the right pointer;
} Path_running;

typedef enum search_purpose{
    INSERTION, // will determinate the return of the RRN value for insertion;
    ACCESSING, // will determinate the return of the byteOffset for the acessing;
}Search_purpose;

typedef struct search_return{
    Path_running path; // used in the key binary search for the branching in the B-tree
    int last_key_id; // used in the key binary search for the branching in the B-tree
    int returned_keyID;
    unsigned long int returned_byteOffset;
} Search_return;


// -----------------------------------
// -----------------------------------
/* SETUP FUNCTIONS */
// -----------------------------------
// -----------------------------------

BT_header_t * bt_header_create(void){
    
    BT_header_t * header = (BT_header_t *) malloc(sizeof(BT_header_t));
    assert(header);
    
    header->status = '0';
    header->root_RRN = -1;
    header->prox_RRN = -1;
    header->height = 0;
    header->n_indexed_keys = 0;
    
    return header;
}

void bt_header_delete(BT_header_t ** header){
    free(*header);
    *header = NULL;
}

BT_node_t * bt_node_create(void){
    
    BT_node_t * node = (BT_node_t *) malloc(sizeof(BT_node_t));
    assert(node);
    node->level = 0;
    node->occupancy_rate = 0;
    
    // inicializing all the node information with -1 = NILL value ;
    for (int i = 0; i < TREE_ORDER - 1; i++){
        node->keys[i].value = -1;
        node->keys[i].byteOffset = -1;
        node->descendants_RRN[i] = -1;
    }
    node->descendants_RRN[TREE_ORDER - 1] = -1;
    
    return node;
}

void bt_node_delete(BT_node_t ** node){
    free(*node);
    *node = NULL;
}

BTree * b_tree_create(void){
    BTree * tree = (BTree * ) malloc(sizeof(BTree));
    assert(tree);
    
    tree->header = bt_header_create();
    tree->root = bt_node_create();
    
    return tree;
}

void b_tree_delete(BTree ** tree){
    bt_header_delete(&(*tree)->header);
    bt_node_delete(&(*tree)->root);
    free(*tree);
    *tree = NULL;
}



// -----------------------------------
// -----------------------------------
/* READING FUNCTIONS */
// -----------------------------------
// -----------------------------------

BT_header_t * bt_header_read(FILE * src){
    
    int counter = 0;
    
    BT_header_t * h = bt_header_create();
    
    counter += sizeof(char) * fread(&h->status, sizeof(char), 1, src);
    counter += sizeof(int) * fread(&h->root_RRN, sizeof(int), 1, src);
    counter += sizeof(int) * fread(&h->prox_RRN, sizeof(int), 1, src);
    counter += sizeof(int) * fread(&h->height, sizeof(int), 1, src);
    counter += sizeof(int) * fread(&h->n_indexed_keys, sizeof(int), 1, src);
    
    printf("HEADER READ : Valor lido : %d\n", counter);
    
    fseek(src, DISK_PAGE_SIZE - counter, SEEK_CUR);
    
    return h;
}

BT_node_t * bt_node_read(FILE * src, int value_RRN){
    
    fseek(src, value_RRN * DISK_PAGE_SIZE, SEEK_SET);
    
    int counter = 0;
    
    BT_node_t * n = bt_node_create();
    
    counter += sizeof(int) * fread(&n->level, sizeof(int), 1, src);
    counter += sizeof(int) * fread(&n->occupancy_rate, sizeof(int), 1, src);
    
    for(int i = 0; i < TREE_ORDER - 1; i++){
        counter += sizeof(int) * fread(&n->descendants_RRN[i], sizeof(int), 1, src);
        counter += sizeof(int) * fread(&n->keys[i].value, sizeof(int), 1, src);
        counter += sizeof(unsigned long int) * fread(&n->keys[i].byteOffset, sizeof(unsigned long int), 1, src);
    }
    
    counter += sizeof(int) * fread(&n->descendants_RRN[TREE_ORDER - 1], sizeof(int), 1, src);
    
    printf("NODE READ : Valor lido : %d\n", counter);
    
    fseek(src, DISK_PAGE_SIZE - counter, SEEK_CUR);
    
    return n;
}

// -----------------------------------
// -----------------------------------
/* WRITE FUNCTIONS */
// -----------------------------------
// -----------------------------------

void bt_header_write(BT_header_t * h, FILE * src){
    
    int counter = 0;
    
    counter += sizeof(char) * fwrite(&h->status, sizeof(char), 1, src);
    counter += sizeof(int) * fwrite(&h->root_RRN, sizeof(int), 1, src);
    counter += sizeof(int) * fwrite(&h->prox_RRN, sizeof(int), 1, src);
    counter += sizeof(int) * fwrite(&h->height, sizeof(int), 1, src);
    counter += sizeof(int) * fwrite(&h->n_indexed_keys, sizeof(int), 1, src);
    
    printf("HEADER WRITE : Valor escrito : %d\n", counter);
    char c = TRASH_IDENTIFIER ;
    fwrite(&c, sizeof(char), DISK_PAGE_SIZE - counter, src);
}

// -----------------------------------
// -----------------------------------
/* SEARCH FUNCTIONS */
// -----------------------------------
// -----------------------------------


/* This function is responsible for running the key list as a tree, returning the id
of the list where the filter value is contained, if it is. In the other case, returns NILL.
The information stored in the last_id and path will be used in the heuristic for propagate
the search over the tree. */
int key_binary_search(BT_key * list, int inicial_id, int final_id, int filter_value, int * last_id, Path_running * path){

    // the condition of the recursion :
    
    if(inicial_id <= final_id){
        
        int mid_id = inicial_id + (final_id - inicial_id) / 2;

        // comparing the middle index to the searched one and branching the recursion direction
        
        int condition = list[mid_id].value - filter_value;
        *last_id = mid_id;
        
        if(condition == 0) return mid_id;
        
        else if(condition < 0){
            *path = RIGHT;
            return key_binary_search(list, mid_id + 1, final_id, filter_value, last_id, path);
        }

        else{
            *path = LEFT;
            return key_binary_search(list, inicial_id, mid_id - 1, filter_value, last_id, path);
        }
    }
    
    // Stop condition is validated
    
    return -1;
    
}

//
//
//Search_return bTree_search_recursion(BT_node_t * node, int filter_value, Page_buffer * buffer)
//{
//    
//    Search_return info;
//    
//    info.returned_keyID = key_binary_search(node->keys, 0, TREE_ORDER - 2, filter_value, &info.last_key_id, &info.path);
//    
//    // The value searched is in the node_returned position of the key list;
//    if(info.returned_keyID != -1){
//        info.returned_byteOffset = node->keys[info.returned_keyID].byteOffset;
//        return info;
//    }
//    
//    // The value isn't in the current node, so :
//    
//    // -> if the node is a leaf, the value isn't in the tree
//    if(node->level == 1){
//        return info;
//    }
//    
//    // -> the node isn't a leaf, so, we have to read another page and search the value there
//    int next_RRN_id;
//    int next_RRN;
//    next_RRN_id = (info.path == LEFT) ? info.last_key_id : info.last_key_id + 1 ;
//    next_RRN = node->descendants_RRN[next_RRN_id];
//    
//    // ----> if the node isn't a leaf but the searched path is empty, the node ins't in the tree
//    //       and the last visited node is the current one;
//    if(next_RRN == -1){
//        return info;
//    }
//    // ----> if the node isn't a leaf but the searched path isn't empty, the search has
//    //       to branch through the calculated next RRN ;
//    else{
//        // atualize de buffer;
////        return bTree_search_recursion(buffer->list[buffer->counter++], filter_value, buffer);
//    }
//}
//
//
//unsigned long int bTree_search(BTree * tree, int filter_value, Search_purpose purpose){
//    
//    // creating a node buffer to store the read nodes in the main memory;
//    Page_buffer buffer;
//    buffer.counter = 1;
//    // passing the root to the buffer;
//    buffer.list[0] = tree->root;
//    
//    Search_return search_info;
//    
//    search_info = bTree_search_recursion(buffer.list[0], filter_value, &buffer);
//    
//    /*
//     
//       LEAF ROOT : if the tree is empty or the root is a leaf, the above function won't be able
//       to determine the last node RRN because the recursion will stop in the first step.
//       We know that this information is required for the insertion, so, this value will
//       be manually added to the returned registry if the case requires it.
//     
//     */
//    
//    
//    if(purpose == INSERTION){
//        // vai ser o último nó inserido no buffer;
//        return search_info.last_node_RRN;
//    }else{
//        // apagar todo o buffer
//        return search_info.returned_byteOffset
//    }
//  
//}


