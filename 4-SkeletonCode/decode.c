#include <stdio.h>
#include <string.h>
#include "decode.h"
#include "types.h"
#include "common.h"



/* read and validate decoding arguments
 * inside the function:
 * 1. open the stego image in read mode.
 * 2. check whether the image file is opened successfully.
 * 3. check whether the source image has a .bmp extension.
 * 4. store the source image name in src_image_fname.
 * 5. open the output file name provided by the user.
 * 6. store the output file name without its extension in secret_fname.
 * if any validation fails, return e_failure.
 * if all validations are successful, return e_success.
*/
Status read_and_validate_dec_args(char *argv[], DecodeInfo *decInfo)
{
  decInfo->fptr_src_image=fopen(argv[2],"r");
  if(decInfo->fptr_src_image==NULL)
  {
    printf("ERROR: Unable to open source image file.\n");
    return e_failure;
  }
  else
  {
    char *bmp=strstr(argv[2],".bmp");
    if(bmp==NULL)
    {
        printf("ERROR: Invalid source image format. Expected a .bmp file.\n");
        return e_failure;
    }
    else
    {
        if(*(bmp+4)!='\0')
        {
            printf("ERROR: Invalid source image filename. Expected a .bmp extension.\n");
            return e_failure;
        }
        else
        {
            printf("INFO: Source image validated successfully.\n");
            decInfo->src_image_fname=argv[2];
        }
    }
  }
  
  decInfo->fptr_secret=fopen(argv[3],"r");
  if(decInfo->fptr_secret==NULL)
  {
    printf("ERROR: Output secret file not present. Creating new ootput file.\n");
    strcpy(decInfo->secret_fname,"decode");
    return e_success;
  }
  else
  {
    char *tok=strtok(argv[3],".");
    strcpy(decInfo->secret_fname,tok);
    printf("INFO: Secret file opened successfully.\n");
    return e_success;
  }
}

/* do decoding
 * this function is used to perform the complete decoding process.
 * inside the function:
 * 1. skip the BMP header of the stego image.
 * 2. decode and verify the magic string.
 * 3. decode the secret file extension size.
 * 4. decode the secret file extension.
 * 5. decode the secret file size.
 * 6. decode the secret file data and store it in the output file.
 * 7. close the opened files.
 * if any function fails return e_failure.
 * if all functions are successful return e_success.
*/
Status do_decoding(DecodeInfo *decInfo)
{
 if(skip_bmp_header(decInfo->fptr_src_image)==e_failure)
 {
  return e_failure;
 }

 if(dec_magic_string(MAGIC_STRING, decInfo) == e_failure)
 {
  return e_failure;
 }

if(dec_secret_file_extn_size(decInfo) == e_failure)
{
    return e_failure;
}

if(dec_secret_file_extn(decInfo) == e_failure)
{
    return e_failure;
}

if(dec_secret_file_size(decInfo) == e_failure)
{
    return e_failure;
}

if(dec_secret_file_data(decInfo) == e_failure)
{
    return e_failure;
}

fclose(decInfo->fptr_secret);
fclose(decInfo->fptr_src_image);
printf("INFO: Decoding completed successfully.\n");
return e_success;
}

/* skip bmp header
 * the first 54 bytes of the BMP image contain the header.
 * inside the function:
 * 1. use fseek() to move the file pointer past the 54-byte header.
 * 2. if seeking fails return e_failure.
 * 3. if seeking is successful return e_success.
*/
Status skip_bmp_header(FILE *fptr_src_image)
{
  if(fseek(fptr_src_image,54,SEEK_SET)!=0)
  {

    printf("ERROR: Failed to skip BMP header.\n");
    return e_failure;

  }
  else
  {

    printf("INFO: BMP header skipped successfully.\n");
    return e_success;

  }
}



/* decode magic string
 * this function is used to decode and verify the magic string.
 * inside the function:
 * 1. create a buffer of 8 bytes to read image data.
 * 2. read 8 bytes from the stego image for each character.
 * 3. decode one character from the LSBs using dec_byte_from_lsb().
 * 4. compare the decoded character with the magic string.
 * if the characters do not match or reading fails return e_failure.
 * if the complete magic string matches return e_success.
*/
Status dec_magic_string(char *magic_string, DecodeInfo *decInfo)
{
  char buff[8];
  char decode;

  for(int i=0;i<strlen(magic_string);i++)
  {
    if(fread(buff,8,1,decInfo->fptr_src_image)!=1)
    {

      printf("ERROR: Failed to read source image data during magic string encoding.\n");
      return e_failure;

    }
    else
    {
        dec_byte_from_lsb(&decode,buff);
        if(decode !=magic_string[i])
        {

            printf("ERROR: Magic string doesn't match. The image does not contain hidden data.\n");
            return e_failure;

        }
    }
  }
  return e_success;
}




