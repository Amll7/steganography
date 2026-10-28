#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#define MAGIC_STRING "#*"
/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */

uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

/* 
 * 
 * 
 * 
 * 
 * 
 */


Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
  /* source file validation
   *opening the source file using fopen in read mode (argv[2]=sourcefile name)
   *if it return null adress the file is not present
   *if the file is present,
   *check if the file name contain .bmp
   *if the bmp extension is present then store the file name to src_image_fname
  */


  encInfo->fptr_src_image=fopen(argv[2],"r");
  if(encInfo->fptr_src_image==NULL)
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
            encInfo->src_image_fname=argv[2];
        }
    }
  }




  /* secret file validation
   *opening the secret file using fopen in read mode (argv[3]=secretfile name)
   *if it return null adress the file is not present
   *if the file is present,
   *store the name in stego_image_fname
   *print validation sucess
  */

  encInfo->fptr_secret=fopen(argv[3],"r");
  if(encInfo->fptr_secret==NULL)
  {
    printf("ERROR: Unable to open secret file.\n");
    return e_failure;
  }
  else
  {
    encInfo->secret_fname=argv[3];
    printf("INFO: Secret file opened successfully.\n");
  }



/* output file validation
   *opening the output file using fopen in read mode (argv[4]=outputfile name)
   *if it return null adress the file is not present
   *if not present,
   *create a default file for the output by using fopen in write mode.
   *and store it in stego_image_fname
   *if the file is present,
   *check if the file name contain .bmp
   *if the bmp extension is present then store the file name to stego_image_fname
  */
encInfo->fptr_stego_image=fopen(argv[4],"r");
if(encInfo->fptr_stego_image==NULL)
{
  printf("INFO: Output file not specified. Creating default output file.\n");
   encInfo->stego_image_fname="default.bmp";
   encInfo->fptr_stego_image=fopen(encInfo->stego_image_fname,"w");
   if(encInfo->fptr_stego_image==NULL)
   {
    printf("ERROR: Failed to create default output image file.\n");
    return e_failure;
   }
   else
   {
    printf("SUCCESS: Default output image file created successfully.\n");
    return e_success;
   }
}
else
{
    printf("INFO: Output image file opened successfully.\n");
    char *bmp=strstr(argv[4],".bmp");
    if(bmp==NULL)
    {
       printf("ERROR: Invalid output image filename. Expected a .bmp extension.\n");
        return e_failure;
    }
    else
    {
        if(*(bmp+4)!='\0')
        {
           printf("ERROR: Invalid output image filename. Expected a .bmp extension.\n");
            return e_failure;
        }
        else
        {
            printf("INFO: Source image validated successfully.\n");
            encInfo->stego_image_fname=argv[4];
            return e_success;
        }
    }
  }

}


Status do_encoding(char *argv[],EncodeInfo *encInfo)
{
  char *dot=strstr(argv[3],".");
  strcpy(encInfo->extn_secret_file,dot);

  if(check_capacity(encInfo)==e_failure)
  {
    return e_failure;
  }

  if(copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_failure)
  {
    return e_failure;
  }

  if((encode_magic_string(MAGIC_STRING,encInfo))==e_failure)
  {
    return e_failure;
  }
  
  if(encode_secret_file_extn_size(strlen(encInfo->extn_secret_file),encInfo)==e_failure)
  {
    return e_failure;
  }

  if(encode_secret_file_extn(encInfo->extn_secret_file,encInfo)==e_failure)
  {
    return e_failure;
  }

  if(encode_secret_file_size(encInfo->size_secret_file,encInfo)==e_failure)
  {
    return e_failure;
  }

  if(encode_secret_file_data(encInfo)==e_failure)
  {
    return e_failure;
  }
  
  if(copy_remaining_img_data(encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_failure)
  {
    return e_failure;
  }
  printf("done\n");
}





/*check_capacity
 *The capacity of sourcefile should be greater than the size of secret file.
 *each bit of the secret file is encoded into each bytes of source (*8).
 *The size of secret includes extra 14 bytes (that are included in o/p file)
 * 2bytes- Magic string
 * 4bytes- 
 * 4bytes
 * 4bytes-
 and the secret file contents
*/
/* inside function,
 * the size of secret file it stored to size_secret_file by calling func get_file_size.
 * get size of source file by calling func get_image_size_for_bmp.
 * (size_secret_file+14)*8.
 * comparing both.
 * if get_image_size_for_bmp is greater return e_success.
 * otherwise return e_failure.
*/
Status check_capacity(EncodeInfo *encInfo)
{
  encInfo->size_secret_file=get_file_size(encInfo->fptr_secret);
  if(get_image_size_for_bmp(encInfo->fptr_src_image)>((encInfo->size_secret_file+14)*8))
  {
    printf("INFO: Source image has sufficient capacity for encoding.\n");
    return e_success;
  }
  else
  {
    printf("ERROR: Insufficient image capacity to encode the secret file.\n");
    return e_failure;
  }
}


/*get_file_size
 *getting the secretfile size 
 *set the fptr at the end using fseek(SEEK_END)
 *by using ftell return the position of the fptr
*/
uint get_file_size(FILE *fptr)
{
  fseek(fptr,0,SEEK_END);
  return ftell(fptr);
}


/*copy_bmp_header
 *the source image contains 54 bytes header in the starting
 *so those 54 bytes should be copied to the o/p file
*/
/*inside function
 *set the file pointer to the starting using rewind
 *create a char buff of 54 byte
 *read 54 bytes from source image using fread
 *write that 54 bytes to the destination which is fptr_stego_image.
*/
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
  rewind(fptr_src_image);
  char buff[54];
  if(fread(buff,54,1,fptr_src_image)!=1)
  {

    printf("ERROR: Failed to read BMP image header.\n");
    return e_failure;
  }
  if(fwrite(buff,54,1,fptr_dest_image)!=1)
  {

    printf("ERROR: Failed to write BMP image header to output file.\n");
    return e_failure;
  }

  printf("INFO: BMP image header copied successfully.\n");
  return e_success;
}


