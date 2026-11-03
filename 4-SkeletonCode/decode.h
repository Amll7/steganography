#ifndef DECODE_H
#define DECODE_H

#include "types.h" // Contains user defined types

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4


typedef struct _DecodeInfo
{
    char *src_image_fname;
    FILE *fptr_src_image;
    char image_data[MAX_IMAGE_BUF_SIZE];

    /* Secret File Info */
    char secret_fname[30];
    FILE *fptr_secret;
    char extn_secret_file[MAX_FILE_SUFFIX];
    long size_secret_file;
    int secret_file_extn_size;

}DecodeInfo;


/* Decoding function prototype */

/* Read and validate dec args from argv */
Status read_and_validate_dec_args(char *argv[], DecodeInfo *decInfo);

Status do_decoding(DecodeInfo *decInfo);

Status skip_bmp_header(FILE *fptr_src_image);

Status dec_magic_string(char *magic_string, DecodeInfo *decInfo);

Status dec_secret_file_extn(DecodeInfo *decInfo);

Status dec_secret_file_extn_size(DecodeInfo *decInfo);

Status dec_secret_file_size(DecodeInfo *decInfo);

Status dec_secret_file_data(DecodeInfo *decInfo);

Status dec_byte_from_lsb(char *data, char *buff);

Status dec_size_from_lsb(int *size, char *buff);

#endif
