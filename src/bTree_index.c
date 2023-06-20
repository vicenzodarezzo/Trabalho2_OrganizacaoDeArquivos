/* Vicenzo D'Arezzo Zilio - 13671790 */
/* João Gabriel Manfré Nazar - 13733652 */
/* Files Organization - 2023 */

#include "../include/bTree_index.h"
#include "../include/inputOutput.h"
#include "../include/overflow_management.h"

// -----------------------------------
// -----------------------------------
/* SETUP FUNCTIONS */
// -----------------------------------
// -----------------------------------

int key_comparator(const void * a, const void * b){
    return ((BT_key *) a)->value > ((BT_key *) b)->value ? 1 : -1 ;
}

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
    
    return tree;
}

void b_tree_delete(BTree ** tree){
    bt_header_delete(&(*tree)->header);
    bt_node_delete(&(*tree)->root);
    free(*tree);
    *tree = NULL;
}

int index_crimeField_pairing(char str[]){
    int returned_value;
    if(!strcmp(str, "idCrime")){
        returned_value = 0;
    }else if(!strcmp(str, "dataCrime")){
        returned_value = 1;
    }else if(!strcmp(str, "numeroArtigo")){
        returned_value = 2;
    }else if(!strcmp(str, "lugarCrime")){
        returned_value = 3;
    }else if(!strcmp(str, "descricaoCrime")){
        returned_value = 4;
    }else if(!strcmp(str, "marcaCelular")){
        returned_value = 5;
    }else{
        printf("Error pairing input string with a field");
        exit(1) ;
    }
    return returned_value;
}

void bTree_closing(BTree ** tree){
    
    BTree * t = *tree;
    
    // now that the using of the btree in main memory is ended,
    // we have to indicate that the index file in second memory
    // is again valid;
    
    t->header->status = '1';
    fseek(t->index_file, 0, SEEK_SET);
    bt_header_write(t->header, t->index_file);
    
    bt_node_delete(&(t->root));
    bt_header_delete(&(t->header));
    
    free(t);
    t = NULL;
    
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
    
    return h;
}

BT_node_t * bt_node_read(FILE * src, int value_RRN){
    
    fseek(src, (value_RRN + 1)* DISK_PAGE_SIZE, SEEK_SET);
    
    int counter = 0;
    
    BT_node_t * n = bt_node_create();
    
    counter += sizeof(int) * fread(&n->level, sizeof(int), 1, src);
    counter += sizeof(int) * fread(&n->occupancy_rate, sizeof(int), 1, src);
    
    for(int i = 0; i < TREE_ORDER - 1; i++){
        counter += sizeof(int) * fread(&n->descendants_RRN[i], sizeof(int), 1, src);
        counter += sizeof(int) * fread(&n->keys[i].value, sizeof(int), 1, src);
        counter += sizeof(long long int) * fread(&n->keys[i].byteOffset, sizeof(long long int), 1, src);
    }
    
    counter += sizeof(int) * fread(&n->descendants_RRN[TREE_ORDER - 1], sizeof(int), 1, src);
    
    
    return n;
}

BTree * bTree_initializing(FILE * index_file){
    
    // while the tree is manipulated in main memory, we have to
    // indicate the inconsistency of the information in stored
    // second memory at the file.
    
    BTree * tree = b_tree_create();
    tree->header = bt_header_read(index_file);
    
    if(tree->header->status == '0'){
        fprintf(stdout, "Falha no processamento do arquivo.\n");
        exit(0);
    }
    
    tree->header->status = '0';
    fseek(index_file, 0, SEEK_SET);
    bt_header_write(tree->header, index_file);
    
    tree->root = bt_node_read(index_file, tree->header->root_RRN);
    tree->index_file = index_file;
    
    return tree;
}

void print_no(BT_node_t * no){
    printf("NO:\n");
    
    printf("RATE: %d\n", no->occupancy_rate);
    printf("\tCHAVES:\n");
    for(int i = 0; i < 4; i++){
        printf("\t\t %d ", no->keys[i].value);
    }
    printf("\n\tBOS:\n");
    for(int i = 0; i < 4; i++){
        printf("\t\t %lld ", no->keys[i].byteOffset);
    }
    printf("\n\tRRNS:\n");
    for(int i = 0; i < 5; i++){
        printf("\t\t %d ", no->descendants_RRN[i]);
    }
    printf("\n");
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
    
    char c = TRASH_IDENTIFIER ;
    
    for(int i = DISK_PAGE_SIZE - counter; i > 0; i--) fwrite(&c, sizeof(char), 1, src);
    
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
    }else{
        // Stop condition is validated
        return -1;
    }
   
}

