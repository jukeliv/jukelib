#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/*
    # Vector Builder by Máximo Carrasco
     A little metaprogramming console app that just makes you a file where there are declaration + definitions of a dynamic array of a user-defined datatype!
    # Usage
     build the file: cc vector_builder.c -o vector_builder
     execute: ./vector_builder <datatype> <output>.h
*/

size_t lenstr(char* str) {
    size_t i = 0;
    while(str[i++]);
    return i;
}

void memput(char* dst, size_t dst_len, char* src, size_t src_len, size_t i) {
    size_t j = i;
    while(j < dst_len) {
        dst[j] = src[j-i];
        ++j;
    }
}

char uppercase_char(char c) {
    if (c >= 'a' && c <= 'z')
        c = c-'a'+'A';
    return c;
}

void touppercase(char* str, size_t str_len) {
    size_t i = 0;
    while (i < str_len){
        str[i] = uppercase_char(str[i]);
        i++;
    }
}

char* copystr(char* src, size_t src_len) {
    char* buf = (char*)calloc(src_len,sizeof(char));
    assert(buf && "BUY MORE RAM!");
    size_t i = 0;
    while(i < src_len) {
        buf[i] = src[i];
        i++;
    }
    return buf;
}

void USAGE(char* program) {
    printf("Usage: %s <datatype> <output>.h\n", program);
}

int main(int argc, char** argv) {
    if(argc < 2) {
        fprintf(stderr, "ERROR: No input type passed!\n");
        USAGE(argv[0]);
        return 1;
    }
    if(argc < 3) {
        fprintf(stderr, "ERROR: No output file passed!\n");
        USAGE(argv[0]);
        return 1;
    }
    FILE* fp = fopen(argv[2], "w+");
    if(!fp) {
        fprintf(stderr, "ERROR: Couldn't open output file!\n");
        return 1;
    }

    size_t type_len = lenstr(argv[1]);

    size_t header_safe_guards_size = 4+type_len+2;
    char* header_safe_guards = calloc(header_safe_guards_size, sizeof(char)+1);
    assert(header_safe_guards && "BUY MORE RAM!");

    char* uppercase_type = copystr(argv[1], type_len);
    touppercase(uppercase_type, type_len);

    memput(header_safe_guards, header_safe_guards_size, "VEC_", 4, 0);
    memput(header_safe_guards, header_safe_guards_size, uppercase_type, type_len, 4);
    memput(header_safe_guards, header_safe_guards_size, "_H", 2, header_safe_guards_size-2);

    fprintf(fp, "#include <stdlib.h>\n\n");

    fprintf(fp, "#ifndef %*s\n", header_safe_guards_size, header_safe_guards);
    fprintf(fp, "#define %*s\n\n", header_safe_guards_size, header_safe_guards);

    size_t vector_type_size = 4+type_len;
    char* vector_type = calloc(vector_type_size, sizeof(char));
    assert(vector_type && "BUY MORE RAM!");

    memput(vector_type, vector_type_size, "Vec_", 4, 0);
    memput(vector_type, vector_type_size, argv[1], type_len, 4);

    vector_type[4] = uppercase_char(vector_type[4]);

    fprintf(fp, "typedef struct {\n");
    fprintf(fp, "\tsize_t size;\n");
    fprintf(fp, "\tsize_t cap;\n");
    fprintf(fp, "\t%s* content;\n", argv[1]);
    fprintf(fp, "}%*s;\n\n", vector_type_size, vector_type);

    fprintf(fp, "void %*s_Initalize(%*s* array);\n", vector_type_size, vector_type, vector_type_size, vector_type);
    fprintf(fp, "void %*s_Push(%*s* array, %s element);\n", vector_type_size, vector_type, vector_type_size, vector_type, argv[1]);
    fprintf(fp, "void %*s_Drop(%*s* array, size_t index);\n", vector_type_size, vector_type, vector_type_size, vector_type);
    fprintf(fp, "void %*s_Insert(%*s* array, %s element, size_t index);\n\n", vector_type_size, vector_type, vector_type_size, vector_type, argv[1]);

    fprintf(fp, "void %*s_Initalize(%*s* array)\n", vector_type_size, vector_type, vector_type_size, vector_type);
    fprintf(fp, "{\n");
    fprintf(fp, "\tarray->size = 0;\n");
    fprintf(fp, "\tarray->cap = 1;\n");
    fprintf(fp, "\tarray->content = (%s*)malloc(array->cap*sizeof(%s));\n", argv[1], argv[1]);
    fprintf(fp, "}\n\n");

    fprintf(fp, "void %*s_Push(%*s* array, %s element)", vector_type_size, vector_type, vector_type_size, vector_type, argv[1]);
    fprintf(fp, "{\n");
    fprintf(fp, "\tif(array->cap <= array->size) {\n");
    fprintf(fp, "\t\tarray->cap*=2;\n");
    fprintf(fp, "\t\tarray->content = (%s*)realloc(array->content, array->cap*sizeof(%s));\n", argv[1], argv[1]);
    fprintf(fp, "\t}\n");
    fprintf(fp, "\tarray->content[array->size++] = element;\n");
    fprintf(fp, "}\n\n");

    fprintf(fp, "%s %*s_Pop(%*s* array)\n", argv[1], vector_type_size, vector_type, vector_type_size, vector_type);
    fprintf(fp, "{\n");
    fprintf(fp, "\tarray->content[array->size--];\n");
    fprintf(fp, "}\n\n");

    fprintf(fp, "void %*s_Drop(%*s* array, size_t index)\n", vector_type_size, vector_type, vector_type_size, vector_type);
    fprintf(fp, "{\n");
    fprintf(fp, "\tsize_t i = index;\n");
    fprintf(fp, "\twhile (i < array->size) {\n");
    fprintf(fp, "\t\tarray->content[i] = array->content[++i];\n");
    fprintf(fp, "\t}\n");
    fprintf(fp, "\tarray->size--;\n");
    fprintf(fp, "}\n\n");

    fprintf(fp, "void %*s_Insert(%*s* array, %s element, size_t index)\n", vector_type_size, vector_type, vector_type_size, vector_type, argv[1]);
    fprintf(fp, "{\n");
    fprintf(fp, "\tif (array->size >= array->cap) {\n");
    fprintf(fp, "\t\tarray->cap *= 2;\n");
    fprintf(fp, "\t\tarray->content = (%s*)realloc(array->content, array->cap * sizeof(%s));\n", argv[1], argv[1]);
    fprintf(fp, "\t}\n");
    fprintf(fp, "\n");
    fprintf(fp, "\tfor (size_t i = array->size; i > index; --i) {\n");
    fprintf(fp, "\t\tarray->content[i] = array->content[i - 1];\n");
    fprintf(fp, "\t}\n");
    fprintf(fp, "\n");
    fprintf(fp, "\tarray->content[index] = element;\n");
    fprintf(fp, "\t++array->size;\n");
    fprintf(fp, "}\n\n");

    fprintf(fp, "#endif//%*s\n", header_safe_guards_size, header_safe_guards);
    return 0;
}
