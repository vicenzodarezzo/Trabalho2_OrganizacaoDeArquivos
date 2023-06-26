//
//  csv_functions.h
//  bStar_index
//
//  Created by Vicenzo D’Arezzo on 13/06/23.
//

#ifndef csv_functions_h
#define csv_functions_h

#include <stdio.h>
#include "inputOutput.h"

// -------------------------------------------------
// -------------------------------------------------
// -> SETUP FILES FUNCTIONS:
// -------------------------------------------------
// -------------------------------------------------

void data_file_settingStatus(FILE * data_file, char c);

void index_file_settingHeader(FILE * index_file, char status, BT_header_t * header);

void data_file_settingHeader(FILE * data_file, char status, Header_t * header);


//--------------------------------------------------
//--------------------------------------------------
// -> DATA MANIPULATING FUNCTIONS
//--------------------------------------------------
//--------------------------------------------------


// -------------------------------------------------
// -------------------------------------------------
// -> SEARCH FUNCTIONS:
// -------------------------------------------------
// -------------------------------------------------

void select_from(FILE * data_file, FILE * index_file, char * index_name, int n_search);

// -------------------------------------------------
// -------------------------------------------------
// -> INSERTION FUNCTIONS:
// -------------------------------------------------
// -------------------------------------------------

void insert_into(FILE * data_file, FILE * index_file, char * index_name, int n_executions,
     Header_t * data_header);

#endif /* csv_functions_h */
