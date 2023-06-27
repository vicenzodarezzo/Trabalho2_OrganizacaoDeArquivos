//
//  overflow_management.c
//  BS_Tree
//
//  Created by Vicenzo D’Arezzo on 19/06/23.
//

#include "../include/overflow_management.h"

// ------------------------------
// ------------------------------
// GENERAL PURPOSE
// ------------------------------
// ------------------------------


void node_list_shift(void * list, int new_key_position, int occupancy_rate, List_type type){

    if(type == KEY){
        
        BT_key * key_list = (BT_key *) list;
        for(int i = occupancy_rate; i > new_key_position; i--){
            key_list[i] = key_list[i-1];
        }
    }
    else{
        // in this case, the RRN list has 1 more position than the key list
        // so, the contion and the position acces of the loop has to be
        // adapted;
        int * rrn_list = (int *) list;
        
        for (int i = occupancy_rate; i >= new_key_position; i--) {
            rrn_list[i+1] = rrn_list[i];
        }
    }
}

Overflow_block * overflow_inicializing_interval(Overflow_block * info_block,
     BT_node_t * src_node, int list_ini_position, int list_final_postion){
    
    int access_counter = 0; // control the access index in the node
    
    // passing the first pointer of the list to allign the key and pointer
    // actualization in the loop
    
    info_block->rrn_list[list_ini_position] = src_node->descendants_RRN[access_counter];
    
    while (list_ini_position < list_final_postion) {
        
        info_block->key_list[list_ini_position] =
        src_node->keys[access_counter];
        
        info_block->rrn_list[list_ini_position + 1] =
        src_node->descendants_RRN[access_counter + 1];
        
        access_counter++;
        list_ini_position++;
    }
    
    return info_block;
}

// information limite : posicao em que para de copiar informação : nó ascendido
// size limit : posicao fim de memória : TREE ORDER - 1

BT_node_t * overflow_atualizing_Node(BT_node_t * tree_node, Overflow_block * info_block,
     int inicial_overflowList_id, int information_limit){
    
    int access_counter = 0;
    
    // rebooting the occupancy rate in order to atualize the page
    tree_node->occupancy_rate = 0;
    
    // passing the first pointer of the list to allign the key and pointer
    // atualization in the loop
    
    tree_node->descendants_RRN[access_counter] = info_block->rrn_list[inicial_overflowList_id];
    
    for(int i = inicial_overflowList_id; i < inicial_overflowList_id + 4; i++){
        
        if(i < information_limit){
            
            // while there is still logical information to be transmitted ;
            tree_node->keys[access_counter] = info_block->key_list[i];
            tree_node->descendants_RRN[access_counter + 1] = info_block->rrn_list[i + 1];
            tree_node->occupancy_rate = tree_node->occupancy_rate + 1;
        }else{
            // there is no more logical information to be transmitted, but,
            // we have to assure that the leftover postion in memory is inicialized
            // as NULL representation ;
            tree_node->keys[access_counter].value = -1;
            tree_node->keys[access_counter].byteOffset = -1;
            tree_node->descendants_RRN[access_counter+1] = -1;
        }
        
        access_counter++;
        
    }
    
    return tree_node;
}

// ------------------------------
// ------------------------------
// CRIACAO DE OVERFLOW BLOCK
// ------------------------------
// ------------------------------

/*
 RECEBE:
    Os nós já lidos em memória primária
    O bloco a ser inserido
    A informação se a página irmã está à direita ou à esquerda da página atual
 RETORNA
    A lista com todos os RRNs e outra com todas as Chaves
    A posição do meio = ascendida
    O comprimento da lista
 */
