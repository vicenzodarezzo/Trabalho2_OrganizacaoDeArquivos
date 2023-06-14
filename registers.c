/* Vicenzo D'Arezzo Zilio (80%) funções 3, 4, 5, 6 - 13671790 */
/* Marina Souza Figueiredo (20%) funções 7 - 13671827 */
/* Files Organization - 2023 */



#include "registers.h"

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
// HEADER FUNCTIONS:
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------


Header_t * header_create(void){
    Header_t * h = (Header_t *) malloc(sizeof(Header_t));
    assert(h);
    // assumes that, while creating the header register, the current file able to be read/written
    h->status = '0';
    h->nRegFile = 0;
    h->nRegRem = 0;
    h->proxByteOffset = 0;
    
    return h;
}

void header_regNumber_increment(Header_t * h){
    assert(h);
    (h)->nRegFile++;
}

void header_byteOffset_increment(Header_t * h, unsigned long int n_bytes_written){
    assert(h);
    h->proxByteOffset += n_bytes_written ;
}

void header_delete(Header_t ** h){
    assert(*h);
    free(*h);
    *h = NULL;
}

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
// CRIME FUNCTIONS:
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------

Crime_t * crime_create(void){
    Crime_t * c = (Crime_t *) malloc(sizeof(Crime_t));
    assert(c);
    // assumes that the crime register created has not been removed.
    // in that case the user can set this field for '1' later in the code
    c->removed = '0';
    return c;
}

Crime_t ** crime_list_create(int length){
    Crime_t ** list = (Crime_t **) malloc(sizeof(Crime_t *) * length);
    assert(list);
    for(int i = 0; i < length; i++) list[i] = crime_create();
    return list;
}

void crime_liberate_dynamic_strings(Crime_t ** crime){
    Crime_t * pointer = *crime;
    assert(pointer);
    if(pointer->crimeDescription != NULL) free(pointer->crimeDescription);
    if(pointer->crimePlace != NULL) free(pointer->crimePlace);
}


void crime_field_association(char * info, int field_counter, Crime_t * crime){
    
    Crime_t * add_register = crime;
    // length of the current information
    int length = (int) strlen(info);
    // a counter used to run through the variable length strings
    int iterator;
    
    // deciosion structure based in the rest of the division of the counter and the number
    // of fields in a crime register.
    switch (field_counter % NUMBER_OF_FIELDS)
    {
    case (0) :
        add_register->idCrime = atoi(info);
        break;

    case (1) :
        for(int i = 0; i < 10; i++){
            if(i < length) add_register->crimeDate[i] = info[i];
            //substitutes the \0 by the specified delimiternReg
            else add_register->crimeDate[i] = TRASH_IDENTIFIER;
        }
        break;

    case (2) :
        // Veryfys the case of a NULL information
        if(info[0] != '\0') add_register->nArticle = atoi(info);
        else add_register->nArticle = -1;
        break;

    case (3) :
        // this is a variable size field, so, we need to allocate the memory
            add_register->crimePlace = (char  *) malloc(sizeof(char) * (length + 1));
            assert(add_register->crimePlace);
            for(iterator = 0; info[iterator] != '\0'; iterator++){
                add_register->crimePlace[iterator] = info[iterator];
            }
            add_register->crimePlace[iterator] = '\0';
            break;

    case (4) :
        // this is a variable size field, so, we need to allocate the memory
            add_register->crimeDescription = (char  *) malloc(sizeof(char) * (length + 1));
            assert(add_register->crimeDescription);
            for(iterator = 0; info[iterator] != '\0'; iterator++){
                add_register->crimeDescription[iterator] = info[iterator];
            }
            add_register->crimeDescription[iterator] = '\0';
        break;

    case (5):
        for(int i = 0; i < 12; i++){
            if(i < length) add_register->phoneBrand[i] = info[i];
            //substitutes the \0 by the specified delimiter
            else add_register->phoneBrand[i] = TRASH_IDENTIFIER;
        }
        break;

    default:
        fprintf(stderr, "\nError: pointer %% numberOfFileds\n");
        break;
    }
}


int crime_numbered_fields_compare( Crime_t  * a, Crime_t * b, int * fields_list, int list_length){
    
    int returned_value = 0;
    
    for(int i = 0; i < list_length && returned_value == 0; i++){
        switch(fields_list[i]){
            case (0) :
                
                if(a->idCrime != b->idCrime) returned_value = 1;
                break;

            case (1) :
                
                if(strncmp(a->crimeDate, b->crimeDate, 10) != 0) returned_value = 1;
                break;

            case (2) :
                
                if(a->nArticle != b->nArticle) returned_value = 1;
                break;

            case (3) :
                
                if(strcmp(a->crimePlace, b->crimePlace) != 0) returned_value = 1;
                break;
                
            case (4) :
                
                if(strcmp(a->crimeDescription, b->crimeDescription) != 0) returned_value = 1;
                break;
                
            case (5):
                if(strncmp(a->phoneBrand, b->phoneBrand, 12) != 0) returned_value = 1;
                break;

            default:
                fprintf(stderr, "\nError: pointer %% numberOfFileds\n");
                break;
            }
    }
    
    return returned_value;
}

void crime_delete(Crime_t ** pointer){
    Crime_t * p = *pointer;
    assert(p);
    free(p);
    *pointer = NULL;
}

void crime_list_delete(Crime_t *** pointer, int length){
    Crime_t ** p = *pointer;
    for(int i = 0; i < length; i++) crime_delete(&p[i]);
    free(p);
    p = NULL;
}

