/* Vicenzo D'Arezzo Zilio - 13671790 */
/* INFORMACOES JOAO */
/* Files Organization - 2023 */


#include "inputOutput.h"
#include "index.h"

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------
// GIVEN FUNCTIONS:
// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

void binarioNaTela(char *nomeArquivoBinario) {
    /*
     * Você não precisa entender o código dessa função.
     *
     * Use essa função para comparação no run.codes.
     * Lembre-se de ter fechado (fclose) o arquivo anteriormente.
     * Ela vai abrir de novo para leitura e depois fechar
     * (você não vai perder pontos por isso se usar ela).
     */

    unsigned long i, cs;
    unsigned char *mb;
    size_t fl;
    FILE *fs;
    if (nomeArquivoBinario == NULL || !(fs = fopen(nomeArquivoBinario, "rb"))) {
        fprintf(stderr,
                "ERRO AO ESCREVER O BINARIO NA TELA (função binarioNaTela): "
                "não foi possível abrir o arquivo que me passou para leitura. "
                "Ele existe e você tá passando o nome certo? Você lembrou de "
                "fechar ele com fclose depois de usar?\n");
        return;
    }
    fseek(fs, 0, SEEK_END);
    fl = ftell(fs);
    fseek(fs, 0, SEEK_SET);
    mb = (unsigned char *)malloc(fl);
    fread(mb, 1, fl, fs);

    cs = 0;
    for (i = 0; i < fl; i++) {
        cs += (unsigned long)mb[i];
    }
    printf("%lf\n", (cs / (double)100));
    free(mb);
    fclose(fs);
}

//-----------------------------------------------------------------------------------


void scan_quote_string(char *str) {
    /*
     * Use essa função para ler um campo string delimitado entre aspas (").
     * Chame ela na hora que for ler tal campo. Por exemplo:
     *
     * A entrada está da seguinte forma:
     *    nomeDoCampo "MARIA DA SILVA"
     *
     * Para ler isso para as strings já alocadas str1 e str2 do seu programa,
     * você faz:
     *    scanf("%s", str1); // Vai salvar nomeDoCampo em str1
     *    scan_quote_string(str2); // Vai salvar MARIA DA SILVA em str2
     *                             // (sem as aspas)
     */

    char R;

    while ((R = getchar()) != EOF && isspace(R)) {
        ;  // ignorar espaços, \r, \n...
    }

    if (R == 'N' || R == 'n') {  // campo NULO
        getchar();
        getchar();
        getchar();  // ignorar o "ULO" de NULO.
        strcpy(str, "");  // copia string vazia
    } else if (R == '\"') {
        if (scanf("%[^\"]", str) != 1) {  // ler até o fechamento das aspas
            strcpy(str, "");
        }
        getchar();  // ignorar aspas fechando
    } else if (R != EOF) {  // vc tá tentando ler uma string
                            // que não tá entre aspas!
                            // Fazer leitura normal %s então,
                            // pois deve ser algum inteiro ou algo assim...
        str[0] = R;
        scanf("%s", &str[1]);
    } else {  // EOF
        strcpy(str, "");
    }
}


// WRITING FUNCTIONS: -----------------------------------------------------------


// This is an auxiliar function to manages writing correctly variable lenght char fields in
// the file. It assures the correct delimiter and returns the specified number of written bytes.
unsigned long int crime_varible_length_field_write(Crime_t * crime, int id_field, FILE * f){
    
    // The counter is used to store the number of bytes written by the function ;
    unsigned long int counter = 0;
    char delimiter = STR_DELIMITER;
    // Interaction pointer is used to run the variable length string;
    char * interaction_pointer;
    
    switch(id_field){
        case 0 :
            interaction_pointer = crime->crimePlace;
            break;
        case 1 :
            interaction_pointer = crime->crimeDescription;
            break;
        default:
            fprintf(stderr, "error in the id_field in variables fields writing\n");
            return 0;
    }
    
    while(*interaction_pointer != '\0'){
        counter += sizeof(char) * fwrite(interaction_pointer++, sizeof(char), 1, f);
    }
    // writes the delimiter in the end of the string
    counter += sizeof(char) * fwrite(&delimiter, sizeof(char), 1, f);
    return counter;
}


//-----------------------------------------------------------------------------------


unsigned long int crime_bin_write(FILE * binFile, Crime_t * crime){
    FILE * f = binFile;
    assert(f);
    char delimiter = '#';
    unsigned long int counter = 0;
    
    counter += sizeof(char) * fwrite(&(crime->removed), sizeof(char), 1, f);
    counter += sizeof(int) * fwrite(&(crime->idCrime), sizeof(int), 1, f);
    counter += sizeof(char) * fwrite(&(crime->crimeDate), sizeof(char), 10, f);
    counter += sizeof(int) * fwrite(&(crime->nArticle), sizeof(int), 1, f);
    counter += sizeof(char) * fwrite(&(crime->phoneBrand), sizeof(char), 12, f);
    counter += crime_varible_length_field_write(crime, 0, f);
    counter += crime_varible_length_field_write(crime, 1, f);
    counter += sizeof(char) * fwrite(&delimiter, sizeof(char), 1, f);
    
    return counter;
}

