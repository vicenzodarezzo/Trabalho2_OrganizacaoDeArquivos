//
//  main.c
//  bStar_index
//
//  Created by Vicenzo D’Arezzo on 13/06/23.
//

#include <stdio.h>
#include <stdlib.h>
#include "include/csv_functions.h"

int main(int argc, char ** argv){
    
    int function_id;
    char bin_file_name[BUFSIZ];
    char index_file_name[BUFSIZ];
    char index_type[BUFSIZ];
    char index_field[BUFSIZ];
    int n_executions = 0;
    
    
    fscanf(stdin, "%d", &function_id);
    
    FILE * f_data;
    FILE * f_index;
    Header_t * data_header;
    
    switch(function_id){
        case 8:
            fscanf(stdin, " %s %s %s %s", bin_file_name, index_field,
                   index_type, index_file_name);

            f_data =  (FILE *) fopen(bin_file_name, "r+b");
            if(f_data == NULL){
                printf("Falha no processamento do arquivo.a\n");
                exit(0);
            }

            f_index = (FILE *) fopen(index_file_name, "w+b");
            if(f_index == NULL){
                printf("Falha no processamento do arquivo.b\n");
                exit(0);
            }

            // READING THE HEADER AND ASSERTING THE STATUS :

            data_header = header_create();
            header_reading(f_data, data_header);
            data_file_settingHeader(f_data, '0', data_header);

            create_index(f_data, f_index, index_field, data_header);

            data_file_settingHeader(f_data, '1', data_header);
            header_delete(&data_header);
            
            fclose(f_data);
            fclose(f_index);

            binarioNaTela(index_file_name);

            break;
        
        case 9:
            fscanf(stdin, " %s %s %s %s %d", bin_file_name, index_field,
                   index_type, index_file_name, &n_executions);
            
            
            f_data =  (FILE *) fopen(bin_file_name, "r+b");
            if(f_data == NULL){
                printf("Falha no processamento do arquivo.a\n");
                exit(0);
            }
            
            f_index = (FILE *) fopen(index_file_name, "r+b");
            if(f_index == NULL){
                printf("Falha no processamento do arquivo.b\n");
                exit(0);
            }
            
            data_file_settingStatus(f_data, '0');
            
            select_from(f_data, f_index, index_field, n_executions);

            data_file_settingStatus(f_data, '1');
            
            fclose(f_data);
            fclose(f_index);

            break;

        case 10:
            
            fscanf(stdin, " %s %s %s %s %d", bin_file_name, index_field,
                   index_type, index_file_name, &n_executions);

            f_data =  (FILE *) fopen(bin_file_name, "r+b");
            
            if(f_data == NULL){
                printf("Falha no processamento do arquivo.a\n");
                exit(0);
            }

            f_index = (FILE *) fopen(index_file_name, "r+b");
            if(f_index == NULL){
                printf("Falha no processamento do arquivo.b\n");
                exit(0);
            }
            
            // REDING THE HEADERS AND ASSERTING THE STATUS :
            
            data_header = header_create();
            header_reading(f_data, data_header);
            
            data_file_settingHeader(f_data, '0', data_header);

            insert_into(f_data, f_index, index_field, n_executions, data_header);

            data_file_settingHeader(f_data, '1', data_header);
            header_delete(&data_header);
            
            fclose(f_data);
            fclose(f_index);
            
            binarioNaTela(bin_file_name);
            binarioNaTela(index_file_name);

            break;
    }
    
    return 0;
        
}
