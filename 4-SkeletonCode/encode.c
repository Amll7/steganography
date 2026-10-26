#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"

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
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }

    // No failure return e_success
    return e_success;
}
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
  //source file
  encInfo->fptr_src_image=fopen(argv[2],"rb");
  if(encInfo->fptr_src_image==NULL)
  {
    printf("failure no file");
    return e_failure;
  }
  else
  {
    char *bmp=strstr(argv[2],".bmp");
    if(bmp==NULL)
    {
        printf("failure no bmp");
        return e_failure;
    }
    else
    {
        if(*(bmp+4)!='\0')
        {
            printf("fail no null");
            return e_failure;
        }
        else
        {
            printf("success");
            encInfo->src_image_fname=argv[2];
        }
    }
  }


  //secretfile
  encInfo->fptr_secret=fopen(argv[3],"r");
  if(encInfo->fptr_secret==NULL)
  {
    printf("fail");
    return e_failure;
  }
  else
  {
    printf("sucess");
  }

//output file
encInfo->fptr_stego_image=fopen(argv[4],"r");
if(encInfo->fptr_stego_image==NULL)
{
   printf("default");
   encInfo->stego_image_fname="default.bmp";
   printf("%s",encInfo->stego_image_fname);
}
else
{
  encInfo->stego_image_fname=argv[4];
  printf("sucess");
  printf("%s",encInfo->stego_image_fname);
}

}
Status do_encoding(EncodeInfo *encInfo)
{
  check_capacity(encInfo)
}
Status check_capacity(EncodeInfo *encInfo)
{
  encInfo->size_secret_file=get_file_size(encInfo->fptr_secret);
  if(get_image_size_for_bmp(encInfo->fptr_src_image)>((encInfo->size_secret_file+14)*8))
  {
    return e_success;
  }
  else
  {
    return e_failure;
  }
}
uint get_file_size(FILE *fptr)
{
  fseek(fptr,0,SEEK_END);
  return ftell(fptr);
}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
  rewind(fptr_src_image);
  char buff[54];
  fread(buff,54,1,fptr_src_image);
  fwrite(ch,54,1,fptr_dest_image);
}
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
  char buff[8];
  for(int i=0;i<strlen(magic_string);i++)
  {
    if(fread(buff,8,1,encInfo->fptr_src_image)!=8)
    {
      return e_failure;
    }
    encode_byte_to_lsb(magic_string[i],buff);
    if(fwrite(buff,8,1,encInfo->fptr_stego_image)!=8)
    {
      return e_failure;
    }
  
    return e_success;

  }
}
Status encode_byte_to_lsb(char data,char *image_buffer)
{
for(int i=7;i>=0;i--)
{
  if(data&(1<<i))
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
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
  char buff[8];
  int length=strlen(file_extn);
  for(int i=0;i<length;i++)
  {
    if(fread(buff,8,1,encInfo->fptr_src_image)!=8)
    {
      return e_failure;
    }
    encode_byte_to_lsb(file_extn[i],buff);
    if(fwrite(buff,8,1,encInfo->fptr_stego_image)!=8)
    {
      return e_failure;
    }
  }
    return e_success;
}
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
 char buff[32];
 if(fread(buff,32,1,encInfo->fptr_src_image)!=8)
 {
    return e_failure;
 }
  encode_size_to_lsb(file_size,buff);

  if(fwrite(buff,32,1,encInfo->fptr_stego_image)!=8)
  {
    return e_failure;
  }
}

Status encode_size_to_lsb(uint size, char *image_buffer)
{
for(int i=0;i<32;i++)
{
  if(size&(1<<i))
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
  for(i=0;i<encInfo->size_secret_file;i++)
  {
    
  }


  
}


