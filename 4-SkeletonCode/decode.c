#include <stdio.h>
#include <string.h>
#include "decode.h"
#include "types.h"
#include "common.h"

Status open_files_output(DecodeInfo *dncInfo)
{
    dncInfo->fptr_output_file = fopen(dncInfo->output_fname, "w");
    // Do Error handling
    if (dncInfo->fptr_output_file == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", dncInfo->output_fname);

        return e_failure;
    }
    return e_success;
}

Status open_files_stego(DecodeInfo *dncInfo)
{
    dncInfo->fptr_stego_image = fopen(dncInfo->stego_fname, "r");
    // Do Error handling
    if (dncInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", dncInfo->stego_fname);

        return e_failure;
    }

    return e_success;
}

Status read_and_validate_dncode_args(char *argv[], DecodeInfo *dncInfo)
{
    if (strstr(argv[2], ".bmp") != NULL && strcmp(strstr(argv[2], ".bmp"), ".bmp") == 0)
    {
        dncInfo->stego_fname = argv[2];
        printf("source file set to : %s\n", dncInfo->stego_fname);
    }
    else
    {
        return e_failure;
    }
    if (argv[3] == NULL)
    {
        strcpy(dncInfo->output_fname, "output");
        printf("source file set to : %s\n", dncInfo->output_fname);
    }
    else
    {
        if (strstr(argv[3], ".txt") != NULL && strcmp(strstr(argv[3], ".txt"), ".txt") == 0)
        {
            dncInfo->stego_fname = argv[3];
            printf("source file set to : %s\n", dncInfo->output_fname);
        }
        else
        {
            strcpy(dncInfo->output_fname, "output");
            printf("source file set to : %s\n", dncInfo->output_fname);
        }
    }
    return e_success;
}

Status do_decoding(DecodeInfo *dncInfo)
{
    int ret1 = open_files_stego(dncInfo);
    if (ret1 == e_failure)
    {
        printf("Files are cannot open\n");
        return 0;
    }
    printf("Files are opened sucessfully\n");
    if (skip_header_bytes(dncInfo) != e_success)
    {
        printf("Skiping the header bytes unsuccessfully\n");
        return e_failure;
    }
    else
    {
        printf("Skiping the header bytes sucessfully\n");
    }
    int ret = decode_magic_string(MAGIC_STRING, dncInfo);
    if (ret == e_success)
    {
        printf("Magic string decoded sucessfully\n");
        ret = decode_secret_file_extn(dncInfo);
        if (ret == e_success)
        {
            printf("Secret file extension decoded sucessfully\n");
            if (decode_secret_file_data(dncInfo) == e_success)
            {
                printf("Decoded secret file data succesfully\n");
            }
            else
            {
                printf("Decoded secret file data unsuccesfully\n");
                return e_failure;
            }
        }
        else
        {
            printf("Magic string decoded unsucessfully\n");
            return e_failure;
        }
    }
    return e_success;
}
//skip the 54 bytes heder bytes using fseek
Status skip_header_bytes(DecodeInfo *dncInfo)
{
    fseek(dncInfo->fptr_stego_image, 54, SEEK_SET);
    return e_success;
}
//decode magic string
Status decode_magic_string(const char *magic_string, DecodeInfo *dncInfo)
{
    char MAGIC_string[3] = {'\0'};
    char image_buffer[8];

    for (int i = 0; i < strlen(magic_string); i++)
    {
        //read the 8 byte from stego iimage
        if (fread(image_buffer, 8, 1, dncInfo->fptr_stego_image) != 1)
        {
            printf("Error in decoding magic string\n");
            return e_failure;
        }
        //decode byte to lsb image_buffer
        MAGIC_string[i] = decode_byte_to_lsb(image_buffer);
    }
    
    if (strcmp(magic_string, MAGIC_string) == '\0')
    {
        printf("magic string and decoded string matched\n");
        return e_success;
    }
    printf("magic string and decoded string unmatched\n");
    return e_success;
}

Status decode_byte_to_lsb(char *image_buffer)
{
    char ch = 0;
    for (int i = 0; i < 8; i++)
    {
        ch = ch | ((image_buffer[i] & 1) << (7 - i));
    }
    return ch;
}

Status decode_secret_file_extn(DecodeInfo *dncInfo)
{
    char image_buffer[32];
    if (fread(image_buffer, 32, 1, dncInfo->fptr_stego_image) != 1)
    {
        printf("error in reading stego file\n");
        return e_failure;
    }
    int ext_size = decode_size_to_lsb(image_buffer);
    printf("Extension size of secrett file : %d\n", ext_size);
    char file_extn[ext_size + 1];
    file_extn[ext_size] = '\0';
    for (int i = 0; i < ext_size; i++)
    {
        if (fread(image_buffer, 8, 1, dncInfo->fptr_stego_image) != 1)
        {
            printf("error in reading stego file\n");
            return e_failure;
        }
        file_extn[i] = decode_byte_to_lsb(image_buffer);
    }

    strcat(dncInfo->output_fname, file_extn);
    int ret = open_files_output(dncInfo);
    if (ret == e_success)
    {
        printf("file opened sucessfully : %s\n", dncInfo->output_fname);
        return e_success;
    }
    return e_failure;
}

Status decode_size_to_lsb(char *image_buffer)
{
    int size = 0;
    for (int i = 0; i < 32; i++)
    {
        size = size | ((image_buffer[i] & 1) << (31 - i));
    }
    return size;
}

Status decode_secret_file_data(DecodeInfo *dncInfo)
{
    char image_buffer[32];
    char data[8];
    if (fread(image_buffer, 32, 1, dncInfo->fptr_stego_image) != 1)
    {
        printf(" 1 error in reading stego file\n");
        return e_failure;
    }
    int file_size = decode_size_to_lsb(image_buffer);
    printf("Fiile size of secret file : %d\n", file_size);
    char secret_data[file_size + 1];
    for (int i = 0; i < file_size; i++)
    {
        if (fread(data, 8, 1, dncInfo->fptr_stego_image) != 1)
        {
            printf("error in reading stego file\n");
            // return e_failure;
        }
        secret_data[i] = decode_byte_to_lsb(data);
        
    }
    if (fwrite(secret_data, file_size, 1, dncInfo->fptr_output_file) != 1)
        {
            printf("error in writing  output file\n");
            return e_failure;
        }
        return e_success;
}