/*encode_magic_string
 *we need to encode the magic string to the o/p image.
 *inside func,
 *create a char buff of 8 bytes
 *read 8 bytes from source
 *we need to encode this 8 bits of each character of magic string to 
  lsb of each bytes of source using encode_byte_to_lsb
 *after encoding to lsb, write this to o/p image.
 *this should be done for all characters of magic string.

*/
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
  char buff[8];
  for(int i=0;i<strlen(magic_string);i++)
  {

    if(fread(buff,8,1,encInfo->fptr_src_image)!=1)
    {

      printf("ERROR: Failed to read source image data during magic string encoding.\n");
      return e_failure;

    }

    encode_byte_to_lsb(magic_string[i],buff);

    if(fwrite(buff,8,1,encInfo->fptr_stego_image)!=1)
    {

      printf("ERROR: Failed to write encoded magic string to output image.\n");
      return e_failure;

    }
  }

  printf("INFO: Magic string encoded successfully.\n");
  return e_success;
  

}


/*encode_byte_to_lsb
 *this function is used to encode each bits of the data 
  to lsb of each byte of the source
 *inside func,
  encoding is starting from the MSB.
 *if the bit of the data is set,then we will set in the lsb of image buffer
  else we will clear the lsb of image buffer.
*/
Status encode_byte_to_lsb(char data,char *image_buffer)
{
for(int i=7;i>=0;i--)
{
  if(data&(1U<<i))
  {
    image_buffer[i]=image_buffer[i]|1;
  }
  else
  {
    image_buffer[i]=image_buffer[i]&(~1);
  }
}
 return e_success;

}


/*encode_secret_file_extn
 *we have to encode the secret file extension to the o/p image.
 *inside func,
 *create a char buff of 8 bytes.
 *find the length of file extension.
 *read 8 bytes from source.
 *encode byte to lsb.
 *write the buff to the o/p image.
 *this should be done for all the character of extension
 *so should be done inside a loop for length of file_extn.
*/
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
  char buff[8];
  int length=strlen(file_extn);
  for(int i=0;i<length;i++)
  {

    if(fread(buff,8,1,encInfo->fptr_src_image)!=1)
    {

      printf("ERROR: Failed to read source image data during extension encoding.\n");
      return e_failure;

    }

    encode_byte_to_lsb(file_extn[i],buff);

    if(fwrite(buff,8,1,encInfo->fptr_stego_image)!=1)
    {

      printf("ERROR: Failed to write encoded file extension to output image.\n");
      return e_failure;

    }

  }
    printf("INFO: Secret file extension encoded successfully.\n");
    return e_success;
}



Status encode_secret_file_extn_size(uint size, EncodeInfo *encInfo)
{
 char buff[32];
 if(fread(buff,32,1,encInfo->fptr_src_image)!=1)
 {
    return e_failure;
 }
  encode_size_to_lsb(size,buff);

  if(fwrite(buff,32,1,encInfo->fptr_stego_image)!=1)
  {
    return e_failure;
  }
  return e_success;
}

/*encode_secret_file_size
  
*/
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
 char buff[32];
 if(fread(buff,32,1,encInfo->fptr_src_image)!=1)
 {
    return e_failure;
 }
  encode_size_to_lsb(file_size,buff);

  if(fwrite(buff,32,1,encInfo->fptr_stego_image)!=1)
  {
    return e_failure;
  }
  return e_success;
}



Status encode_size_to_lsb(uint size, char *image_buffer)
{
for(int i=31;i>=0;i--)
{
  if(size&(1U<<i))
  {
    image_buffer[i]=image_buffer[i]|1;
  }
  else
  {
    image_buffer[i]=image_buffer[i]&(~1);
  }
}
 return e_success;
}



Status encode_secret_file_data(EncodeInfo *encInfo)
{
  char buff[8];
  int i;
  char data;
  rewind(encInfo->fptr_secret);
  for(i=0;i<encInfo->size_secret_file;i++)
  {
    if(fread(buff,8,1,encInfo->fptr_src_image)!=1)
    {
    return e_failure;
    }
    data=fgetc(encInfo->fptr_secret);
    encode_byte_to_lsb(data,buff);
    if(fwrite(buff,8,1,encInfo->fptr_stego_image)!=1)
    {
      return e_failure;
    }
    
  }
  return e_success;
}


Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    int ch;
    while ((ch = fgetc(fptr_src)) != EOF)
    {
        if (fputc(ch, fptr_dest) == EOF)
        {
            printf("ERROR: Failed to copy remaining image data.\n");
            return e_failure;
        }
    }

    return e_success;
}


