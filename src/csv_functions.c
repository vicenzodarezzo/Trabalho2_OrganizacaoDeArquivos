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

void index_file_settingHeader(FILE * index_file, char status, BT_header_t * header){
    
    if(status == '0' && header->status == '0'){
        fprintf(stdout, "Falha no processamento do arquivo.\n");
        exit(0);
    }
    
    header->status = status;
    fseek(index_file, 0, SEEK_SET);
    
    bt_header_write(header, index_file);
}

void data_file_settingHeader(FILE * data_file, char status, Header_t * header){
    
    if(status == '0' && header->status == '0'){
        fprintf(stdout, "Falha no processamento do arquivo.\n");
        exit(0);
    }
    
    header->status = status;
    fseek(data_file, 0, SEEK_SET);
    header_bin_write(data_file, header);
}



//--------------------------------------------------
//--------------------------------------------------
// -> DATA MANIPULATING FUNCTIONS
//--------------------------------------------------
//--------------------------------------------------

void create_index(FILE * data_file, FILE * index_file, char * index_name, Header_t * data_header) {

    // binary tree index setup
    BTree * tree = bt_index_create(index_file);

    // Checking the indexed value
    if(index_crimeField_pairing(index_name) != INDEX_IDENTIFIER){
        fprintf(stdout, "The indexed value isn't the primary key -> NOT POSSIBLE TO INSERT INTO THE INDEX TREE\n");
        return;
    }

    // creating the auxiliary crime variables used to store and insert the crime from the database
    Crime_t * inserted_crime = crime_create();

    // setting the byteOffset to the first crime on the database
    long long int current_byteOffset = sizeof(Header_t);

    // insertion loop, assumes every crime has the indexed value idCrime
    long long int inserted_crime_size;
    for(int i = 1; i <= data_header->nRegFile; ++i) {

        //printf("CRIME %d\n", i);
        inserted_crime_size = crime_reading(data_file, inserted_crime);

        //crime_printing(inserted_crime);

        if(inserted_crime->removed == 1) { 
            // if it was removed, it must not be inserted into the index file
            crime_liberate_dynamic_strings(&inserted_crime);
            current_byteOffset += inserted_crime_size;
            continue;
        }
        
        // insertion into the binary_tree
        BT_key inserted_key;
        inserted_key.value = inserted_crime->idCrime;
        inserted_key.byteOffset = current_byteOffset;
        bTree_id_insertion(tree, inserted_key);

        crime_liberate_dynamic_strings(&inserted_crime);
        current_byteOffset += inserted_crime_size;
    }
    
    crime_delete(&inserted_crime);
    bTree_closing(&tree);
}

// -------------------------------------------------
// -------------------------------------------------
// -> SEARCH FUNCTIONS:
// -------------------------------------------------
// -------------------------------------------------

void select_from(FILE * data_file, FILE * index_file, char * index_name, int n_executions){
    
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
    
    while(search_counter++ < n_executions){
        
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

void insert_into(FILE * data_file, FILE * index_file, char * index_name, int n_executions,
     Header_t * data_header) {
    
    // building the index tree on primary memory
    BTree * tree = bTree_initializing(index_file);

    // Checking the indexed value
    if(index_crimeField_pairing(index_name) != INDEX_IDENTIFIER){
        fprintf(stdout, "The indexed value isn't the primary key -> NOT POSSIBLE TO INSERT INTO THE INDEX TREE\n");
        bTree_closing(&tree);
        return;
    }

    // creating the auxiliary crime variables used to store and insert the crime from input
    Crime_t * inserted_crime;
    bool index_flag; //used to determine if the inserted crime has information on the indexed field
    char index_info[BUFSIZ];

    // since its an insertion, the only possibility is writing at the end of the data_file
    fseek(data_file, data_header->proxByteOffset, SEEK_SET);
    
    for(int i = 1; i <= n_executions; ++i){
        inserted_crime = Crime_insertion_input_reading(INDEX_IDENTIFIER, &index_flag, index_info);

        // the following tests consider that the index_info is the idCrime, so the inserted crime must have it
        if(!index_flag) {
            fprintf(stdout, "The inserted register does not have the indexed field -> NOT POSSIBLE TO INSERT INTO THE INDEX TREE\n");
            crime_liberate_dynamic_strings(&inserted_crime);
            crime_delete(&inserted_crime);
            continue;
        }

        // index insertion values setup
        int inserted_value = atoi(index_info);
        //printf("\n\n inserted value: %d\n", inserted_value);
        long long int inserted_byteOffset = data_header->proxByteOffset;
        
        //printf("BYTEOFFSET: %lld\n", inserted_byteOffset);
        
        // insertion of the inserted crime into the index tree
        BT_key inserted_key;
        inserted_key.value = inserted_value;
        inserted_key.byteOffset = inserted_byteOffset;
        bool index_insertion_success = bTree_id_insertion(tree, inserted_key);

        if(index_insertion_success) {
            // inserting the read crime into the binary file
            long long int counter;
            counter = crime_bin_write(data_file, inserted_crime);
            
            // atualizing the information in the header registry
            data_header->proxByteOffset = data_header->proxByteOffset + counter;
            data_header->nRegFile = data_header->nRegFile + 1;
        }
        crime_liberate_dynamic_strings(&inserted_crime);
        crime_delete(&inserted_crime);
    }

    bTree_closing(&tree);
}
