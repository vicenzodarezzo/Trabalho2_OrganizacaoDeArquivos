/* Vicenzo D'Arezzo Zilio (80%) - 13671790 */
/* Marina Souza Figueiredo (20%) - 13671827 */
/* File Organization - 2023 */

#include "index.h"

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------
// SETUP FUNCTIONS :
// -------------------------------------------------------------------------
// -------------------------------------------------------------------------


Index_header_t  * index_header_create(void){
    Index_header_t * header = (Index_header_t *) malloc (sizeof(Index_header_t));
    assert(header);
    header->status = '0' ;
    header->n_reg_file = 0 ;
    
    return header;
}

//-----------------------------------------------------------------------------------

void index_header_delete(Index_header_t   ** header){
    assert(*header) ;
    free(*header) ;
    *header = NULL ;
}

//-----------------------------------------------------------------------------------

Index_data_t ** index_dataList_create(int length){
    Index_data_t ** list = (Index_data_t **) malloc(sizeof(Index_data_t *) * length);
    assert(list);
    for(int i = 0; i < length; i++){
        list[i] = (Index_data_t *) malloc(sizeof(Index_data_t));
        assert(list[i]);
    }
    return list;
}

//-----------------------------------------------------------------------------------

void index_data_vec_delete(Index_data_t *** data, int length){
    assert(*data) ;
    for(int i = 0; i < length ; i++){
        free((*data)[i]);
    }
    free(*data) ;
    *data = NULL ;
}

//-----------------------------------------------------------------------------------


void files_maintenance(FILE * index_file, FILE * data_file, char * index_file_name, Index_data_t ** index_list, int n_index, int index_type, int rem_counter, unsigned long int * deleted_data_adress){
    
    // INDEX FILE
        // We have to first close the current index file to be able to rewrite it from the
        // beggining with de "wb" C oppening mode through fopen.
    
    assert(index_file);
    fclose(index_file);
    
    index_file = (FILE *) fopen(index_file_name, "wb");
    assert(index_file);
    
        //First we build the processing header register and write it for saving it space
    Index_header_t * id_header = index_header_create();
    Header_t * dt_header = header_create();
    
    id_header->status = '0';
    id_header->n_reg_file = 0;
    int valid_registers = 0;
    bool writing_flag;
    
    index_header_writing(index_file, id_header);
    
    bool number_flag = index_type == Number ? true : false;
    
        // writing the valid registers
    for(int i = 0; i < n_index; i++){
        
        writing_flag = true;
        
        //asserting if the byteOffset matches one of the removed registers
        for(int j = 0; j < rem_counter && writing_flag; j++){
            if(index_list[i]->byte_Offset == deleted_data_adress[j]) writing_flag = false;
        }
        
        if(writing_flag){
            valid_registers++;
            index_data_writing(index_file, index_list[i], number_flag);
        }
        
    }
    
        //Re-writing the index headers, now with the new status and register numbers
    
    fseek(index_file, 0, SEEK_SET);
    id_header->status = '1';
    id_header->n_reg_file = valid_registers;
    index_header_writing(index_file, id_header);
    
    // DATA FILE
    
        // in the data file, we need to atualize the header register with the new
        // removed registers number;
    
    fseek(data_file, 0, SEEK_SET);
    header_reading(data_file, dt_header);
    dt_header->nRegRem += rem_counter;
    fseek(data_file, 0, SEEK_SET);
    header_bin_write(data_file, dt_header);
}

Index_data_t ** index_list_append(union key search_key, unsigned long int byteOffset, Index_data_t ** index_list, int * list_length, int key_type){
    index_list = (Index_data_t **) realloc(index_list, sizeof(Index_data_t *) * (*list_length + 1));
    assert(index_list);
    index_list[*list_length] = (Index_data_t *) malloc(sizeof(Index_data_t));
    assert(index_list[*list_length]);
    
    if(key_type == Number) index_list[*list_length]->search_key.number = search_key.number;
    else{
        strncpy(index_list[*list_length]->search_key.string, search_key.string, 12);
        index_list[*list_length]->search_key.string[12] = '\0';
        
    }
    
    index_list[*list_length]->byte_Offset = byteOffset;
    
    (*list_length)++;
    return index_list;
}

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------
// SORTING FUNCTIONS
// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