//-----------------------------------------------------------------------------------


unsigned long int header_bin_write(FILE * binFile, Header_t * header){
    
    assert(binFile);
    unsigned long int counter = 0;
    
    counter += sizeof(char) * fwrite(&(header->status), sizeof(char), 1, binFile);
    counter += sizeof(unsigned long int) * fwrite(&(header->proxByteOffset), sizeof(unsigned long int), 1, binFile);
    counter += sizeof(int) * fwrite(&(header->nRegFile), sizeof(int), 1, binFile);
    counter += sizeof(int) * fwrite(&(header->nRegRem), sizeof(int), 1, binFile);
    
    return counter;
}

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------
// READING FUNCTIONS:
// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

unsigned long int header_reading(FILE * read_file, Header_t * header){
    
    assert(read_file);
    unsigned long int counter = 0 ;
    counter += sizeof(char) * fread(&(header->status), sizeof(char), 1, read_file);
    counter += sizeof(unsigned long int) * fread(&(header->proxByteOffset), sizeof(unsigned long int), 1, read_file);
    counter += sizeof(int) * fread(&(header->nRegFile), sizeof(int), 1, read_file);
    counter += sizeof(int) * fread(&(header->nRegRem), sizeof(int), 1, read_file);
    
    return counter ;
}

//-----------------------------------------------------------------------------------


// This is an auxiliar function for reading a variable length string from a binary file.
// It assumes that the string was written following the specified string delimiter. Using
// this information, return an inicialized and allocated dynamic string without the
// specified string delimiter.
char * variable_field_reading(FILE * read_file, unsigned long int * counter){
    
    char str_buffer[BUFSIZ];
    char * str_return;
    int char_counter = 0;
    char char_buffer;
    
    fread(&char_buffer, sizeof(char), 1, read_file);
    
    while(char_buffer != STR_DELIMITER){
        str_buffer[char_counter++] = char_buffer;
        fread(&char_buffer, sizeof(char), 1, read_file);
    }
    
    // uses the \0 just to delimit the buffer, the current string will be trated with the string
    // delimiter in the writing function.
    
    str_buffer[char_counter++] = '\0';
    
    str_return = (char *) malloc(sizeof(char)*char_counter);
    assert(str_return);
    strcpy(str_return, str_buffer);
    *counter += char_counter;
    return str_return;
}

//-----------------------------------------------------------------------------------


unsigned long int crime_reading(FILE * read_file, Crime_t * crime){
    
    Crime_t * pointer = crime;
    char * varible_field_info;
    unsigned long int counter = 0 ;
    
    // Reading the fixed lenght fields of the register
    counter += sizeof(char) * fread(&(pointer->removed), sizeof(char), 1, read_file);
    counter += sizeof(int) * fread(&(pointer->idCrime), sizeof(int), 1, read_file);
    counter += sizeof(char) * fread(&(pointer->crimeDate), sizeof(char), 10, read_file);
    counter += sizeof(int) * fread(&(pointer->nArticle), sizeof(int), 1, read_file);
    counter += sizeof(char) * fread(&(pointer->phoneBrand), sizeof(char), 12, read_file);
    
    //Reading the variable lenght fields of the register
    for (int i = 0; i < 2; i++) {
        varible_field_info = variable_field_reading(read_file, &counter);
        crime_field_association(varible_field_info, 3+i, crime);
    }
    
    
    //Descarting the Register Delimiter
    fseek(read_file, 1, SEEK_CUR);
    counter++;
    
    
    return counter;
}

//-----------------------------------------------------------------------------------


Crime_t * input_information_reading_for_searches(char * index_field_info, bool * index_search, int
    index_id, int * searched_fields, int n_fields, Crime_t * crime_reading_pointer){
    
    char name_field_buffer[BUFSIZ];
    char info_field_buffer[BUFSIZ];
    
    for (int i = 0; i < n_fields; i++){
        
        fscanf(stdin, "%s", name_field_buffer);
       
        searched_fields[i] = index_crimeField_pairing(name_field_buffer);
        
        if(searched_fields[i] == 0 || searched_fields[i] == 2){
            fscanf(stdin, "%s", info_field_buffer);
        }else{
            scan_quote_string(info_field_buffer);
        }
        
        crime_field_association(info_field_buffer, searched_fields[i], crime_reading_pointer);
        
        //analyzing if the case uses the index:
        if(searched_fields[i] == index_id){
            *index_search = true;
            strcpy(index_field_info, info_field_buffer);
        }
    }
    
    return crime_reading_pointer;
}

Crime_t * Crime_insertion_input_reading(int id_index, bool * index_flag, char * index_info){
    
    Crime_t * crime = crime_create();
    char buffer[BUFSIZ];
    
    for(int i = 0;  i < 6; i++){
        scan_quote_string(buffer);
    
        if(i == id_index){
            if(strcmp(buffer, "") == 0) *index_flag = false;
            else strcpy(index_info, buffer);
        }
        
        crime_field_association(buffer, i, crime);
    }
    
    return crime;
}

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------
// GENERAL PORPOUSE FUNCTIONS
// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