Overflow_block * create_oveflowBlock_3Nodes(BT_key father_key, BT_node_t * insertion_node,
        BT_node_t * sister_node, Insertion_block * block, Path_running sister_direction){
    
    // for storing
    Overflow_block * info_block;
    int interval_limit;
    
    // for searching
    Path_running path;
    int last_accessed_id;
    //int b_search_return;
    
    // building the oveflow block and its lists
    info_block = (Overflow_block *) malloc(sizeof(Overflow_block));
    assert(info_block);
    
    info_block->list_len = 2 + insertion_node->occupancy_rate + sister_node->occupancy_rate;
    
    info_block->key_list = (BT_key *) malloc(sizeof(BT_key) * info_block->list_len);
    assert(info_block->key_list);
    
    info_block->rrn_list = (int *) malloc(sizeof(int) * (info_block->list_len+1));
    assert(info_block->rrn_list);
    
    
    // Associting information in the overflow registry according to the sourcepages order
    if(sister_direction == RIGHT){
        
        info_block = overflow_inicializing_interval(info_block, insertion_node, 0,
             insertion_node->occupancy_rate);
        
        info_block->key_list[insertion_node->occupancy_rate] = father_key;
        
        interval_limit = insertion_node->occupancy_rate + sister_node->occupancy_rate + 1;
        
        info_block = overflow_inicializing_interval(info_block, sister_node,
            insertion_node->occupancy_rate + 1, interval_limit);
        
        
    }else{
        
        info_block = overflow_inicializing_interval(info_block, sister_node, 0,
             sister_node->occupancy_rate);
        
        info_block->key_list[sister_node->occupancy_rate] = father_key;
        
        interval_limit = insertion_node->occupancy_rate + sister_node->occupancy_rate + 1;
        
        info_block = overflow_inicializing_interval(info_block, insertion_node,
            sister_node->occupancy_rate + 1, interval_limit);
    }
    
    // searching the insertion position in the key_list
    key_binary_search(info_block->key_list, 0, info_block->list_len - 2,
                            block->key.value, &last_accessed_id, &path);

    // the last_accessed_id + path represents where the key will be placed
    info_block->inserted_id = (path == LEFT) ? last_accessed_id : last_accessed_id + 1;

    node_list_shift(info_block->key_list, info_block->inserted_id, info_block->list_len - 1, KEY);
    node_list_shift(info_block->rrn_list, info_block->inserted_id, info_block->list_len - 1, RRN);
    
    info_block->key_list[info_block->inserted_id].value = block->key.value;
    info_block->key_list[info_block->inserted_id].byteOffset = block->key.byteOffset;
    info_block->rrn_list[info_block->inserted_id+1] = block->right_RRN;
    
    return info_block;
}


Overflow_block * create_oveflowBlock_1Node(BT_node_t * insertion_node, Insertion_block block){
    
    // for storing
    Overflow_block * info_block;
    
    // for searching
    Path_running path;
    int last_accessed_id;

    // building the list
    info_block = (Overflow_block *) malloc(sizeof(Overflow_block));
    assert(info_block);
    
    info_block->list_len = 1 + insertion_node->occupancy_rate;
    
    info_block->key_list = (BT_key *) malloc(sizeof(BT_key) * info_block->list_len);
    assert(info_block->key_list);
    
    info_block->rrn_list = (int *) malloc(sizeof(int) * (info_block->list_len + 1));
    assert(info_block->rrn_list);
        
    // inserting the node keys and rrn values into the overflow_block key and rrn vectors ;
    
    info_block = overflow_inicializing_interval(info_block, insertion_node,
         0, insertion_node->occupancy_rate);
    
    // searching the insertion position in the key_list
    key_binary_search(info_block->key_list, 0, info_block->list_len - 2,
                            block.key.value, &last_accessed_id, &path);

    // the last_accessed_id + path represents where the key will be placed
    info_block->inserted_id = (path == LEFT) ? last_accessed_id : last_accessed_id + 1;

    node_list_shift(info_block->key_list, info_block->inserted_id, info_block->list_len - 1, KEY);
    node_list_shift(info_block->rrn_list, info_block->inserted_id, info_block->list_len - 1, RRN);
    
    info_block->key_list[info_block->inserted_id] = block.key;
    info_block->rrn_list[info_block->inserted_id + 1] = block.right_RRN;
    
    return info_block;
}


// ------------------------------
// ------------------------------
// REDISTRIBUIÇÃO
// ------------------------------
// ------------------------------

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

BT_node_t * key_redistribuition_decision(BT_node_t * father_node, int overflowed_RRN_id,
     FILE * index_file, Path_running * direction, int * father_key_id){
    
    BT_node_t * node_buffer_left = NULL;
    BT_node_t * node_buffer_right = NULL;
    int left_sister_rrn_id = 0;
    int right_sister_rrn_id = 0;

    // testing if the insertion position is one of the list limits case :
    if(overflowed_RRN_id != 0){
        left_sister_rrn_id = overflowed_RRN_id - 1;
        if(left_sister_rrn_id >= 0){
            node_buffer_left = bt_node_read(index_file,
                 father_node->descendants_RRN[left_sister_rrn_id]);
        }
    }if(overflowed_RRN_id != father_node->occupancy_rate){
        right_sister_rrn_id = overflowed_RRN_id + 1;
        if(right_sister_rrn_id >= 0){
            node_buffer_right = bt_node_read(index_file,
             father_node->descendants_RRN[right_sister_rrn_id]);
        }
    }
    
    // deciding which of the nodes, or none of them, will be targeted by
    // the redistribuition.
    
    if(node_buffer_left != NULL && node_buffer_left->occupancy_rate < TREE_ORDER - 1){

        if(node_buffer_right != NULL) bt_node_delete(&node_buffer_right);
        *direction = LEFT;
        
        // checking if we have to change the father_id :
        if(left_sister_rrn_id < *father_key_id){
            *father_key_id = *father_key_id - 1 ;
        }
        return node_buffer_left;
        
    }else if(node_buffer_right != NULL && node_buffer_right->occupancy_rate < TREE_ORDER - 1){

        if(node_buffer_left != NULL) bt_node_delete(&node_buffer_left);
        *direction = RIGHT;

        // checking if we have to change the father_id :
        if(right_sister_rrn_id > *father_key_id + 1){
            *father_key_id = *father_key_id + 1 ;
        }
        return node_buffer_right;
        
    }else{
        if(node_buffer_right != NULL) bt_node_delete(&node_buffer_right);
        if(node_buffer_left != NULL) bt_node_delete(&node_buffer_left);
        return NULL;
    }
}