int index_data_compare_numberKey(Index_data_t * a, Index_data_t * b){
    int returned_value;
    // Comparing the key
    returned_value = a->search_key.number - b->search_key.number;
    
    return returned_value ;
}


//-----------------------------------------------------------------------------------


int index_data_compare_stringKey(Index_data_t * a, Index_data_t * b){
    int returned_value;
    
    // Comparing the key
    returned_value = strncmp(a->search_key.string, b->search_key.string, 12);
    
    return returned_value ;
}

//-----------------------------------------------------------------------------------


void index_data_swap(Index_data_t * a, Index_data_t * b, int key_type){
    
    unsigned long int b_buffer = a->byte_Offset;
    
    if(key_type == Number){
        int n_buffer = a->search_key.number ;
        a->search_key.number = b->search_key.number ;
        b->search_key.number = n_buffer ;
    }else{
        char str_buffer[13];
        memmove(str_buffer, a->search_key.string, strlen(a->search_key.string)+1);
        memmove(a->search_key.string, b->search_key.string, strlen(b->search_key.string)+1);
        memmove(b->search_key.string, str_buffer, strlen(str_buffer)+1);
    }
    
    a->byte_Offset = b->byte_Offset;
    b->byte_Offset = b_buffer;
    
}

//-----------------------------------------------------------------------------------


void heapifyMax(Index_data_t ** vec, int length, int current_node, int key_type){
 
    // Initialize largest as root
    int greatest = current_node;
 
    // Tree branchs :
    int l_branch = 2 * current_node + 1;
    int r_branch = 2 * current_node + 2;
    bool atualizing_flag = false;
 
    // verifying if the branchs excedes the greatest node
    
    if (l_branch < length && ((key_type == Number ? index_data_compare_numberKey(vec[l_branch], vec[greatest]) : index_data_compare_stringKey(vec[l_branch], vec[greatest])) > 0)){
        greatest = l_branch;
        atualizing_flag = true;
    }
 
    if (r_branch < length && ((key_type == Number ? index_data_compare_numberKey(vec[r_branch], vec[greatest]) : index_data_compare_stringKey(vec[r_branch], vec[greatest])) > 0)){
        greatest = r_branch;
        atualizing_flag = true;
    }
 
    // swapping, if it is necessary
    if (atualizing_flag) {
        
        index_data_swap(vec[greatest], vec[current_node], key_type);
        
        //applying the recursion on the branch tree
        heapifyMax(vec, length, greatest, key_type);
    }
}

//-----------------------------------------------------------------------------------


void heap_sort(Index_data_t ** vec, int length, int key_type){
    
    //converting the vector to a heap:
    for(int i = length / 2 - 1; i >= 0; i--) heapifyMax(vec, length, i, key_type);
    
    //sorting:
    for(int i = length - 1; i >= 0; i--){
        //putting the greatest in the end of the vector
        index_data_swap(vec[0], vec[i], key_type);
        //atualizing the heap
        heapifyMax(vec, i, 0, key_type);
    }
}


//-----------------------------------------------------------------------------------


// Uses the HeapSort implemented in a sequencial vector to ordenate the Index data registers list
void index_data_sorting(Index_data_t ** vec, int key_type, int n_index){
    heap_sort(vec, n_index, key_type);
}


//-----------------------------------------------------------------------------------



void index_list_printing(Index_data_t **vec, int key_type, int n_index){
    
    printf("\n\n IMPRIMINDO \n\n");
    
    for(int i = 0; i < n_index; i++){
        if(key_type == Number){
            printf("REG : %d / %lu \n", vec[i]->search_key.number, vec[i]->byte_Offset);
        }else{
            printf("REG : %s / %lu \n", vec[i]->search_key.string, vec[i]->byte_Offset);
        }
    }
}

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------
// READING FUNCTIONS :
// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------------


void index_string_key_association(char * destiny, char * info, int key_field){
    int char_counter = 0;
    if(key_field == 1){
        char_counter = 10;
        strncpy(destiny, info, char_counter);
    }else if(key_field == 5){
        char_counter = 12;
        strncpy(destiny, info, char_counter);
    }else{
        for(char_counter = 0; info[char_counter] != '\0' && char_counter < 12; char_counter++){
            destiny[char_counter] = info[char_counter];
        }
    }
    destiny[char_counter] = '\0';
}


//-----------------------------------------------------------------------------------