Branching_value bTree_branching_through_node(BT_node_t * node, int filter_value, bool * find_flag){
    
    int return_bSearch;
    Path_running path;
    int last_key_id;
    Branching_value result;
    
    // searching for the filter value in the keys list of the current node;
    return_bSearch = key_binary_search(node->keys, 0, node->occupancy_rate - 1, filter_value, &last_key_id, &path);
    
    if(return_bSearch != -1){
        // The value searched is in the returned position of the key list;
        *find_flag = true;
        // In this case, the returned value is the searched key
        result.finded_key = node->keys[return_bSearch];
    }else{
        // in the other case, the deep search has to continue through on of
        // the child pointer of the node;
        int next_RRN_id;
        next_RRN_id = (path == LEFT) ? last_key_id : last_key_id + 1 ;
        result.next_RRN = node->descendants_RRN[next_RRN_id];
    }
    return result;
}


long long int bTree_id_search(FILE * index_file, BT_node_t * node, int filter_value){
    
    long long int info;
    bool find_flag;
    Branching_value branching_result;
    
    branching_result = bTree_branching_through_node(node, filter_value, &find_flag);
    
    
    if(find_flag) info = branching_result.finded_key.byteOffset;
    else{
        if(branching_result.next_RRN == -1){
            // the branching finded the last position it can reach, without finding the
            // filter value ;
            info = -1;
        }else{
            // if the next RRN is an valid reference, the recursion continues through the
            // tree depth, accessing in main memory a new node in the next tree level ;
           
            BT_node_t * next_node = bt_node_read(index_file, branching_result.next_RRN);

            info = bTree_id_search(index_file, next_node, filter_value);
            
            // deletion of the obsolete node int the recursion tail ;
            bt_node_delete(&next_node);
        }
    }
    
    return info;
}

// -----------------------------------
// -----------------------------------
/* INSERTION FUNCTIONS */
// -----------------------------------
// -----------------------------------

// AUXILIAR FUNCTIONS :

/**
 * The next procedures represents cases that individually represents one sections
 * of the insertion in a B star tree. They are:
 *  --> sorted_simple_insertion
 *  --> redistribuition
 *  --> split 1 - 2
 *  --> split 2 - 3
 */

// ASSUMINDO QUE O BLOCO TEM NO MAX FILHO SOMENTE NA DIREITA


//Essa funcao esta assumindo que o inserted block ja contem o filho ou setado para o
//ponteiro propagado ou setado para -1;

void key_sorted_insertion(BT_node_t * node, Insertion_block * block, FILE * index_file){

    // the first step in this case is find where
    // the new key will be placed in the node

    Path_running path;
    int last_accessed_id;
    int b_search_return;

    int key_position;

    printf("No antes:\n");
    print_no(node);

    b_search_return = key_binary_search(node->keys, 0, node->occupancy_rate,
                            block->key.value, &last_accessed_id, &path);

    // the last_accessed_id + path represents where the key will be placed
    key_position = (path == LEFT) ? last_accessed_id : last_accessed_id + 1;

    // now that the position is obtained, we have to obtain the space
    // in the node for this insertion. For this, we will shift all
    // the further keys and pointers:
    
    node_list_shift(node->keys, key_position, node->occupancy_rate, KEY);
    node_list_shift(node->descendants_RRN, key_position, node->occupancy_rate, RRN);
    
    // iserting the new key:

    node->keys[key_position] = block->key;
    node->descendants_RRN[key_position + 1] = block->right_RRN;

    // atualizing the node information:
    node->occupancy_rate = node->occupancy_rate + 1;

    printf("no dps:\n");
    print_no(node);
}

/* RECEBE:
    Os nós lidos em memória primária
    A página irmã decidida conforme a funcão acima implementada
    O bloco a ser inserido (Insertion Block)
    A informação sobre a direção da página Irmã
    O id da chave pai dentro do nó pai, obtida na recursão
*/

// JA DESALOCA O SISTER NODE ;