void crime_printing(Crime_t * crime){
    
    char * interaction_pointer;
    int fixed_string_char_counter;
    
    // crime id:
    fprintf(stdout, "%d, ", crime->idCrime);
    
    // crime date:
    interaction_pointer = crime->crimeDate;
    if(*interaction_pointer == TRASH_IDENTIFIER) fprintf(stdout, "NULO");
    else{
        fixed_string_char_counter = 0;
        while(*interaction_pointer != TRASH_IDENTIFIER && fixed_string_char_counter < 10){
            fprintf(stdout, "%c", *interaction_pointer);
            interaction_pointer++;
            fixed_string_char_counter++;
        }
    }
    printf(", ");
    
    // number article:
    if(crime->nArticle == -1) fprintf(stdout, "NULO, ");
    else fprintf(stdout, "%d, ", crime->nArticle);
    
    // crime place:
    interaction_pointer = crime->crimePlace;
    if(*interaction_pointer == '\0') fprintf(stdout, "NULO");
    else{
        while(*interaction_pointer != '\0'){
            fprintf(stdout, "%c", *interaction_pointer);
            interaction_pointer++;
        }
    }
    printf(", ");
    
    // crime description
    interaction_pointer = crime->crimeDescription;
    if(*interaction_pointer == '\0') fprintf(stdout, "NULO");
    else{
        while(*interaction_pointer != '\0'){
            fprintf(stdout, "%c", *interaction_pointer);
            interaction_pointer++;
        }
    }
    printf(", ");
    
    // phone brand:
    interaction_pointer = crime->phoneBrand;
    if(*interaction_pointer == TRASH_IDENTIFIER) fprintf(stdout, "NULO");
    else{
        fixed_string_char_counter = 0;
        while(*interaction_pointer != TRASH_IDENTIFIER && fixed_string_char_counter < 12){
            fprintf(stdout, "%c", *interaction_pointer);
            interaction_pointer++;
            fixed_string_char_counter++;
        }
    }
    printf("\n");
    
}


// -------------------------------------------------------------------------
// -------------------------------------------------------------------------
// -> SEARCH AND SELECT FUNCTIONS
// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

void linear_selection(FILE * data_file, CallBackF * executable){

    assert(data_file);
    Header_t * header = header_create();
    unsigned long int current_byteOffset = 0;
    unsigned long int last_byteOffset;
    
    current_byteOffset += header_reading(data_file, header);
    
    Crime_t * crime_reading_pointer = crime_create();
    
    if(header->status == '0'){
        printf("Falha no processamento do arquivo.\n");
        header_delete(&header);
        crime_delete(&crime_reading_pointer);
        return;
    }if (header->nRegFile == 0){
        printf("Registro inexistente.\n");
        header_delete(&header);
        crime_delete(&crime_reading_pointer);
        return;
    }
    
    // Now that we know the file is ok to be read, for each register, the data will be associated with
    // the crime pointer and printed in the standard output destiny. In the end og the interation we
    // libarate the dynamic memory allocated in the dynamic fields of the crime pointer, without liberating it self,
    // to assure the correct use of memory in each interation of the loop.

    for(int counter = 0; counter < header->nRegFile; counter++){
        
        last_byteOffset = current_byteOffset;
        current_byteOffset += crime_reading(data_file, crime_reading_pointer);
        
        if(crime_reading_pointer->removed != '1'){
            
            if(executable->comparing_flag == false){
                // unconditional printing
                executable->print(crime_reading_pointer);
            }else if(executable->comparing_flag && executable->deletion_flag){
                
                if(executable->compare(crime_reading_pointer, executable->filter, executable->field_list, executable->n_fields) == 0){
            
                    // doing the logical deletion
                    executable->logical_deletion(last_byteOffset, data_file, crime_reading_pointer);
                    executable->results_flag = true;
                    
                    // atualizing the deleted adress vector
                    executable->deleted_data_adress =  (unsigned long int *) realloc(executable->deleted_data_adress, sizeof(unsigned long int) * (executable->general_counter + 1));
                    assert(executable->deleted_data_adress);
                    executable->deleted_data_adress[executable->general_counter] = last_byteOffset;
                    executable->general_counter++;
                }
            }else{
                if(executable->compare(crime_reading_pointer, executable->filter, executable->field_list, executable->n_fields) == 0){
                    executable->print(crime_reading_pointer);
                    executable->results_flag = true;
                }
            }
        }
    }
    
    crime_liberate_dynamic_strings(&crime_reading_pointer);
    crime_delete(&crime_reading_pointer);
    header_delete(&header);
}
  

//-----------------------------------------------------------------------------------

void crime_logical_deletion(unsigned long int byteOffset, FILE * data_file, Crime_t * crime){
    crime->removed = '1';
    fseek(data_file, byteOffset, SEEK_SET);
    crime_bin_write(data_file, crime);
}