Index_data_t ** index_data_reading(FILE * read_file, int n_reg, unsigned long int * byte_Offset, int key_field, int * n_index){
    
    //Construction variables
    Index_data_t ** index_vec = NULL;
    int reg_counter = * n_index;
    unsigned long int byte_counter = *byte_Offset; // The counter will menages the counting of it
    unsigned long int byte_indicator = 0; // The indicator will store the last counte to desgnate it for the next register;
    
    //Buffer varibles
    Crime_t * crime_reading_pointer = crime_create();
    int field_indicator;
    bool index_validation_flag;
    
    //READING THE FILE
    for(int i = 0; i < n_reg; i++){
        
        union key key_info;
        
        //reading the currente file:
        byte_indicator = byte_counter;
        byte_counter += crime_reading(read_file, crime_reading_pointer);
        index_validation_flag = false;
        
        //verifying if the register can be transmited to the index
        if(crime_reading_pointer->removed == '0'){
            
            switch(key_field){
                // in each of the cases, the information wil be transmited to the buffer union
                    
                case 0 :
                    // crime_id never will be null in our data set
                    key_info.number = crime_reading_pointer->idCrime;
                    field_indicator = Number;
                    index_validation_flag = true;
                    
                    break;
                    
                case 1 :
                    if(crime_reading_pointer->crimeDate[0] != TRASH_IDENTIFIER){
                        index_string_key_association(key_info.string, crime_reading_pointer->crimeDate, key_field);
                        field_indicator = String;
                        index_validation_flag = true;
                    }
                    
                    break;
                    
                case 2 :
                    if(crime_reading_pointer->nArticle != -1){
                        key_info.number = crime_reading_pointer->nArticle ;
                        field_indicator = Number;
                        index_validation_flag = true;
                    }
                    
                    break;
                    
                case 3 :
                    
                    if(crime_reading_pointer->crimePlace[0] != '\0'){
                        index_string_key_association(key_info.string, crime_reading_pointer->crimePlace, key_field);
                        field_indicator = String;
                        index_validation_flag = true;
                    }
                    
                    break;
                    
                case 4 :
                    
                    if(crime_reading_pointer->crimeDescription[0] != '\0'){
                        index_string_key_association(key_info.string, crime_reading_pointer->crimeDescription, key_field);
                        field_indicator = String;
                        index_validation_flag = true;
                    }
                    
                    break;
                    
                case 5 :
                    if(crime_reading_pointer->phoneBrand[0] != TRASH_IDENTIFIER){
                        index_string_key_association(key_info.string, crime_reading_pointer->phoneBrand, key_field);
                        field_indicator = String;
                        index_validation_flag = true;
                    }
                    break;
                    
                default:
                    printf("Error maping input string with a field");
                    break;
            }
        }
        
        //Passing the information to new node in the index list, if the read register is valid
        if(index_validation_flag){
            //creating a new space in the index list
            index_vec = (Index_data_t **) realloc(index_vec, (reg_counter + 1) * sizeof(Index_data_t *));
            assert(index_vec);
            index_vec[reg_counter] = (Index_data_t *) malloc(sizeof(Index_data_t));
            assert(index_vec[reg_counter]);
            
            //Passing the right information to the node
            index_vec[reg_counter]->byte_Offset = byte_indicator;
            index_vec[reg_counter]->search_key = key_info;
            reg_counter++;
        }
    }
    
    *n_index = reg_counter;
    *byte_Offset = byte_counter;
    return index_vec ;
}

// -------------------------------------------------------------------------

Index_header_t * index_header_reading(FILE * index_file, Index_header_t * header){
    fread(&(header->status), sizeof(char), 1, index_file);
    fread(&(header->n_reg_file), sizeof(int), 1, index_file);
    return header;
}

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------
// WRITING FUNCTIONS :
// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

void index_string_writing(char * str, FILE * f){
    
    int char_counter;
    
    //asserting the right disposition of the TRASH_IDENTIFIER in the strings
    for(char_counter = 0; str[char_counter] != '\0' && char_counter < 12; char_counter++);
    while(char_counter < 12) str[char_counter++] = TRASH_IDENTIFIER;
    str[char_counter] = '\0';
    
    fwrite(str, sizeof(char), 12, f);
}

//-----------------------------------------------------------------------------------


void index_header_writing(FILE * f, Index_header_t * header){
    //Writing the header file:
    fwrite(&(header->status), sizeof(char), 1, f);
    fwrite(&(header->n_reg_file), sizeof(int), 1, f);
}

