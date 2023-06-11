/* Vicenzo D'Arezzo Zilio - 13671790 */
/* INFORMACOES JOAO */
/* Files Organization - 2023 */

#include "csvFunctions.h"

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------
// -> SETUP FILES FUNCTIONS:
// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

void create_table(FILE * read_file, FILE * write_file){
    
    // Stating variables ;
    
    Crime_t * crime_reading_pointer = crime_create();
    Header_t * header_writing_pointer = header_create();
    
    char * endl_pointer;
    char line_buffer[BUFSIZ];
    char info_buffer[BUFSIZ];
    int char_counter;
    int field_counter = 0;
    
    // Reading and discarting the header line of the text file;
    fgets(line_buffer, BUFSIZ, read_file);
    
    // Reserving the space in the beggining of the file for the header register, even
    // without the number of register.
    header_byteOffset_increment(header_writing_pointer, header_bin_write(write_file, header_writing_pointer));
    
    
    // The Register reading loop. It reads line per line of the file,
    // storing the position of the end of each line through the strchr function.
    // In the line, compares char per char to the delimiters of the file
    // building an information buffer that is transfered to the association function
    // Finally, we have the information stored in the crime_pointer, that will be
    // written in the binary file by the write function.
    
    // We also verify the first char of the buffer because, in genereal, the most of
    // csv files are ended with '\n' . So we assure the code correctness anda save
    // an interation by doing that
    
    while(fgets(line_buffer, BUFSIZ, read_file) != NULL && line_buffer[0] != '\n'){

        // Stores the ending of the line
        endl_pointer = strchr(line_buffer, (int)'\n');
        
        char_counter = 0;
        field_counter = 0;
        header_regNumber_increment(header_writing_pointer);
        
        // Runs each char until finds the endl adress
        for(char * i = line_buffer; i <= endl_pointer; i++){
            if(*i != ',' && *i != '\n'){
                // Stores the char into the buffer
                info_buffer[char_counter++] = *i;
            }else{
                // uses the \0 just to delimit the buffer, the current string will be trated with the string
                // delimiter in the association function.
                info_buffer[char_counter] = '\0';
                crime_field_association(info_buffer, field_counter++, crime_reading_pointer);
                char_counter = 0;
                memset(info_buffer, 0, sizeof(info_buffer));
            }
        }
        
        memset(line_buffer, 0, sizeof(line_buffer));
        
        // Writing the current crime in the binary file
        header_byteOffset_increment(header_writing_pointer, crime_bin_write(write_file, crime_reading_pointer));
        
        // Cleaning the dynamic alocatted info in the crime reading pointer;
        crime_liberate_dynamic_strings(&crime_reading_pointer);
    }
    
    //rewrite the header file, now with the correct number of registers and the correct status:
    header_writing_pointer->status = '1' ;
    fseek(write_file, 0, SEEK_SET);
    header_bin_write(write_file, header_writing_pointer);

    crime_delete(&crime_reading_pointer);
    header_delete(&header_writing_pointer);
}

//-----------------------------------------------------------------------------------


void create_index(FILE * read_file, char key_str[], FILE * index_file){
    
    Header_t * reading_header = header_create();
    
    int n_registers;
    unsigned long int byte_Offset_counter = 0 ;
    int n_index = 0;
    int key_field;
    int key_type;
    
    //CHECKING THE STATUS OF THE READING FILE :
    
    assert(read_file);
    
    byte_Offset_counter += header_reading(read_file, reading_header);
    n_registers = reading_header->nRegFile;
    
    if(reading_header->status == '0'){
        printf("Falha no processamento do arquivo.\n");
        header_delete(&reading_header);
        return;
    }if(n_registers == 0){
        printf("Registro inexistente.\n");
        header_delete(&reading_header);
        return;
    }
    
    //ASSERTING THE KEY TYPE:
    key_field = index_crimeField_pairing(key_str);
    key_type = (key_field == 0 || key_field == 2) ? Number : String ;
    
    //CREATING THE INDEX LIST :
    
    Index_data_t ** index_list = index_data_reading(read_file, n_registers, &byte_Offset_counter, key_field, &n_index);
    
    //ORDENATE
    index_data_sorting(index_list, key_type, n_index);
    
    //WRITE
    Index_header_t * index_header = index_header_create();
    index_header->status = '0';
    index_header->n_reg_file = n_index;
    
    index_header_writing(index_file, index_header);
    
    index_data_list_writing(index_file, index_list, key_type, n_index);
    
    index_header->status = '1';
    fseek(index_file, 0, SEEK_SET);
    index_header_writing(index_file, index_header);
    
    
    //ENDING
    index_header_delete(&index_header);
    index_data_vec_delete(&index_list, n_index);
   
    return;
}

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------
// -> SEARCH AND SELECT FUNCTIONS
// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

