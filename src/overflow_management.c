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
    
    printf("SHIFTANDO UMA LISTA\n");
    
    if(type == KEY){
        
        printf("TIPO KEY\n");
        
        BT_key * key_list = (BT_key *) list;
        for(int i = occupancy_rate; i > new_key_position; i--){
            key_list[i] = key_list[i-1];
        }
    }else{
        
        printf("TIPO INT\n");
        
        // in this case, the RRN list has 1 more position than the key list
        // so, the contion and the position acces of the loop has to be
        // adapted;
        int * rrn_list = (int *) list;
        for (int i = occupancy_rate; i >= new_key_position; i--) {
            rrn_list[i+2] = rrn_list[i+1];
        }
    }
}

Overflow_block * overflow_inicializing_interval(Overflow_block * info_block,
     BT_node_t * src_node, int list_ini_position, int list_final_postion){
    
    int access_counter = 0; // control the access index in the node
    
    // passing the first pointer of the list to allign the key and pointer
    // atualization in the loop
    
    info_block->rrn_list[list_ini_position] = src_node->descendants_RRN[access_counter++];
    
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
     int inicial_overflowList_id, int information_limit, int size_limit){
    
    //PRINT PARA DEBUGAR;
    printf("ANTES:\n");
    print_no(tree_node);
    
    int access_counter = 0;
    
    // rebooting the occupancy rate in order to atualize the page
    tree_node->occupancy_rate = 0;
    
    // passing the first pointer of the list to allign the key and pointer
    // atualization in the loop
    
    tree_node->descendants_RRN[access_counter] = info_block->rrn_list[inicial_overflowList_id];
    
    for(int i = inicial_overflowList_id; i < size_limit; i++){
        
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
    
    //PRINT PARA DEBUGAR;
    printf("DEPOIS:\n");
    print_no(tree_node);
    
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
        BT_node_t * sister_node, Insertion_block block, Path_running sister_direction){
    
    // for storing
    Overflow_block * info_block;
    int interval_limit;
    
    // for searching
    Path_running path;
    int last_accessed_id;
    int b_search_return;
    
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
    b_search_return = key_binary_search(info_block->key_list, 0, info_block->list_len - 1,
                            block.key.value, &last_accessed_id, &path);

    // the last_accessed_id + path represents where the key will be placed
    info_block->inserted_id = (path == LEFT) ? last_accessed_id : last_accessed_id + 1;

    node_list_shift(info_block->key_list, info_block->inserted_id, info_block->list_len - 1, KEY);
    node_list_shift(info_block->rrn_list, info_block->inserted_id, info_block->list_len - 1, RRN);
    
    info_block->key_list[info_block->inserted_id] = block.key;
    info_block->rrn_list[info_block->inserted_id+1] = block.right_RRN;
    
    return info_block;
}


Overflow_block * create_oveflowBlock_1Node(BT_node_t * insertion_node, Insertion_block block){
    
    // for storing
    Overflow_block * info_block;
    
    // for searching
    Path_running path;
    int last_accessed_id;
    int b_search_return;

    // building the list
    info_block = (Overflow_block *) malloc(sizeof(Overflow_block));
    assert(info_block);
    
    info_block->list_len = 1 + insertion_node->occupancy_rate;
    
    info_block->key_list = (BT_key *) malloc(sizeof(BT_key) * info_block->list_len);
    assert(info_block->key_list);
    
    info_block->rrn_list = (int *) malloc(sizeof(int) * (info_block->list_len+1));
    assert(info_block->rrn_list);
        
    // inserting the node keys and rrn values into the overflow_block key and rrn vectors ;
    
    info_block = overflow_inicializing_interval(info_block, insertion_node,
         0, insertion_node->occupancy_rate);
    
    // searching the insertion position in the key_list
    b_search_return = key_binary_search(info_block->key_list, 0, info_block->list_len - 1,
                            block.key.value, &last_accessed_id, &path);

    // the last_accessed_id + path represents where the key will be placed
    info_block->inserted_id = (path == LEFT) ? last_accessed_id : last_accessed_id + 1;

    node_list_shift(info_block->key_list, info_block->inserted_id, info_block->list_len - 1, KEY);
    node_list_shift(info_block->rrn_list, info_block->inserted_id, info_block->list_len - 1, RRN);
    
    info_block->key_list[info_block->inserted_id] = block.key;
    info_block->rrn_list[info_block->inserted_id+1] = block.right_RRN;
    
    // calculating the mid_value of the list
    info_block->ascended_id[0] = info_block->list_len / 2 ;
    
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
     FILE * index_file, Path_running * direction){
    
    BT_node_t * node_buffer_left = NULL;
    BT_node_t * node_buffer_right = NULL;
    int sister_rrn;

    // testing if the insertion position is one of the list limits case :
    if(overflowed_RRN_id != 0){
        sister_rrn = overflowed_RRN_id - 1;
        if(sister_rrn >= 0){
            node_buffer_left = bt_node_read(index_file, sister_rrn);
        }
    }else if(overflowed_RRN_id != TREE_ORDER - 1){
        sister_rrn = overflowed_RRN_id + 1;
        if(sister_rrn >= 0){
            node_buffer_right = bt_node_read(index_file, sister_rrn);
        }
    }
    
    // deciding which of the nodes, or none of them, will be targeted by
    // the redistribuition.
    if(node_buffer_left != NULL && node_buffer_left->occupancy_rate < TREE_ORDER - 1){
        
        if(node_buffer_right != NULL) bt_node_delete(&node_buffer_right);
        *direction = LEFT;
        return node_buffer_left;
        
    }else if(node_buffer_right != NULL && node_buffer_right->occupancy_rate < TREE_ORDER - 1){
        
        if(node_buffer_left != NULL) bt_node_delete(&node_buffer_left);
        *direction = RIGHT;
        return node_buffer_right;
        
    }else{
        if(node_buffer_right != NULL) bt_node_delete(&node_buffer_right);
        if(node_buffer_left != NULL) bt_node_delete(&node_buffer_left);
        return NULL;
    }
}


// ------------------------------
// ------------------------------
// SPLIT 2 - 3
// ------------------------------
// ------------------------------

BT_node_t * node_Split2_3_decision(BT_node_t * father_node, int overflowed_RRN_id,
     FILE * index_file, Path_running * direction){
    
    BT_node_t * node_buffer_left = NULL;
    BT_node_t * node_buffer_right = NULL;
    int sister_rrn;

    // testing if the insertion position is one of the list limits case :
    if(overflowed_RRN_id != 0){
        sister_rrn = overflowed_RRN_id - 1;
        if(sister_rrn >= 0){
            node_buffer_left = bt_node_read(index_file, sister_rrn);
        }
    }else if(overflowed_RRN_id != TREE_ORDER - 1){
        sister_rrn = overflowed_RRN_id + 1;
        if(sister_rrn >= 0){
            node_buffer_right = bt_node_read(index_file, sister_rrn);
        }
    }
    
    // deciding which of the nodes, or none of them, will be targeted by
    // the redistribuition.
    if(node_buffer_right != NULL){
        if(node_buffer_left != NULL) bt_node_delete(&node_buffer_left);
        *direction = RIGHT;
        return node_buffer_right;
        
    }else if(node_buffer_left != NULL){
        if(node_buffer_right != NULL) bt_node_delete(&node_buffer_right);
        *direction = LEFT;
        return node_buffer_left;
            
    }else{
        if(node_buffer_right != NULL) bt_node_delete(&node_buffer_right);
        if(node_buffer_left != NULL) bt_node_delete(&node_buffer_left);
        fprintf(stdout, "NENHUM PONTEIRO DISPONIVEL : FALHA NO SPLIT DECISION\n");
        return NULL;
    }
}