//-----------------------------------------------------------------------------------


void index_data_writing(FILE * f, Index_data_t * data_reg, bool number_key_flag){
    
    if(number_key_flag) fwrite(&(data_reg->search_key.number), sizeof(int), 1, f);
    else index_string_writing(data_reg->search_key.string, f);
    
    fwrite(&(data_reg->byte_Offset), sizeof(unsigned long int), 1, f);
}

//-----------------------------------------------------------------------------------


void index_data_list_writing(FILE * f, Index_data_t ** data_vec, int key_type, int n_reg_file){
    
    bool number_flag;
    
    //Writing the index data vector:
    number_flag = key_type == Number ? true : false ;
    
    for(int i = 0; i < n_reg_file && data_vec[i] != NULL; i++) index_data_writing(f, data_vec[i], number_flag);
}

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------
// SEARCHING FUNCTIONS:
// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

Index_data_t ** index_bringing_to_mainMemory(FILE * index_file, int key_type, int * list_length){
    
    Index_header_t * header = index_header_create();
    fread(&(header->status), sizeof(char), 1, index_file);
    fread(&(header->n_reg_file), sizeof(int), 1, index_file);
    
    if(header->status == 0){
        printf("Falha no processamento do arquivo.c\n");
        index_header_delete(&header);
        return NULL;
    }
    
    Index_data_t ** index_list = index_dataList_create(header->n_reg_file);
    for(int i = 0; i < header->n_reg_file; i++){
        if(key_type == Number) fread(&(index_list[i]->search_key.number), sizeof(int), 1, index_file);
        else{
            fread(&(index_list[i]->search_key.string), sizeof(char), 12, index_file);
            index_list[i]->search_key.string[12] = '\0';
        }
        fread(&(index_list[i]->byte_Offset), sizeof(unsigned long int), 1, index_file);
    }
    
    *list_length = header->n_reg_file;
    index_header_delete(&header);
    
    return index_list;
}

//-----------------------------------------------------------------------------------


// Returns the id of the searched value in the list ;
int index_binary_search(Index_data_t ** list, int inicial_id, int final_id, Index_data_t * filter, int key_type){
    
    // the stop condition of the recursion :
    if(inicial_id <= final_id){
        
        int mid_id = inicial_id + (final_id - inicial_id) / 2;
        int conditon;
        
        //comparing the middle index to the searched one and branching the recursion direction
        if(key_type == Number){
            conditon = index_data_compare_numberKey(list[mid_id], filter);
            if(conditon == 0) return mid_id;
            else{
                if(conditon < 0) return index_binary_search(list, mid_id + 1, final_id, filter, key_type);
                else return index_binary_search(list, inicial_id, mid_id - 1, filter, key_type);
            }
        }else{
            conditon = index_data_compare_stringKey(list[mid_id], filter);
            if(conditon == 0) return mid_id;
            else{
                if(conditon < 0) return index_binary_search(list, mid_id + 1, final_id, filter, key_type);
                else return index_binary_search(list, inicial_id, mid_id - 1, filter, key_type);
            }
        }
    }
    
    // if the codes reaches the bottom, the searched value isn`t in the list
    return -1;
}


//-----------------------------------------------------------------------------------


Index_data_t ** index_list_filtering(Index_data_t ** source_list, union key filter, int n_index, int key_type, int * new_list_length){
    
    Index_data_t ** source = source_list;
    Index_data_t ** new_list = NULL;
    Index_data_t index_buffer;
    int new_counter;
    int found_id;
    
    
    // Will use the binary search to find one index that matches the filter.
    // But we have to be prepared to the case of non-unique filter keys.
    // So, to assure that, the code will find the leftest matching key and
    // crate a new list from it.
    
    if(key_type == Number){
        index_buffer.search_key.number = filter.number;
        
    }else{
        strcpy(index_buffer.search_key.string, filter.string);
        index_buffer.search_key.string[12] = '\0';
    }
    
    found_id = index_binary_search(source, 0, n_index - 1, &index_buffer, key_type);
    
    if(found_id == -1){
        *new_list_length = 0;
        return NULL;
    }
    
    while(found_id >= 0 && ((key_type == Number ? index_data_compare_numberKey(source[found_id], &index_buffer) : index_data_compare_stringKey(source[found_id], &index_buffer)) == 0)) found_id--;

    // now we assured that the found_id has the id from the first matching key is next to the found_id
    found_id++;
    
    for(new_counter = 0; found_id < n_index && ((key_type == Number ? index_data_compare_numberKey(source[found_id], &index_buffer) : index_data_compare_stringKey(source[found_id], &index_buffer)) == 0) && source[found_id] != NULL; found_id++, new_counter++){
        
        // reajusting the adress length
        new_list = (Index_data_t **) realloc(new_list,sizeof(Index_data_t *) * (new_counter + 1));
        assert(new_list);
        new_list[new_counter] = (Index_data_t *) malloc(sizeof(Index_data_t));
        assert(new_list[new_counter]);
        
        // passing the information to the new list
        new_list[new_counter]->byte_Offset = source[found_id]->byte_Offset;
        
        if(key_type == Number) new_list[new_counter]->search_key.number = source[found_id]->search_key.number;
        else strcpy(new_list[new_counter]->search_key.string, source[found_id]->search_key.string);
    }
    
    *new_list_length = new_counter;
    return new_list;
}