/* RECEBE:
    Os nós lidos em memória primária
    A página irmã decidida conforme a funcão acima implementada
    O bloco a ser inserido (Insertion Block)
    A informação sobre a direção da página Irmã
    O id da chave pai dentro do nó pai, obtida na recursão
*/

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
    
    // LIBERATING THE ADRESSED MEMORY
    
    bt_node_delete(&sister_node);
}

// ------------------------------
// ------------------------------
// SPLIT 1 - 2
// ------------------------------
// ------------------------------

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
    new_root->level = root_node->level + 1;
    
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

    bt_node_delete(&sister_node);

    // emptying the insertion block
    block->key.value = -1;
}

// ------------------------------
// ------------------------------
// SPLIT 2 - 3
// ------------------------------
// ------------------------------

BT_node_t * node_Split2_3_decision(BT_node_t * father_node, int overflowed_RRN_id,
     FILE * index_file, Path_running * direction, int * father_key_id){
    
    BT_node_t * node_buffer_left = NULL;
    BT_node_t * node_buffer_right = NULL;
    int left_sister_rrn_id = 0;
    int right_sister_rrn_id = 0;

    // testing if the insertion position is one of the list limits case :
    if(overflowed_RRN_id != 0){
        left_sister_rrn_id = overflowed_RRN_id - 1;
        if(left_sister_rrn_id >= 0){
            node_buffer_left = bt_node_read(index_file, father_node->descendants_RRN[left_sister_rrn_id]);
        }
    }if(overflowed_RRN_id != father_node->occupancy_rate){
        right_sister_rrn_id = overflowed_RRN_id + 1;
        if(right_sister_rrn_id >= 0){
            node_buffer_right = bt_node_read(index_file, father_node->descendants_RRN[right_sister_rrn_id]);
        }
    }
    
    // deciding which of the nodes, or none of them, will be targeted by
    // the redistribuition.
    if(node_buffer_right != NULL){
        if(node_buffer_left != NULL) bt_node_delete(&node_buffer_left);
        *direction = RIGHT;
        
        // checking if we have to change the father_id :
        if(right_sister_rrn_id > *father_key_id + 1){
            *father_key_id = *father_key_id + 1 ;
        }
        return node_buffer_right;
        
    }else if(node_buffer_left != NULL){
        if(node_buffer_right != NULL) bt_node_delete(&node_buffer_right);
        *direction = LEFT;
        
        // checking if we have to change the father_id :
        if(left_sister_rrn_id < *father_key_id){
            *father_key_id = *father_key_id - 1 ;
        }
        
        return node_buffer_left;
            
    }else{
        if(node_buffer_right != NULL) bt_node_delete(&node_buffer_right);
        if(node_buffer_left != NULL) bt_node_delete(&node_buffer_left);
        fprintf(stdout, "NENHUM PONTEIRO DISPONIVEL : FALHA NO SPLIT DECISION\n");
        return NULL;
    }
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
        insertion_node = overflow_atualizing_Node(insertion_node, info_block, 0,
             info_block->ascended_id[0]);
        sister_node = overflow_atualizing_Node(sister_node, info_block,
            info_block->ascended_id[0] + 1, info_block->ascended_id[1]);
        
    }else{
        sister_node = overflow_atualizing_Node(sister_node, info_block, 0,
             info_block->ascended_id[0]);
        insertion_node = overflow_atualizing_Node(insertion_node, info_block,
             info_block->ascended_id[0] + 1, info_block->ascended_id[1]);
    }
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

    fseek(index_file, (insertion_RRN + 1) * DISK_PAGE_SIZE, SEEK_SET);
    bt_node_write(insertion_node, index_file);
    
    fseek(index_file, (sister_RRN + 1) * DISK_PAGE_SIZE, SEEK_SET);
    bt_node_write(sister_node, index_file);
    
    fseek(index_file, (new_RRN + 1) * DISK_PAGE_SIZE, SEEK_SET);
    bt_node_write(new_node, index_file);
    
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