void select_all(FILE * read_file){
    
    //configuring the callBack Function to print all valid registers
    CallBackF executable;
    executable.comparing_flag = false;
    executable.deletion_flag = false;
    executable.print = &(crime_printing);
    
    linear_selection(read_file, &executable);
}

//-----------------------------------------------------------------------------------


void select_from(FILE * data_file, FILE * index_file, char * index_name, int n_search){
    
    int index_id;
    int index_type;
    int search_counter = 0;
    char index_field_info[BUFSIZ];
    bool index_search = false;
    char c;
    Index_data_t ** index_list;
    int index_list_length = 0;
    
    
    // checking the index_type
    index_id = index_crimeField_pairing(index_name);
    if(index_id == 0 || index_id == 2) index_type = Number;
    else index_type = String;
    

    index_list = index_bringing_to_mainMemory(index_file, index_type, &index_list_length);
    
    while(search_counter++ < n_search){
        
        //READING THE SEARCHED FIELDS:
        
        index_search = false;
       
        int n_fields;
        
        fscanf(stdin, "%d", &n_fields);
        
        while (( c = getchar()) != '\n' && c != '\0' && c != ' ');
        
        int searched_fields [n_fields];
        
        Crime_t * crime_filter = crime_create();
        
        crime_filter = input_information_reading_for_searches(index_field_info, &index_search, index_id, searched_fields, n_fields, crime_filter);
        
        // SETTING UP THE CALLBACK FUNCTION REGISTER FOR CONTIONIAL PRINTING:
        
        CallBackF exe;
        exe.comparing_flag = true;
        exe.deletion_flag = false;
        exe.compare = &(crime_numbered_fields_compare);
        exe.print = &(crime_printing);
        exe.filter = crime_filter;
        exe.field_list = searched_fields;
        exe.n_fields = n_fields;
        exe.results_flag = false;
        
        // DIVIDING THE SEARCH POSSIBILITIES
        
        printf("Resposta para a busca %d\n", search_counter);
        
        if(index_search){
            /* SEARCHING BY THE INDEX : */ /* filtering the Index_file by a binary search and acessing the
             data file punctually by the byteOffset, in the access function, the exe call back function
             structure will be executed */
            
            fseek(index_file, 0, SEEK_SET);
            
            index_list = indexed_crime_searching(index_list, index_list_length, data_file, index_type, index_field_info, &exe);
            
        }else{
            /* SEARCHING WITHOUT INDEX : */ /* We will use the select all function to run sequentially
             the data file and, in each of the read Crime, the callBack function will execute a
             a condtional printing based in the comparing flag and the crime filter */
            
            
            fseek(data_file, 0, SEEK_SET);

            linear_selection(data_file, &exe);
            
            if(exe.results_flag == false) printf("Registro inexistente.\n");
        }
        
        crime_delete(&crime_filter);
    }
}


//-----------------------------------------------------------------------------------


void delete_from(FILE * data_file, FILE * index_file, char * index_name, int n_search, char * index_file_name){

    int index_id;
    int index_type;
    int search_counter = 0;
    char index_field_info[BUFSIZ];
    bool index_search = false;
    char c;
    Index_data_t ** index_list;
    int index_list_length = 0;
    
    
    // checking the index_type
    index_id = index_crimeField_pairing(index_name);
    if(index_id == 0 || index_id == 2) index_type = Number;
    else index_type = String;
    

    index_list = index_bringing_to_mainMemory(index_file, index_type, &index_list_length);
    
    // SETTING UP THE CALLBACK FUNCTION REGISTER FOR DELETION:
    CallBackF exe;
    exe.comparing_flag = true;
    exe.deletion_flag = true;
    exe.compare = &(crime_numbered_fields_compare);
    exe.print = &(crime_printing);
    exe.logical_deletion = &(crime_logical_deletion);
    exe.results_flag = false;
    exe.general_counter = 0;
    exe.deleted_data_adress = NULL;
    
    while(search_counter++ < n_search){
        
        //READING THE SEARCHED FIELDS:
        
        index_search = false;
       
        int n_fields;
        
        fscanf(stdin, "%d", &n_fields);
        
        while (( c = getchar()) != '\n' && c != '\0' && c != ' ');
        
        int searched_fields [n_fields];
        
        Crime_t * crime_filter = crime_create();
        
        crime_filter = input_information_reading_for_searches(index_field_info, &index_search, index_id, searched_fields, n_fields, crime_filter);
        
        
        // SETTING UP THE REST OF CALLBACK FUNCTION REGISTER FOR DELETION:
        exe.filter = crime_filter;
        exe.field_list = searched_fields;
        exe.n_fields = n_fields;
        
        
        // DIVIDING THE SEARCH POSSIBILITIES
        
        
        if(index_search){
            
            /* SEARCHING BY THE INDEX : */ /* filtering the Index_file by a binary search and acessing the
             data file punctually by the byteOffset, in the access function, the exe call back function
             structure will be executed, re-writing the register with a new deletion status */
            
            index_list = indexed_crime_searching(index_list, index_list_length, data_file, index_type, index_field_info, &exe);
            
            
        }else{
            /* SEARCHING WITHOUT INDEX : */ /* We will use the select all function to run sequentially
             the data file and, in each of the read Crime, the callBack function will execute a
             a condtional logical deletion based in the comparing flag and the crime filter, re-writing the
             register with a new deletion status */
        
            fseek(data_file, 0, SEEK_SET);
            
            linear_selection(data_file, &exe);
            
        }
        
        crime_delete(&crime_filter);
    }
    
    // Now, int the end of the searchs loop, the Index_list in main memory has the removed
    // register as NULL value, so, we need to just atualize the file information according
    // to the lists valid Index_data Registers
    
    files_maintenance(index_file, data_file, index_file_name, index_list, index_list_length, index_type, exe.general_counter, exe.deleted_data_adress);
    
    free(exe.deleted_data_adress);
}


