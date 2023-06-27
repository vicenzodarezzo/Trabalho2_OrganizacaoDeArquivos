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

BT_header_t * bt_header_create(void){
    
    BT_header_t * header = (BT_header_t *) malloc(sizeof(BT_header_t));
    assert(header);
    
    header->status = '0';
    header->root_RRN = -1;
    header->prox_RRN = 0;
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
    node->level = 1;
    node->occupancy_rate = 0;
    
    // inicializing all the node information with -1 ;
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
    
    b_tree_delete(tree);
}

BTree * bt_index_create(FILE * index_file) {
    BTree * tree = b_tree_create();
    tree->index_file = index_file;

    tree->root = bt_node_create();
    tree->header = bt_header_create();
    tree->header->root_RRN = -1;
    tree->header->prox_RRN = 0;

    return tree;
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

void print_node(BT_node_t * node){
    printf("NO:\n");
    
    printf("RATE: %d\n", node->occupancy_rate);
    printf("\tCHAVES:\n");
    for(int i = 0; i < 4; i++){
        printf("\t\t %d ", node->keys[i].value);
    }
    printf("\n\tBOS:\n");
    for(int i = 0; i < 4; i++){
        printf("\t\t %lld ", node->keys[i].byteOffset);
    }
    printf("\n\tRRNS:\n");
    for(int i = 0; i < 5; i++){
        printf("\t\t %d ", node->descendants_RRN[i]);
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
    
    // filling the remaining disk page space
    char c = TRASH_IDENTIFIER ;
    
    for(int i = DISK_PAGE_SIZE - counter; i > 0; i--) fwrite(&c, sizeof(char), 1, src);
    
}

void bt_node_write(BT_node_t * node, FILE * src){
    int counter = 0;
    
    // printf("NO NA ESCRITA\n");
    // print_node(node);
    
    counter += sizeof(int) * fwrite(&(node->level), sizeof(int), 1, src);
    counter += sizeof(int) * fwrite(&(node->occupancy_rate), sizeof(int), 1, src);
    
    for(int i = 0; i < TREE_ORDER - 1; i++){
        counter += sizeof(int) * fwrite(&node->descendants_RRN[i], sizeof(int), 1, src);
        counter += sizeof(int) * fwrite(&node->keys[i].value, sizeof(int), 1, src);
        counter += sizeof(long long int) * fwrite(&node->keys[i].byteOffset,
             sizeof(long long int), 1, src);
    }
    
    counter += sizeof(int) * fwrite(&node->descendants_RRN[TREE_ORDER - 1], sizeof(int), 1, src);
    
    //printf("\n\nCOUNTER : %d \n\n", counter);
    
    // filling the remaining disk page space
    char c = TRASH_IDENTIFIER ;
    for(int i = DISK_PAGE_SIZE - counter; i > 0; i--) fwrite(&c, sizeof(char), 1, src);
    
    //printf("\n\nCOUNTER FINAL : %d \n\n", counter);
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

Branching_value bTree_branching_through_node(BT_node_t * node, int filter_value, bool * find_flag,
     int * last_key_id){
    
    int return_bSearch;
    Path_running path;
    Branching_value result;
    
    // searching for the filter value in the keys list of the current node;
    return_bSearch = key_binary_search(node->keys, 0, node->occupancy_rate - 1, filter_value, last_key_id, &path);
    
    if(return_bSearch != -1){
        // The value searched is in the returned position of the key list;
        *find_flag = true;
        // In this case, the returned value is the searched key
        result.finded_key = node->keys[return_bSearch];
    }else{
        *find_flag = false;
        // in the other case, the deep search has to continue through on of
        // the child pointer of the node;
        int next_RRN_id;
        next_RRN_id = (path == LEFT) ? (*last_key_id) : ((*last_key_id) + 1) ;
        result.next_RRN = node->descendants_RRN[next_RRN_id];
    }
    return result;
}


long long int bTree_id_search(FILE * index_file, BT_node_t * node, int filter_value){
    
    long long int info;
    bool find_flag;
    Branching_value branching_result;
    int last_key_id;
    
    branching_result = bTree_branching_through_node(node, filter_value, &find_flag, &last_key_id);
    
    
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

void key_sorted_insertion(BT_node_t * node, Insertion_block * block, FILE * index_file,
     int insertion_RRN){

    // the first step in this case is to find where
    // the new key will be placed in the node

    Path_running path = LEFT;
    int last_accessed_id = 0;

    int key_position;

    print_node(node);

    key_binary_search(node->keys, 0, node->occupancy_rate - 1,
        block->key.value, &last_accessed_id, &path);
    
    // the last_accessed_id + path represents where the key will be placed
    key_position = ((path == LEFT) ? last_accessed_id : last_accessed_id + 1);

    printf("\nkey position : %d\n", key_position);

    // now that the position is obtained, we have to obtain the space
    // in the node for this insertion. For this, we will shift all
    // the further keys and pointers:
    
    node_list_shift(node->keys, key_position, node->occupancy_rate, KEY);
    node_list_shift(node->descendants_RRN, key_position, node->occupancy_rate, RRN);
    
    // inserting the new key:

    node->keys[key_position] = block->key;
    node->descendants_RRN[key_position + 1] = block->right_RRN;

    // atualizing the node information:
    node->occupancy_rate = node->occupancy_rate + 1;
    
    // writing the node in secondary memory:
    
    printf("RRN DE ESCRITA: %d \n", insertion_RRN);
    fseek(index_file, (insertion_RRN + 1) * DISK_PAGE_SIZE, SEEK_SET);
    
    bt_node_write(node, index_file);

    printf("\nno dps:\n\n\n");
    print_node(node);

    // emptying the insertion block
    block->key.value = -1;
}


/* RECEBE:
    Os nós lidos em memória primária
    A página irmã decidida conforme a funcão acima implementada
    O bloco a ser inserido (Insertion Block)
    A informação sobre a direção da página Irmã
    O id da chave pai dentro do nó pai, obtida na recursão
*/

// JA DESALOCA O SISTER NODE POIS ELE NECESSARIAMENTE NAO SERA MAIS UTILIZADO AO FINAL DA OPERAÇÃO;

void key_redistribuition(BT_node_t * father_node, BT_node_t * insertion_node,
     BT_node_t * sister_node, int id_father_key, Insertion_block * block,
     Path_running sisterPage_direction, FILE * index_file, int father_RRN){
    
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
    
    if(sisterPage_direction == RIGHT){ // the case where the insertion page is lower than the sister_page ;
        
        insertion_node = overflow_atualizing_Node(insertion_node, info_block, 0,
             info_block->ascended_id[0]);
        
        sister_node = overflow_atualizing_Node(sister_node, info_block,
             info_block->ascended_id[0] + 1, info_block->list_len);
        
    }else{ // the case where the insertion page is greater than the sister_page ;
        
        sister_node = overflow_atualizing_Node(sister_node, info_block, 0,
             info_block->ascended_id[0]);
        
        insertion_node = overflow_atualizing_Node(insertion_node, info_block,
             info_block->ascended_id[0] + 1, info_block->list_len);
    }
    
    // CLEANING THE AUXILIARY STRUCTURES
    
    free(info_block->key_list);
    free(info_block->rrn_list);
    free(info_block);
    info_block = NULL;
    
    // WRITING IN SECONDARY MEMORY
    int insertion_RRN;
    int sister_RRN;
    
    if(sisterPage_direction == RIGHT){
        insertion_RRN = father_node->descendants_RRN[id_father_key];
        sister_RRN = father_node->descendants_RRN[id_father_key + 1];
    }else{
        sister_RRN = father_node->descendants_RRN[id_father_key];
        insertion_RRN = father_node->descendants_RRN[id_father_key + 1];
    }
    
    fseek(index_file, (insertion_RRN + 1) * DISK_PAGE_SIZE, SEEK_SET);
    bt_node_write(insertion_node, index_file);
    
    fseek(index_file, (sister_RRN + 1) * DISK_PAGE_SIZE, SEEK_SET);
    bt_node_write(sister_node, index_file);
    
    fseek(index_file, (father_RRN + 1) * DISK_PAGE_SIZE, SEEK_SET);
    bt_node_write(father_node, index_file);

    print_node(father_node);
    print_node(insertion_node);
    print_node(sister_node);
    
    // LIBERATING THE ADRESSED MEMORY
    
    bt_node_delete(&sister_node);
}


Insertion_block * node_split2_3(BT_node_t * father_node, BT_node_t * insertion_node,
     BT_node_t * sister_node, int id_father_key, Insertion_block * block,
     Path_running sisterPage_direction, BTree * tree, int father_RRN){
    
    Overflow_block * info_block;
    int new_RRN;
    FILE * index_file = tree->index_file;
    
    // CREATING THE SPLIT STRUCTURE : keys and pointers lists
    
    info_block = create_oveflowBlock_3Nodes(father_node->keys[id_father_key],
         insertion_node, sister_node, block, sisterPage_direction);
    
    // calculating the third_value of the list
    info_block->ascended_id[0] = info_block->list_len / 3 ;
    info_block->ascended_id[1] = 2 * info_block->list_len / 3  + 1;
    
    // CREATING THE NEW NODE IN MAIN MEMORY
    BT_node_t * new_node = bt_node_create();
    
    new_node->level = insertion_node->level;
    new_node->occupancy_rate = 0;
    new_RRN = tree->header->prox_RRN;
    
    // Atualizing the next RRN in the tree ;
    tree->header->prox_RRN = tree->header->prox_RRN + 1;
    
    // ASCENDING THE PROPAGATED NODES
    
    // -> the first node ascended will be exchanged with the father key
    father_node->keys[id_father_key] =
    info_block->key_list[info_block->ascended_id[0]];
    
    // -> the second node will be propageted in the tree recursion;
    block->key = info_block->key_list[info_block->ascended_id[1]];
    block->right_RRN = new_RRN;
    
    // ATUALIZING THE INSERTION LEVEL NODES
    
    insertion_node->occupancy_rate = 0;
    sister_node->occupancy_rate = 0;
    
    if(sisterPage_direction == RIGHT){
        //printf("\n\n insertion node \n\n");
        insertion_node = overflow_atualizing_Node(insertion_node, info_block, 0,
             info_block->ascended_id[0]);
        //printf("\n\n sister node \n\n");
        sister_node = overflow_atualizing_Node(sister_node, info_block,
            info_block->ascended_id[0] + 1, info_block->ascended_id[1]);
        
    }else{
        //printf("\n\n sister node \n\n");
        sister_node = overflow_atualizing_Node(sister_node, info_block, 0,
             info_block->ascended_id[0]);
        //printf("\n\n insertion node \n\n");
        insertion_node = overflow_atualizing_Node(insertion_node, info_block,
             info_block->ascended_id[0] + 1, info_block->ascended_id[1]);
    }
    //printf("\n\n new node \n\n");
    new_node = overflow_atualizing_Node(new_node, info_block, info_block->ascended_id[1] + 1,
             info_block->list_len);
    
    // CLEANING THE AUXILIARY STRUCTURES
    
    free(info_block->key_list);
    free(info_block->rrn_list);
    free(info_block);
    info_block = NULL;
    
    // WRITING IN SECONDARY MEMORY
    
    // -> NODES :
    int insertion_RRN;
    int sister_RRN;
    
    if(sisterPage_direction == RIGHT){
        insertion_RRN = father_node->descendants_RRN[id_father_key];
        sister_RRN = father_node->descendants_RRN[id_father_key + 1];
    }else{
        sister_RRN = father_node->descendants_RRN[id_father_key];
        insertion_RRN = father_node->descendants_RRN[id_father_key + 1];
    }
    
    printf("\n\nESCRITA\n\n");
    
    printf("INSERTION RRN : %d \n", insertion_RRN);
    fseek(index_file, (insertion_RRN + 1) * DISK_PAGE_SIZE, SEEK_SET);
    bt_node_write(insertion_node, index_file);
    
    printf("SISTER RRN : %d \n", sister_RRN);
    fseek(index_file, (sister_RRN + 1) * DISK_PAGE_SIZE, SEEK_SET);
    bt_node_write(sister_node, index_file);
    
    printf("NEW RRN : %d \n", new_RRN);
    fseek(index_file, (new_RRN + 1) * DISK_PAGE_SIZE, SEEK_SET);
    bt_node_write(new_node, index_file);
    
    printf("FATHER RRN : %d \n", father_RRN);
    fseek(index_file, (father_RRN + 1) * DISK_PAGE_SIZE, SEEK_SET);
    bt_node_write(father_node, index_file);
    
    
    // -> HEADER
    fseek(index_file, 0, SEEK_SET);
    bt_header_write(tree->header, index_file);
    
    // LIBERATING THE ADRESSED MEMORY
    bt_node_delete(&new_node); // was leaking but we thought it didnt need to be deleted, needs further analysis
    bt_node_delete(&sister_node);
    
    return block;
}

void node_split1_2(BT_node_t * root_node, BTree * tree, Insertion_block * block){
    
    FILE * index_file;
    index_file = tree->index_file;
    
    // creating the Overflow structure to store all the nodes information ;
    Overflow_block * info_block = create_oveflowBlock_1Node(root_node, * block);

    // calculating the mid_value of the list
    info_block->ascended_id[0] = info_block->list_len / 2 ;
    
    // creating new nodes that are generated in this split 
    BT_node_t * new_root = bt_node_create();
    BT_node_t * sister_node = bt_node_create();
    
    int previous_root_RRN = tree->header->root_RRN;
    
    int new_sister_RRN = (tree->header->prox_RRN)++;
    
    int new_root_RRN = (tree->header->prox_RRN)++;
    
    sister_node->level = root_node->level;
    new_root->level += 1;
    
    // atualizing tree information
    tree->root = new_root;
    tree->header->root_RRN = new_root_RRN;
    tree->header->height = tree->header->height + 1;
    
    // atualizing information in the nodes
    root_node = overflow_atualizing_Node(root_node, info_block, 0, info_block->ascended_id[0]);
    
    sister_node = overflow_atualizing_Node(sister_node, info_block, info_block->ascended_id[0] + 1, 
                    info_block->list_len);
    
    // NEW ROOT:
    new_root->occupancy_rate = 1;
    new_root->keys[0] = info_block->key_list[info_block->ascended_id[0]];
    new_root->descendants_RRN[0] = previous_root_RRN;
    new_root->descendants_RRN[1] = new_sister_RRN;
    
    // CLEANING THE AUXILIARY STRUCTURES
    free(info_block->key_list);
    free(info_block->rrn_list);
    free(info_block);
    info_block = NULL;
    
    // WRITING IN SECONDARY MEMORY
    fseek(index_file, (new_root_RRN + 1) * DISK_PAGE_SIZE, SEEK_SET);
    bt_node_write(new_root, index_file);
    
    fseek(index_file, (previous_root_RRN + 1) * DISK_PAGE_SIZE, SEEK_SET);
    bt_node_write(root_node, index_file);
    
    fseek(index_file, (new_sister_RRN + 1) * DISK_PAGE_SIZE, SEEK_SET);
    bt_node_write(sister_node, index_file);
    
    // LIBERATING THE ADRESSED MEMORY
    
    print_node(new_root);
    print_node(root_node);
    print_node(sister_node);

    bt_node_delete(&sister_node);

    // emptying the insertion block
    block->key.value = -1;
}

Insertion_block * overflow_treatment( BTree * tree, BT_node_t * father_node,
     BT_node_t * overflowed_node, int father_RRN, int id_father_key,
     Insertion_block * insert_info){
    
    
    if(overflowed_node->level == tree->root->level) { 
        printf("NO RAIZ -> SPLIT 1/2\n");
        node_split1_2(overflowed_node, tree, insert_info);
        insert_info->key.value = -1;
        
    }else{
        int overflowed_RRN_id = id_father_key + (insert_info->key.value > father_node->keys[id_father_key].value);

        BT_node_t * sister_node;
        Path_running sister_direction;
        
        sister_node = key_redistribuition_decision(father_node, overflowed_RRN_id,
            tree->index_file, &sister_direction);
        
        if(sister_node != NULL){
            printf("-> REDST.\n");
            key_redistribuition(father_node, overflowed_node, sister_node, id_father_key,
                 insert_info, sister_direction, tree->index_file, father_RRN);
            insert_info->key.value = -1;

            // emptying the insertion block
            insert_info->key.value = -1;
            
        }else{
            printf(" -> SPLIT 2/3\n");
            sister_node = node_Split2_3_decision(father_node, overflowed_RRN_id,
                 tree->index_file, &sister_direction);
            
            // in the case of the 2-3 split, the propagated block will carry
            // the second ascended key with the RRN pointer to the new node
            // allocated, ir order to insert the block in the father node;
            
            insert_info = node_split2_3(father_node, overflowed_node, sister_node,
                 id_father_key, insert_info, sister_direction, tree, father_RRN);
            
        }
    }
    
    return insert_info;
}

void bTree_insertion_in_node(BTree * tree, BT_node_t * current_node, BT_node_t * father_node,
  int father_RRN, int current_RRN, int id_father_key, Insertion_block * insertion_info){
    
    
    if(tree->root->occupancy_rate == 0){
        // empty tree : insertion in the empty root
        key_sorted_insertion(tree->root, insertion_info, tree->index_file,
             tree->header->root_RRN);
        
    }else{
        
        if(current_node->occupancy_rate < TREE_ORDER - 1){
            // there is space in the current node and it is a leaf, so, we insert the key
            key_sorted_insertion(current_node, insertion_info, tree->index_file, current_RRN);
            
        }else{
            printf("OVERFLOW\n");

            insertion_info = overflow_treatment(tree, father_node, current_node, father_RRN,
                id_father_key, insertion_info);
        }
    }
}
    

bool bTree_recursion_insertion(BTree * tree, BT_node_t * current_node, int current_RRN,
    BT_node_t * last_node, int last_node_RRN, int last_key_id, Insertion_block * block){
    
    // searching the information in the node :
    
    bool find_flag;
    int new_key_id;
    Branching_value branch_result;

    printf("RRN ATUAL :: %d\n\n", current_RRN);
    print_node(current_node);
    printf("\n\n");
    branch_result = bTree_branching_through_node(current_node, block->key.value, &find_flag,
        &new_key_id);
    
    if(find_flag){
        fprintf(stdout, " %d THE INSERTED KEY ALREADY EXISTS IN TREE\n", block->key.value);
        return false;
    }else{
        
        // now we have to be able to find the end of the insertion path to
        // insert the node in the right position
        
        if(branch_result.next_RRN != -1){
            
            // there is a valid pointer to be accessed in the current node, causing the
            // insertion to branch through the depth of the tree ;
            
            BT_node_t * new_node = bt_node_read(tree->index_file, branch_result.next_RRN);
            
            bool success = bTree_recursion_insertion(tree, new_node, branch_result.next_RRN, current_node,
               current_RRN, new_key_id, block);
            
            // in the recursion tail, we liberate the memory associated to the obsolete node
            bt_node_delete(&new_node);
            
            // still in the tail, we have to treat the case of an propagated key that can
            // be generated by a 2-3 split with a full father node;
            // for this, we have to test in every tail if a key is still in the insertion
            // block registry.
            
            if(block->key.value != -1 && success == true){
                bTree_insertion_in_node(tree, current_node, last_node,
                last_node_RRN, current_RRN, last_key_id, block);
            }

            return success;
            
        }else{
            printf("ACHOU O NÓ DE INSERCAO\n");
        
            bTree_insertion_in_node(tree, current_node, last_node,
               last_node_RRN, current_RRN, last_key_id, block);
            
            tree->header->n_indexed_keys = tree->header->n_indexed_keys + 1;

            return true;
        }
    }
}

bool bTree_id_insertion(BTree * tree, BT_key key) {
    Insertion_block block;

    block.key = key;
    block.right_RRN = -1;
    
    if(tree->header->root_RRN == -1) {
        tree->header->root_RRN += 1;
        tree->header->prox_RRN += 1;
        tree->header->height += 1;

        key_sorted_insertion(tree->root, &block, tree->index_file,
             tree->header->root_RRN);
            
        tree->header->n_indexed_keys = tree->header->n_indexed_keys + 1;
        return true;
    }
    
    return bTree_recursion_insertion(tree, tree->root, tree->header->root_RRN, NULL, -1, 0, &block);
}
