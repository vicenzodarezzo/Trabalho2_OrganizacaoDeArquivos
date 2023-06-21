//
//  csv_functions.c
//  bStar_index
//
//  Created by Vicenzo D’Arezzo on 13/06/23.
//

#include "../include/csv_functions.h"

// -------------------------------------------------
// -------------------------------------------------
// -> SETUP FILES FUNCTIONS:
// -------------------------------------------------
// -------------------------------------------------

void data_file_settingStatus(FILE * data_file, char c){
    
    Header_t * header = header_create();
    header_reading(data_file, header);
    
    // verifying if the program is trying to inicialize a
    // inconsistent file from second memory
    if(c == '0' && header->status == '0'){
        fprintf(stdout, "Falha no processamento do arquivo.\n");
        exit(0);
    }
    
    header->status = c;
    fseek(data_file, 0, SEEK_SET);
    header_bin_write(data_file, header);
    header_delete(&header);
}


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

void select_from(FILE * data_file, FILE * index_file, char * index_name, int n_search){
    
    BTree * tree = bTree_initializing(index_file);
    
    int search_counter = 0;
    bool indexed_search;
    int n_fields;
    long long int byte_offSet;
    
    // Checking the indexed value
    if(index_crimeField_pairing(index_name) != INDEX_IDENTIFIER){
        fprintf(stdout, "The indexed value isn't the primary key -> ALL LINEAR SEARCHS\n");
    }
    
    indexed_search = (index_crimeField_pairing(index_name) == INDEX_IDENTIFIER);
    
    while(search_counter++ < n_search){
        
        //BUILDING THE SEARCH FILTER :
        
        // -> number of described fields
        fscanf(stdin, " %d", &n_fields);
        
        fflush(stdout);
        
        int searched_fields [n_fields];
        Crime_t * crime_filter = crime_create();
        
        crime_filter = input_information_reading_for_searches(&indexed_search, searched_fields,
            n_fields, crime_filter);
        
        //SETTING UP THE CALLBACK FUNCTION REGISTER FOR CONTIONIAL PRINTING:
        
        CallBackF exe;
            exe.comparing_flag = true;
            exe.deletion_flag = false;
            exe.compare = &(crime_numbered_fields_compare);
            exe.print = &(crime_printing);
            exe.filter = crime_filter;
            exe.field_list = searched_fields;
            exe.n_fields = n_fields;
            exe.results_flag = false;
        
        fprintf(stdout, "Resposta para a busca %d\n", search_counter);
        
        if(indexed_search){
            /* SEARCHING BY THE INDEX : */
            // doing the search in the levels of the tree. The ACESSING constant passed as a paramenter
            // will set the running to return the byteOffset of the key in the final node reached.
            // In the point_access function, the crime, if not logically removed, will be printed
            // in the standard output as requested by the executable registry.
            
            byte_offSet = bTree_id_search(tree->index_file, tree->root, crime_filter->idCrime);
            
            if(byte_offSet != -1){
                byteOffset_point_access((unsigned long int) byte_offSet, data_file, &exe);
            }
            
        }else{
            
            /* SEARCHING WITHOUT INDEX : */
            // we will use the select all function to run sequentially
            // the data file and, in each of the read Crime, the callBack function will execute a
            // a condtional printing based in the comparing flag and the crime filter.
            
            fseek(data_file, 0, SEEK_SET);
            linear_selection(data_file, &exe);

        }
        
        if(exe.results_flag == false) fprintf(stdout, "Registro inexistente.\n");
        crime_delete(&crime_filter);
    }
    
    //deleting the tree
    bTree_closing(&tree);
}

// -------------------------------------------------
// -------------------------------------------------
// -> INSERTION FUNCTIONS:
// -------------------------------------------------
// -------------------------------------------------
