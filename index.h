/* Vicenzo D'Arezzo Zilio - 13671790 */
/* INFORMACOES JOAO */
/* Files Organization - 2023 */

#ifndef index_h
#define index_h

#include "registers.h"
#include "inputOutput.h"

// Defining the union :
union key{
    int number ;
    char string [13] ;
};

// Defining the enum :
enum key_type { Number = 0 , String = 1} ;

// REGISTERS STRUCTURES :
typedef struct index_header{
    char status ;
    int n_reg_file ;
}Index_header_t;

typedef struct index_data{
    union key search_key ;
    unsigned long int byte_Offset ;
} Index_data_t;


// SETUP FUNCTIONS : -------------------------------------

Index_header_t * index_header_create(void);

Index_data_t ** index_dataList_create(int length);

void index_header_delete(Index_header_t ** header);

void index_data_vec_delete(Index_data_t *** data, int length);

/**
 * This function is responsable for atualizing the information stored in the index file and in the header of the data file.
 * It utilizes the information generated in the deletion function to rewrite from the star the index file, removing the reference
 * to the deleted registers through the linear verification of the byteOffset stored in the key and the deleted_data_adress, a
 * byteoffset list genereted in the deletion. In the end, reads and writes the header of the data file, now with the new number
 * of removed registers.
 */
void files_maintenance(FILE * index_file, FILE * data_file, char * index_file_name,
    Index_data_t ** index_list, int n_index, int index_type, int rem_counter,
    unsigned long int * deleted_data_adress);

Index_data_t ** index_list_append(union key search_key, unsigned long int byteOffset,
    Index_data_t ** index_list, int * list_length, int key_type);

// READING FUNCTIONS : -------------------------------------------
/**
 * Associates the input string to a int value ( 0 to 5 ) that provides the information of each of the fields that input refers
 */
int index_crimeField_pairing(char str[]);

/**
 * The main function for reading. It is responsible for reading crime per crime in the read_file, assuring if the file, the crime registry
 *  and the indexed field are valid. In that way, stores the informations in a list of index_data_t pointers and return it to the program.
 *  Other functions called inside it are descibed in the source file of the index module.
 */
Index_data_t ** index_data_reading(FILE * read_file, int n_reg, unsigned long int *  byte_Offset,
    int key_field, int * n_index);

Index_header_t * index_header_reading(FILE * index_file, Index_header_t * header);


void index_string_key_association(char * destiny, char * info, int key_field);

// SORTING FUNCTIONS : --------------------------------------------

/**
 * Uses the a HeapSort, with the heap abstraction in the parameterized vector, to ordenates de Index_data_t by its information.
 */
void index_data_sorting(Index_data_t ** vec, int key_type, int n_index);

/**
 * Prints a list of Index data Registers
 */
void index_list_printing(Index_data_t **vec, int key_type, int n_index);


// WRITING FUNCTIONS : -------------------------------------------------

/**
 * The main function for writing. It is responsible for writing the header and the list of data related to the index registry. In case of string search_key,
 * assures the right form of writing using the trash identifier '$' . Also, the inicial header will be seted as status '0' and, in the end of the list writing,
 * will be re-written with the '1' status. Other functions called inside it are descibed in the source file of the index module.
 */
void index_data_list_writing(FILE * f, Index_data_t ** data_vec, int key_type, int n_reg_file);

/**
 * The function for writing the Index_header registry into the file .
 */
void index_header_writing(FILE * f, Index_header_t * header);

/**
 * The function for writing a single Index_data registry into the file .
 */
void index_data_writing(FILE * f, Index_data_t * data_reg, bool number_key_flag);


// SEARCHING FUNCTIONS : ---------------------------------------------------

/**
 * Returns a list of Index data registers in the mainMemory, based in the linearly reading of the index_file.
 */
Index_data_t ** index_bringing_to_mainMemory(FILE * index_file, int key_type, int * list_length);

/**
 * Creates a new Index data registers list based in the previous one (source_list). For it, applies a comparison with the key_filter provided.
 * It is important to know that the new length of the list will be stored in the int pointer provided and the source list will have it memory released
 */
Index_data_t ** index_list_filtering(Index_data_t ** source_list, union key filter, int n_index,
    int key_type, int * new_list_length);

/**
 *  This function representes the punctual access of the data file by an indexed information. For it, receives an source index_list, that representes the index
 *  informaiton in main memory, and applies a filter through a crime register built with the input information. The callBack executable parameter works in the
 *  actions that can be done during the access and manipulation of index and data, alowing the function to work with basic searching and searching for delection.
 */
Index_data_t ** indexed_crime_searching(Index_data_t ** source_index_list, int list_length,
    FILE * data_file, int index_type, char * index_field_info, CallBackF * executable);

/**
 * This function is responsable to access the data file punctually based in the byteoffset stored in the Index data registry.
 * There, with the file position and the read crime register, will execute the callBack function passed as a parameter.
 */
unsigned long int crime_indexed_access(Index_data_t * index, FILE * data_file,
    CallBackF * executable);



#endif /* index_h */
