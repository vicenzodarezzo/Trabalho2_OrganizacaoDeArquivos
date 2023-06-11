//
//  page_buffer.c
//  index_BTree
//
//  Created by Vicenzo D’Arezzo on 11/06/23.
//

#include "page_buffer.h"


// -----------------------------------
// -----------------------------------
/* READ FUNCTIONS */
// -----------------------------------
// -----------------------------------


Page_buffer * buffer_openFile(char file_name[]){
    
    Page_buffer * bff = (Page_buffer *) malloc(sizeof(Page_buffer));
    assert(bff);
    
    FILE * src_file = fopen(file_name, "r+");
    assert(src_file);
    
    bff->header = bt_header_read(src_file);
    
    if(bff->header->status == '0'){
        fprintf(stdout, "Falha no processamento do arquivo.\n");
        exit(0);
    }
    
    // CHANGING THE FILE STATUS FOR MANIPULATION :
    bff->header->status = '0';
    fseek(bff->src, 0, SEEK_SET);
    bt_header_write(bff->header, bff->src);
    bff->counter = 0;
    bff->header = bt_header_create();
    
    for(int i = 0; i < LIST_MAX; i++){
        bff->page_list[i] = NULL;
        bff->modify_list[i] = false;
    }
    
    return bff;
}

bool buffer_isFull (Page_buffer * buffer){
    return buffer->counter == LIST_MAX - 1 ? true : false ;
}

Page_buffer * buffer_read_TreeRoot(Page_buffer * buffer){
    
    if(buffer->counter != 0){
        fprintf(stdout, "INVALID OPERATION : the root has to be the first read node\n");
        exit(1);
    }
    
    buffer->page_list[0] = bt_node_read(buffer->src, buffer->header->root_RRN);
    buffer->counter++;
    
    printf("BF READ ROOT: raiz lida\n");
    
    return buffer;
}

Page_buffer * buffer_read_point_node(Page_buffer * buffer, int value_RRN){
    
    if(!buffer_isFull(buffer)){
        
        buffer->page_list[buffer->counter] = bt_node_read(buffer->src, value_RRN);
        buffer->counter++;
        
    }else{
        fprintf(stdout, "INVALID OPERATION : full buffer\n");
        exit(1);
    }
    
    return buffer;
}

// -----------------------------------
// -----------------------------------
/* ACCESS FUNCTIONS */
// -----------------------------------
// -----------------------------------

BT_node_t * buffer_get_topNode(Page_buffer * buffer){
    return buffer->page_list[buffer->counter - 1];
}

BT_node_t * buffer_get_TreeRoot(Page_buffer * buffer){
    return buffer->page_list[0];
}

BT_node_t * buffer_get_point_node(Page_buffer * buffer, int point_value){
    if(point_value >= buffer->counter){
        fprintf(stdout, "INVALID OPERATION : POINT VALUE TOO LARGE\n");
        exit(1);
    }
    return buffer->page_list[point_value];
}


// -----------------------------------
// -----------------------------------
/* ERASE FUNCTIONS */
// -----------------------------------
// -----------------------------------

Page_buffer * buffer_leaveJustRoot(Page_buffer ** buffer){
    
    Page_buffer * b = *buffer;
    
    while(b->counter > 1){
        bt_node_delete(&b->page_list[--b->counter]);
    }
    
    return b;
}

Page_buffer * buffer_leaveJust_Root_and_Top(Page_buffer ** buffer){
    
    Page_buffer * b = *buffer;
    
    // swapping the top with the id 1 node in the list:
    BT_node_t * temp = buffer_get_topNode(b);
    b->page_list[b->counter - 1] = buffer_get_point_node(b, 1);
    b->page_list[1] = temp;
    
    while(b->counter > 2){
        bt_node_delete(&b->page_list[--b->counter]);
    }
    
    return b;
}

Page_buffer * buffer_erase(Page_buffer ** buffer){
    
    Page_buffer * b = *buffer;
    
    while(b->counter > 0){
        bt_node_delete(&b->page_list[--b->counter]);
    }
    
    return b;
}

void buffer_delete( Page_buffer ** buffer){
    *buffer = buffer_erase(buffer);
    free(*buffer);
    *buffer = NULL;
}

