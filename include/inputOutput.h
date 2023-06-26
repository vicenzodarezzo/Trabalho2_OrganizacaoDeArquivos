/* Files Organization - 2023 */

#ifndef inputOutput_h
#define inputOutput_h

# include <stdbool.h>
# include <ctype.h>

# include "registers.h"
# include "bTree_index.h"

//TYPEDEFS:

typedef struct callBackFunctions{
    
    int (*compare)(Crime_t * a, Crime_t *b, int * fields_list, int n_fields);
    void (*print)(Crime_t * a);
    void (*logical_deletion)(long long int byteOffset, FILE * data_file, Crime_t * crime);
    Crime_t * filter;
    int * field_list;
    int n_fields;
    int general_counter;
    // vector used for deletion
    long long int * deleted_data_adress;
    // flag used for transmits the results of each function through the abstraction
    bool results_flag;
    // Flags for the decision making in the acessing of the functions pointers
    bool comparing_flag;
    bool deletion_flag;
    
}CallBackF;

//GIVEN FUNCTIONS: --------------------------------------------------------------------------------

void binarioNaTela(char *nomeArquivoBinario);

void scan_quote_string(char *str);

// WRITING FUNCTIOS : --------------------------------------------------------------------------------

/** Writes the information from a crime register into a binary file. It's return represents the number of bytes that were written in the file.
 *  You can use it for incrementing the byteOffset of the File. 
 *  A good observation to do is that this function substitutes de '\0' in the writing by the specified string delimiter
 * */
long long int crime_bin_write(FILE * binFile, Crime_t * crime);

/** Writes the information from a header register into a binary file. It's return represents the number of bytes that were written in the file.
 *  You can use it for incrementing the byteOffset of the File
 */
long long int header_bin_write(FILE * binFile, Header_t * header);

// READING FUNCTIONS: --------------------------------------------------------------------------------

long long int header_reading(FILE * read_file, Header_t * h);

long long int crime_reading(FILE * read_file, Crime_t * crime);

/**
 * Function that structures the reading of information from the standard input file in the swage of the search function (same for deletion)
 * it stores the corresponding information fields, in the previous code used in this project, in the searched_fields vector and the crime
 * regitry passed as a parameter.
 */
Crime_t * input_information_reading_for_searches(bool * index_search, int * searched_fields,
    int n_fields, Crime_t * crime_reading_pointer);

/**
 * Function that structures the reading of informaiton from the standard input file in the swage of the insertion function
 * it stores the corresponding information fields, in the previous code used in this project, in the  the crime
 * regitry passed as a parameter and sets a flag as true when it read a valid value for the indexed field, passing to its
 * information the calling function through the index_info string.
 */

Crime_t * Crime_insertion_input_reading(int id_index, bool * index_flag, char * index_info);




// SELECTING FUNCTIONS : searching and deletion ---------------------------------------------------------------------

/**
 * This is the function responsable for running the data file linearly. To do it, it uses a byteoffset counter and a crime buffer to do a 
 * reading cycle, analyzing crime per crime. In the analysis, it uses the callBack structure to do a procedure in the current file position.
 * Through this tecnique, this function can be used for selection (with or without a condition) and deleting.
 */
void linear_selection(FILE * read_file, CallBackF * executable);

/** Prints the crime separating its fields by a comma and ending the line when the register
 * is finished printing. It does not print the delimiters.
 */
void crime_printing(Crime_t * crime);

/**
 * The function that atualizes the status of an crime register and re-writes it in the data file. It's passed
 * as a parameter to the deletion function through the Callback registry
 */
void crime_logical_deletion(long long int byteOffset, FILE * data_file, Crime_t * crime);

/**
 * Momentarily accesses the indicated register in the data_file, doing a executable task according to the
 * CallBack registry passed as a parameter.
 */
long long int byteOffset_point_access(long long int byteOffset, FILE * data_file, CallBackF * executable);

#endif /* inputOutput */