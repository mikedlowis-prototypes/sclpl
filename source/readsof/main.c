#include <stdio.h>
#include <libsof.h>

#define GET_VERSION_YEAR(version)  (version >> 16)
#define GET_VERSION_MONTH(version) ((version >> 8) & 0xFF)
#define GET_VERSION_DAY(version)   (version & 0xFF)

void print_obj(sof_file_t* obj);
void print_hex(char const* header, uint8_t const* buffer, size_t length);

void create_obj_file(char* fname)
{
    sof_file_t* obj = libsof_new_obj();

    libsof_add_symbol(obj, "foo", 0x11223344, 0x22222222, 0x33333333);

    uint8_t data[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
    libsof_add_data(obj, data, 16);
    libsof_add_data(obj, data, 16);
    libsof_add_data(obj, data, 16);

    uint32_t code[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
    libsof_add_code(obj, code, 16);
    libsof_add_code(obj, code, 16);
    libsof_add_code(obj, code, 16);

    libsof_write_obj(obj,fname);
    libsof_free_obj(obj);
}

void print_obj(sof_file_t* obj)
{
    /* print header metadata */
    printf("SOF Version:\t%#x (%d/%d/%d)\n",
           obj->header->version,
           GET_VERSION_DAY(obj->header->version),
           GET_VERSION_MONTH(obj->header->version),
           GET_VERSION_YEAR(obj->header->version));
    printf("Symbol Table:\t%d bytes\n", obj->header->sym_tbl_sz);
    printf("String Table:\t%d bytes\n", obj->header->sym_str_tbl_sz);
    printf("Data Size:\t%d bytes\n",    obj->header->data_sz);
    printf("Code Size:\t%d bytes\n",    obj->header->code_sz);

    /* print symbol table */
    printf("\nIndex\tValue\t\tSize\t\tInfo\t\tName\n");
    for(size_t i = 0; i < libsof_get_num_symbols(obj); i++)
    {
        sof_st_entry_t const* symbol = libsof_get_st_entry(obj,i);
        char const* name = libsof_get_string(obj,symbol->name);
        printf("[%d]\t%#x\t%#x\t%#x\t%s\n", i, symbol->value, symbol->size, symbol->info, name);
    }

    /* print segments as hex listing */
    print_hex("Data Segment", (uint8_t const *)libsof_get_data(obj,0), libsof_get_data_segment_size(obj));
    print_hex("Code Segment", (uint8_t const *)libsof_get_code(obj,0), libsof_get_code_segment_size(obj));
}

void print_hex(char const* header, uint8_t const* buffer, size_t length)
{
    printf("\n\n%s\n",header);
    printf("----------------------------------------------------------");
    for(size_t i = 0; i < length; i++)
    {
        if ((i%16) == 0)
        {
            printf("\n0x%04x\t", i);
        }
        else if ((i%4) == 0)
        {
            printf(" ");
        }
        printf("%02x ", buffer[i]);
    }
}

int main(int argc, char** argv)
{
    if (argc == 1)
    {
        printf("%s: no input files.\n", argv[0]);
    }
    else
    {
        create_obj_file(argv[1]);
        for (uint32_t i = 1; i < argc; i++)
        {
            printf("\nFilename:\t%s\n", argv[i]);
            sof_file_t* obj = libsof_read_obj(argv[i]);
            print_obj(obj);
            libsof_free_obj(obj);
        }
    }
}

