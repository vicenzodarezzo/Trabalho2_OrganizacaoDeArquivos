/* Vicenzo D'Arezzo Zilio - 13671790 */
/* INFORMACOES JOAO */
/* Files Organization - 2023 */



#include "csvFunctions.h"

int main(int argc, char ** argv){
    
    int function_id;
    char text_file_name[BUFSIZ];
    char bin_file_name[BUFSIZ];
    char index_file_name[BUFSIZ];
    char index_type[BUFSIZ];
    char index_field[BUFSIZ];
    int n_search = 0;
    
    
    fscanf(stdin, "%d", &function_id);
    
    
    FILE * f_text;
    FILE * f_bin;
    FILE * f_index;
    
    switch(function_id){
            
        case 1:
            fscanf(stdin, " %s %s", text_file_name, bin_file_name);
            f_text = (FILE *) fopen(text_file_name, "r");
            if(f_text == NULL){
                printf("Falha no processamento do arquivo.");
                exit(0);
            }
            f_bin =  (FILE *) fopen(bin_file_name, "w+b");
            assert(f_bin);
            
            create_table(f_text, f_bin);
            
            fclose(f_text);
            fclose(f_bin);
            
            binarioNaTela(bin_file_name);
            
            break;
        case 2:
            fscanf(stdin, " %s", bin_file_name);
            f_bin = (FILE *) fopen(bin_file_name, "rb");
            if(f_bin == NULL){
                printf("Falha no processamento do arquivo.");
                exit(0);
            }
            
            select_all(f_bin);
            
            fclose(f_bin);
            break;
            
        case 3:
            fscanf(stdin, " %s %s %s %s", bin_file_name, index_field, index_type, index_file_name);
            
            f_bin =  (FILE *) fopen(bin_file_name, "rb");
            if(f_bin == NULL){
                printf("Falha no processamento do arquivo.");
                exit(0);
            }
            
            f_index = fopen(index_file_name, "wb");
            assert(f_index);
            
            create_index(f_bin, index_field, f_index);
            
            fclose(f_bin);
            fclose(f_index);
            
            binarioNaTela(index_file_name);
            
            break;
            
        case 4:
            
            fscanf(stdin, " %s %s %s %s %d", bin_file_name, index_field,
                   index_type, index_file_name, &n_search);
            
            f_bin =  (FILE *) fopen(bin_file_name, "rb");
            if(f_bin == NULL){
                printf("Falha no processamento do arquivo.a");
                exit(0);
            }
            
            f_index = fopen(index_file_name, "rb");
            if(f_index == NULL){
                printf("Falha no processamento do arquivo.b");
                exit(0);
            }
            
            select_from(f_bin, f_index, index_field, n_search);
            fclose(f_bin);
            fclose(f_index);
            
            break;
            
        case 5:
            fscanf(stdin, " %s %s %s %s %d", bin_file_name, index_field,
                   index_type, index_file_name, &n_search);
            
            f_bin =  (FILE *) fopen(bin_file_name, "r+b");
            if(f_bin == NULL){
                printf("Falha no processamento do arquivo.a");
                exit(0);
            }
            
            f_index = fopen(index_file_name, "r+b");
            if(f_index == NULL){
                printf("Falha no processamento do arquivo.b");
                exit(0);
            }
            
            delete_from(f_bin, f_index, index_field, n_search, index_file_name);
            
            fclose(f_bin);
            fclose(f_index);
            
            binarioNaTela(bin_file_name);
            binarioNaTela(index_file_name);
            
            break;
            
        case 6:
            fscanf(stdin, " %s %s %s %s %d", bin_file_name, index_field,
                   index_type, index_file_name, &n_search);
            
            f_bin =  (FILE *) fopen(bin_file_name, "r+b");
            if(f_bin == NULL){
                printf("Falha no processamento do arquivo.a");
                exit(0);
            }
            
            f_index = fopen(index_file_name, "r+b");
            if(f_index == NULL){
                printf("Falha no processamento do arquivo.b");
                exit(0);
            }
            
            inser_into(f_bin, f_index, index_file_name, index_field, n_search);
            
            fclose(f_bin);
            fclose(f_index);
            
            binarioNaTela(bin_file_name);
            binarioNaTela(index_file_name);
            
            break;
            
        default:
            printf("ERROR\n");
            break;
            
        case 7:
            printf("Esta função não está funcionando.\n");
            printf("O exemplo de nossa implementação está nesse código comentada.\n");
            break;
    }
    
    return 0;
}
