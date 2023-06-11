//
//  page_buffer.h
//  index_BTree
//
//  Created by Vicenzo D’Arezzo on 11/06/23.
//

#ifndef page_buffer_h
#define page_buffer_h

#include "registers.h"
#include "inputOutput.h"
#include "bTree_index.h"

#define DISK_PAGE_SIZE 76
#define LIST_MAX 50


typedef struct buffer{
    
    FILE * src;
    BT_header_t * header;
    BT_node_t * page_list[LIST_MAX];
    bool modify_list[LIST_MAX];
    int counter;
    
}Page_buffer;

// -----------------------------------
// -----------------------------------
/* READ FUNCTIONS */
// -----------------------------------
// -----------------------------------

/**
 * Opens the File, crating a new buffer registry. As it opens, reads the header of the file
 * asserting it's validity and set it with an invalid status while the reads continue to happen.
 * This function leaves the reading pointer in the position after the first page - header page.
 */
Page_buffer * buffer_openFile(char file_name[]);

bool buffer_isFull (Page_buffer * buffer);

/**
 * Reads from the buffer file the root of the tree, storing it in the first position of the page list.
 * This function has to be the first node read to be called.
 */
Page_buffer * buffer_read_TreeRoot(Page_buffer * buffer);
/**
 * Reads from the buffer file the the scpecific node represented by the RRN, pushing it on the
 * top of the page list.
 */
Page_buffer * buffer_read_point_node(Page_buffer * buffer, int value_RRN);

// -----------------------------------
// -----------------------------------
/* ACCESS FUNCTIONS */
// -----------------------------------
// -----------------------------------


BT_node_t * buffer_get_topNode(Page_buffer * buffer);

BT_node_t * buffer_get_TreeRoot(Page_buffer * buffer);

BT_node_t * buffer_get_point_node(Page_buffer * buffer, int point_value);


// -----------------------------------
// -----------------------------------
/* ERASE FUNCTIONS */
// -----------------------------------
// -----------------------------------

// ERASING : Deleting the current node of the main memory.
// BE CAREFUL : for atualizing it information, the node has to be
//              written before erasing.

Page_buffer * buffer_leaveJustRoot(Page_buffer ** buffer);

Page_buffer * buffer_leaveJust_Root_and_Top(Page_buffer ** buffer);

Page_buffer * buffer_erase(Page_buffer ** buffer);

void buffer_delete(Page_buffer ** buffer);

#endif /* page_buffer_h */
