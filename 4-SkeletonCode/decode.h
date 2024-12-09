#ifndef DECODE_H
#define DECODE_H

#include "types.h"
#include "stdio.h"

typedef struct DecodeInfo{
    //stego file
    char *stego_fname;
    FILE *fptr_stego_image;

    //output file
    char output_fname[100];
    FILE *fptr_output_file;

}DecodeInfo;

Status read_and_validate_dncode_args(char *argv[], DecodeInfo *dncInfo);
Status open_files_output(DecodeInfo *dncInfo);

Status do_decoding(DecodeInfo *dncInfo);

Status skip_header_bytes(DecodeInfo *dncInfo);

Status decode_magic_string(const char *magic_string, DecodeInfo *dncInfo);
Status decode_byte_to_lsb(char *image_buffer);
Status decode_secret_file_extn( DecodeInfo *dncInfo);
Status decode_size_to_lsb(char *image_buffer);
Status decode_secret_file_data(DecodeInfo *dncInfo);

#endif
