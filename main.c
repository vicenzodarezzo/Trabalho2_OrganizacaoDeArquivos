//
//  main.c
//  bStar_index
//
//  Created by Vicenzo D’Arezzo on 13/06/23.
//

#include "csv_functions.h"

int main(int argc, char ** argv){
    
    int function_id;
    char bin_file_name[BUFSIZ];
    char index_file_name[BUFSIZ];
    char index_type[BUFSIZ];
    char index_field[BUFSIZ];
    int n_search = 0;
    
    
    fscanf(stdin, "%d", &function_id);
    
    FILE * f_data;
    FILE * f_index;
    
    switch(function_id){
        case 9:
            fscanf(stdin, " %s %s %s %s %d", bin_file_name, index_field,
                   index_type, index_file_name, &n_search);
            
            
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
            
            select_from(f_data, f_index, index_field, n_search);
            
            data_file_settingStatus(f_data, '1');
            
            fclose(f_data);
            fclose(f_index);
    }
    
    return 0;
        
}
