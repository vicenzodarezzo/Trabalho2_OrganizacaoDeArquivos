/* Vicenzo D'Arezzo Zilio - 13671790 */
/* INFORMACOES JOAO */
/* Files Organization - 2023 */


#ifndef csvFunctions_h
#define csvFunctions_h

#include <stdio.h>
#include "inputOutput.h"
#include "index.h"

#endif /* csvFunctions_h */

// SETUP FUNCTIONS: ------------------------------------------------------

/** Mainly reads the data from a text - csv - file and writes it in a binary file. For this, it manipulates two buffers,
 * one for the line string and one for the information string, in two loops and the reading auxiliar functions.
 * The first loop is oriented by the reading of a line in once, and the other one consists in a char per char interaction
 * for storing the fields while the end of the register isint found. In the end of each interaction o fthe line loop,
 * the function writes the crime register data in the binary file, atualizing information in the header,
 * that will be correctly written in the end of the function.
 */
void create_table(FILE * read_file, FILE * write_file);

/** Create a linear index based on the string passed as input. This index is sorted and stored in a binary file,
 *  making reference to the access byte of each indexed key. This key can be used as an int or a string of 12 chars (has length 13 in
 *  definition but only 12 will be written, the extra space is for assuring the \0 during it manipulation on main memory) and will truncate
 *  indexed strings in the twelfth character.
 */
void create_index(FILE * read_file, char key_str[], FILE * index_file);

// SELECTING FUNCTIONS : searching and deletion -----------------------------------------------


/** Uses the selection function and configures it to print all the valid register by passing as a parameter
 *  one CallBack function register to print without comparing. It do not uses any idex information or parameters.
 */
void select_all(FILE * read_file);

/**
 *  Searchs for crimes that has the specified value in the specified fields, passed by the user through the standard input.
 *  The search can happen with or without the index. If the indexed field belongs to the group of speciefed inputs, the
 *  algorithm will bring the index to main memory and apply a binary search in it, after this, will acces punctually the data
 *  file and compare the crimes.
 *  If the speciefied fields arent indexed, the algorthm will use the selection function, that runs linearly the data file doing
 *  a conditional printing based in the pointers of funcitions of comparing and printing passed through the CallBack Register.
 */
void select_from(FILE * data_file, FILE * index_file, char * index_name, int n_search);

/**
 * The deletion function works in the same way of the select all. It divides each search in two categories :
 * a indexed search, and a linear search. For the first one, uses the indexed search function
 * to bring the index to main memory and do a binary search in it. For the second,
 * uses the linear selection to run through the data file. The diference between this procedure
 * and the selection one is the Callback register designeted and the maintenance applied to the files in the end.
 * The callback registers will store the decision making flag used for deteletion and its function,
 * also, will store a byteOffset list that represent the adress of the deleted registers.
 * This list will guide the atualization of the data and index file.
 */

void delete_from(FILE * data_file, FILE * index_file, char * index_name, int n_search, char * index_file_name);

// DATA MANIPULATING FUNCTIONS : insertion --------------------------------------------------

/**
 * This function receiver the data and register files, being able to append
 * to tha data base a new crime register. It receives from the stardard input
 * some string values and store they in a crime buffer. This crime buffer will be
 * analyzed if it is subject to an index insertion and, if that is the case, will be appended to the index
 * brought to the main memory. For the writing, in the data file, the proxByteOffset stored in the
 * header register indicate the new postions for writing, and, as new registers are being written, it is updated.
 * In the index file, the Index list in main memory, now appended, is sorted by the heapsort in
 * Index module and re-written from scratch in the file.
 */
void inser_into(FILE * data_file, FILE * index_file, char * index_file_name, char * index_name_info, int n_insertions);