/* decode byte from lsb
 * this function is used to get one character from the image buffer.
 * inside the function:
 * 1. initialize data to zero.
 * 2. run a loop for 8 bits.
 * 3. extract the LSB from each byte of the image buffer.
 * 4. place each extracted bit in its correct position in data.
 * 5. return the decoded character through the data pointer.
*/
Status dec_byte_from_lsb(char *data,char *buff)
{
    *data = 0;

    for(int k=0;k<8;k++)
    {
        if(buff[k]&1)
        {
          *data=*data|(1<<(k));
        }
    }

    return e_success;
}




/* decode secret file extension size
 * this function is used to decode the length of the secret file extension.
 * inside the function:
 * 1. create a buffer of 32 bytes.
 * 2. read 32 bytes from the stego image.
 * 3. decode the size from the LSBs using dec_size_from_lsb().
 * 4. store the decoded size in secret_file_extn_size.
 * if reading fails return e_failure.
 * else return e_success.
*/
Status dec_secret_file_extn_size(DecodeInfo *decInfo)
{
    char buff[32];
    int size;

    if(fread(buff,32,1,decInfo->fptr_src_image)!=1)
    {

        printf("ERROR: Failed to read bytes from source image.\n");
        return e_failure;

    }
    dec_size_from_lsb(&size,buff);
    decInfo->secret_file_extn_size=size;

    printf("INFO: Secret file extension size decoded successfully.\n");
    return e_success;
}




/* decode size from lsb
 * this function is used to decode a 32-bit size value from the image buffer.
 * inside the function:
 * 1. initialize size to zero.
 * 2. run a loop for 32 bits.
 * 3. extract the LSB from each byte of the image buffer.
 * 4. place each extracted bit in its correct position in size.
 * 5. return the decoded size through the size pointer.
*/
Status dec_size_from_lsb(int *size,char *buff)
{
    *size=0;
    for(int i=0;i<32;i++)
    {
        if(buff[i]&1)
        {
          *size=*size|(1<<(i));
        }
    }

    return e_success;
}





/* decode secret file extension
 * this function is used to extract the secret file extension from the image.
 * inside the function:
 * 1. create an array to store the decoded extension.
 * 2. create a buffer of 8 bytes to read image data.
 * 3. read 8 bytes from the stego image for each extension character.
 * 4. decode each character using dec_byte_from_lsb().
 * 5. add the null character at the end of the extension.
 * 6. append the extension to the output file name.
 * 7. create the output file using fopen().
 * if reading or opening the output file fails return e_failure.
 * else return e_success.
*/
Status dec_secret_file_extn(DecodeInfo *decInfo)
{
    
    char extension[decInfo->secret_file_extn_size+1];
    char buff[8];

    for(int i=0;i<decInfo->secret_file_extn_size;i++)
    {
        if(fread(buff,8,1,decInfo->fptr_src_image)!=1)
        {

            printf("ERROR: Failed to read bytes from source image.\n");
            return e_failure;

        }

        dec_byte_from_lsb(&extension[i],buff);
    }
    extension[decInfo->secret_file_extn_size]='\0';
    strcat(decInfo->secret_fname,extension);

    decInfo->fptr_secret=fopen(decInfo->secret_fname,"w");
    if(decInfo->fptr_secret==NULL)
    {

        printf("ERROR: Failed to create output file.\n");
        return e_failure;

    }
    printf("INFO: Secret file extension decoded successfully.\n");
    return e_success;
}





/* decode secret file size
 * this function is used to decode the original size of the secret file.
 * inside the function:
 * 1. create a buffer of 32 bytes.
 * 2. read 32 bytes from the stego image.
 * 3. decode the file size from the LSBs using dec_size_from_lsb().
 * 4. store the decoded size in size_secret_file.
 * if reading fails, return e_failure.
 * else return e_success.
*/
Status dec_secret_file_size(DecodeInfo *decInfo)
{
    int size;
    char buff[32];

    if(fread(buff,32,1,decInfo->fptr_src_image)!=1)
    {

      printf("ERROR: Failed to read bytes from source image.\n");
      return e_failure;

    }
    dec_size_from_lsb(&size,buff);
    decInfo->size_secret_file=size;

    printf("INFO: Secret file size decoded successfully.\n");
    return e_success;
}






/* decode secret file data
 * this function is used to extract the actual contents of the secret file.
 * inside the function:
 * 1. create a buffer of 8 bytes to read image data.
 * 2. run a loop for the size of the secret file.
 * 3. read 8 bytes from the stego image.
 * 4. decode one character using dec_byte_from_lsb().
 * 5. write the decoded character into the output file.
 * if reading or writing fails return e_failure.
 * after decoding all the data return e_success.
*/
Status dec_secret_file_data(DecodeInfo *decInfo)
{
    char buff[8];
    char data;

    for(int i=0;i<decInfo->size_secret_file;i++)
    {
        if(fread(buff,8,1,decInfo->fptr_src_image)!=1)
        {

         printf("ERROR: Failed to read bytes from source image.\n");
         return e_failure;

        }

        dec_byte_from_lsb(&data,buff);

        if(fwrite(&data,1,1,decInfo->fptr_secret)!=1)
        {

          printf("ERROR: Failed to write decoded data into output file.\n");
          return e_failure;

        }
        
    }

    printf("INFO: Secret file data decoded successfully.\n");
    return e_success;
}