//--------------------------------------------------
//--------------------------------------------------
// DATA MANIPULATING FUNCTIONS
//--------------------------------------------------
//--------------------------------------------------

void inser_into(FILE * data_file, FILE * index_file, char * index_file_name, char * index_name_info, int n_insertions){
    
    int insertion_counter = 0;
    
    // for the index :
    int index_id;
    int key_type;
    int list_length = 0;
    char index_info[BUFSIZ];
    bool index_flag;
    Index_header_t * index_header = index_header_create();
    
    // for the crime data :
    Header_t * data_header = header_create();
    
    
    // Reading the data registry
    header_reading(data_file, data_header);
    
    if(data_header->status == '0'){
        header_delete(&data_header);
        index_header_delete(&index_header);
        printf("Falha ao abrir o arquivo.");
    }else if(data_header->nRegFile == 0){
        header_delete(&data_header);
        index_header_delete(&index_header);
        printf("Registro inexistente.");
    }
    
    // demonstrainting in file status that data is still being written
    data_header->status = '0';
    fseek(data_file, 0, SEEK_SET);
    header_bin_write(data_file, data_header);
    
    
    // verifying the index
    index_id = index_crimeField_pairing(index_name_info);
    if(index_id == 0 || index_id == 2) key_type = Number;
    else key_type = String;
    Index_data_t ** index_list = index_bringing_to_mainMemory(index_file, key_type, &list_length);
    
    // acessing the last position for the writing
    fseek(data_file, data_header->proxByteOffset, SEEK_SET);
    
    while(insertion_counter ++ < n_insertions){
        
        index_flag = true;
        Crime_t * crime = Crime_insertion_input_reading(index_id, &index_flag, index_info);
        
        // asserting if the crime can be indexed:
        if(index_flag){
            //building the search key
            union key search_key;
            if(key_type == Number) search_key.number = atoi(index_info);
            else index_string_key_association(search_key.string, index_info, index_id);
            //appending the list
            index_list = index_list_append(search_key, data_header->proxByteOffset, index_list, &list_length, key_type);
        }
        
        data_header->proxByteOffset += crime_bin_write(data_file, crime);
        data_header->nRegFile++;
        
        crime_delete(&crime);
    }
    
    //atualizing the files:
    
        //INDEX:
    index_data_sorting(index_list, key_type, list_length);
    fclose(index_file);
        //atualizing the header and re-opening the file to re-write the new ordenated list
    index_header->status = '0';
    index_header->n_reg_file = list_length;
    index_file = fopen(index_file_name, "wb");
    assert(index_file);
    index_header_writing(index_file, index_header);
    index_data_list_writing(index_file, index_list, key_type, list_length);
        //after the list writing, we write again the register, now with the currente status.
    fseek(index_file, 0, SEEK_SET);
    index_header->status = '1';
    index_header_writing(index_file, index_header);
    
        //DATA:
    fseek(data_file, 0, SEEK_SET);
    data_header->status = '1';
    header_bin_write(data_file, data_header);
    
    // libarating the allocated memory
    header_delete(&data_header);
    index_data_vec_delete(&index_list, index_header->n_reg_file);
    index_header_delete(&index_header);
    
}