//-----------------------------------------------------------------------------------



Index_data_t ** indexed_crime_searching(Index_data_t ** source_index_list, int list_length, FILE * data_file, int index_type, char * index_field_info, CallBackF * executable){
    
    // for the search
    int new_list_length;
    union key filter;
    
    //building a filter key
    if(index_type == Number) filter.number = atoi(index_field_info);
    else{
        int n_info = (int) strlen(index_field_info);
        strncpy(filter.string, index_field_info, 12);
        while (n_info < 12) filter.string[n_info++] = TRASH_IDENTIFIER;
        filter.string[12] = '\0';
    }
    
    // filtering the previous list, returning the Index registers that matches the key filter
    
    Index_data_t ** filtered_list = index_list_filtering(source_index_list, filter, list_length, index_type, &new_list_length);
    
    // if the filtered list is not empty, we need verify the similarity of the information given as input
    // and the crimes read from the data_file through the indexed acess function.
    
    // this flag will be used to verify there wasnt any real executions un the indexed acess;
    executable->results_flag = false;
    
    for(int i = 0; new_list_length > 0 && i < new_list_length; i++) crime_indexed_access(filtered_list[i], data_file, executable);
    
    if(!executable->results_flag && !executable->deletion_flag) printf("Registro inexistente.\n");
        
    if(new_list_length > 0) index_data_vec_delete(&filtered_list, new_list_length);
    
    return source_index_list;
}

//-----------------------------------------------------------------------------------


unsigned long int crime_indexed_access(Index_data_t * index_filtered, FILE * data_file, CallBackF * executable){
    
    unsigned long int returned_value = 0;
    Crime_t * crime_reading_pointer = crime_create();
    
    //bytesOffset valued as zero signify a index register logically removed
    
    fseek(data_file, index_filtered->byte_Offset, SEEK_SET);
    returned_value += crime_reading(data_file, crime_reading_pointer);
    
    if(crime_reading_pointer->removed != '1'){
        if(executable->comparing_flag == false){
            
            //printing without comparing
            
            executable->print(crime_reading_pointer);
            
        }else if(executable->deletion_flag && executable->comparing_flag && index_filtered->byte_Offset != 0){
            
            if(executable->compare(crime_reading_pointer, executable->filter, executable->field_list, executable->n_fields) == 0){
                
                // doing the logical deletion:
                executable->logical_deletion(index_filtered->byte_Offset, data_file, crime_reading_pointer);
                executable->results_flag = true;
                
                // atualizing the removed data vec:
                executable->deleted_data_adress =  (unsigned long int *) realloc(executable->deleted_data_adress, sizeof(unsigned long int) * (executable->general_counter + 1));
                assert(executable->deleted_data_adress);
                executable->deleted_data_adress[executable->general_counter] = index_filtered->byte_Offset;
                executable->general_counter++;
            }
        }else{
            
            //printing the matching crimes
            if(executable->compare(crime_reading_pointer, executable->filter, executable->field_list, executable->n_fields) == 0){
                executable->print(crime_reading_pointer);
                executable->results_flag = true;
            }
        }
    }
    
    crime_liberate_dynamic_strings(&crime_reading_pointer);
    
    crime_delete(&crime_reading_pointer);
    
    return returned_value + index_filtered->byte_Offset;
}