void key_redistribuition(BT_node_t * father_node, BT_node_t * insertion_node,
     BT_node_t * sister_node, int id_father_key, Insertion_block block,
     Path_running sisterPage_direction){
    
    Overflow_block * info_block;
    
    // CREATING THE REDISTRIBUITION STRUCTURE : keys and pointers lists
    
    info_block = create_oveflowBlock_3Nodes(father_node->keys[id_father_key],
         insertion_node, sister_node, block, sisterPage_direction);
    
    // ATUALIZING THE NODES
    
    // calculating the mid_value of the list
    info_block->ascended_id[0] = info_block->list_len / 2 ;
    
    // switching the father key with the ascended one in the key list;
    father_node->keys[id_father_key] = info_block->key_list[info_block->ascended_id[0]];
   
    insertion_node->occupancy_rate = 0;
    sister_node->occupancy_rate = 0;
    
    // the case where the insertion page is lower than the sister_page ;
    if(sisterPage_direction == RIGHT){
        
        insertion_node = overflow_atualizing_Node(insertion_node, info_block, 0,
             info_block->ascended_id[0], TREE_ORDER - 1);
        
        sister_node = overflow_atualizing_Node(sister_node, info_block,
             info_block->ascended_id[0] + 1, info_block->list_len, 2 * (TREE_ORDER -1) + 1);
        
    // the case where the insertion page is higher than the sister_page ;
    }else{
        
        sister_node = overflow_atualizing_Node(sister_node, info_block, 0,
             info_block->ascended_id[0], TREE_ORDER - 1);
        
        insertion_node = overflow_atualizing_Node(insertion_node, info_block,
             info_block->ascended_id[0] + 1, info_block->list_len, 2 * (TREE_ORDER -1) + 1);
    }
    
    // CLEANING THE AUXILIARY STRUCTURES
    
    free(info_block->key_list);
    free(info_block->rrn_list);
    free(info_block);
    info_block = NULL;
    
    // WRITING IN SECONDARY MEMORY!!!!!
}



/*
 RECEBE:
    O nó pai de onde a inserção parte;
    O arquivo da árvore
    O id da pagina onde vai ocorrer a inserção
    Um endereço do tipo Path_Running onde será armazenada a direção da página escolhida
        com base na página de inserção
 RETORNA:
    A posição em memória da página irmã
 */


Insertion_block * node_split2_3(BT_node_t * father_node, BT_node_t * insertion_node,
     BT_node_t * sister_node, FILE * index_file, int id_father_key,
     Insertion_block * block, Path_running sisterPage_direction,
     BTree * tree){
    
    Overflow_block * info_block;
    int new_rrn;
    
    // CREATING THE REDISTRIBUITION STRUCTURE : keys and pointers lists
    
    info_block = create_oveflowBlock_3Nodes(father_node->keys[id_father_key],
         insertion_node, sister_node, *block, sisterPage_direction);
    
    // calculating the third_value of the list
    info_block->ascended_id[0] = info_block->list_len / 3 ;
    info_block->ascended_id[1] = 2 * info_block->list_len / 3 ;
    
    // CREATING THE NEW NODE IN MAIN MEMORY
    BT_node_t * new_node = bt_node_create();
    
    new_node->level = insertion_node->level;
    new_node->occupancy_rate = 0;
    new_rrn = tree->header->prox_RRN;
    
    // Atualizing the next RRN in the tree ;
    tree->header->prox_RRN = tree->header->prox_RRN + 1;
    
    // ASCENDING THE PROPAGATED NODES
    
    // -> the first node ascended will be exchanged with the father key
    father_node->keys[id_father_key] =
    info_block->key_list[info_block->ascended_id[0]];
    
    // -> the second node will be propageted in the tree recursion;
    block->key = info_block->key_list[info_block->ascended_id[1]];
    block->right_RRN = new_rrn;
    
    // ATUALIZING THE INSERTION LEVEL NODES
    
    insertion_node->occupancy_rate = 0;
    sister_node->occupancy_rate = 0;
    
    if(sisterPage_direction == RIGHT){
        
        insertion_node = overflow_atualizing_Node(insertion_node, info_block, 0,
             info_block->ascended_id[0], TREE_ORDER - 1);
        
        sister_node = overflow_atualizing_Node(sister_node, info_block,
            info_block->ascended_id[0] + 1, info_block->ascended_id[1], 2 * (TREE_ORDER - 1) + 1);
        
    }else{
        
        sister_node = overflow_atualizing_Node(sister_node, info_block, 0,
             info_block->ascended_id[0], TREE_ORDER - 1);
        
        insertion_node = overflow_atualizing_Node(insertion_node, info_block,
             info_block->ascended_id[0] + 1, info_block->ascended_id[1], 2 * (TREE_ORDER - 1) + 1);
    }
    
    new_node = overflow_atualizing_Node(new_node, info_block, info_block->ascended_id[1] + 1,
             info_block->list_len, 3 * (TREE_ORDER - 1) + 2);
    
    // CLEANING THE AUXILIARY STRUCTURES
    
    free(info_block->key_list);
    free(info_block->rrn_list);
    free(info_block);
    info_block = NULL;
    
    // WRITING IN SECONDARY MEMORY !!!!!

    
    // LIBERATING MEMORY !!!!!!!!!
    
    return block;
    
}

