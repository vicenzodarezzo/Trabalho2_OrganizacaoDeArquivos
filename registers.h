/* Vicenzo D'Arezzo Zilio (80%) funções 3, 4, 5, 6 - 13671790 */
/* Marina Souza Figueiredo (20%) funções 7 - 13671827 */
/* Files Organization - 2023 */


#ifndef registers_h
#define registers_h

# include <stdio.h>
# include <stdlib.h>
# include <assert.h>
# include <string.h>



// CONVENTIONS:

/* Variabel lenght register, delimited by '#' */
/* Variabel lenght flied, delimited by '|' */

/* NULL values in fixed lenght fields: */
/* If the fields are int type, we treat the null value as the -1 negative number */
/* If the fields are int char, we treat the null value as the '$' */


// FIXED VALUES
# define NUMBER_OF_FIELDS 7
# define STR_DELIMITER '|'
# define TRASH_IDENTIFIER '$'

// REGISTERS STRUCTURES

typedef struct header{
    char status;
    unsigned long int proxByteOffset;
    int nRegFile;
    int nRegRem;
} Header_t ;

typedef struct crime{
    char removed;
    int idCrime;
    char crimeDate[10];
    int nArticle;
    char phoneBrand[12];
    char * crimePlace;
    char * crimeDescription;
} Crime_t ;


// HEADER FUNCTIONS

/** Creates the header register incializing the status field with '1' and the rest of then with 0 or '0', according to its type */
Header_t * header_create(void);

/** Just increment the field */
void header_regNumber_increment(Header_t * h);

/** Just increment the field */
void header_byteOffset_increment(Header_t * h, unsigned long int n_bytes_written);

/** Liberates all of the memory associated with this register and its adress */
void header_delete(Header_t ** h);

// CRIME FUNCTIONS

/** Creates the crime register incializing the fixed length fields, but not the variable. That last ones have to be allocated
in the crime field asociation function */
Crime_t * crime_create(void);

/** Free the memory allocated in the dynamic fields of a crime register, without liberating it self.*/
void crime_liberate_dynamic_strings(Crime_t ** pointer);

/** Auxiliar function to field association between the read strings from some text source and the registers stored by the program. It recives
 the adress of the information, the counter related with the order whose field was read and the register where it is stored. The storage
 is based int the rest of the division by the field counter and the number of fields, assuming the order is correspondent to the file pattern.
 It is valid to say that the fixed length strings are filled with de trash identifier, but the variable ones are not delimited by the specified delimiter
 yet, this action will be assured on the writing functions */
void crime_field_association(char * info, int field_counter, Crime_t * add_register);

/** Free the memory allocated in the dynamic fields of a crime register, without liberating it self.*/
void crime_delete(Crime_t ** pointer);
    
int crime_numbered_fields_compare( Crime_t  * a, Crime_t * b, int * fields_list, int list_length);


#endif /* registers_h */
