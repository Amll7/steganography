#include <stdio.h>
#include "encode.h"
#include "types.h"
#include "decode.h"

int main(int argc,char **argv)
{
    EncodeInfo encInfo;
    DecodeInfo decInfo;

    char opt=argv[1][1];

    if(check_operation_type(opt)==e_encode)
    {
    printf("INFO: Selected operation: Encoding\n");
    if(read_and_validate_encode_args(argv,&encInfo)==e_failure)
    {
        return 0;
    }
    else
    {
    if(do_encoding(argv,&encInfo) == e_failure)
    {
        printf("ERROR: Encoding failed.\n");
        return 0;
    }
    }
    }
    else if(check_operation_type(opt)==e_decode)
    {
     if(check_operation_type(opt)==e_decode)
     {
     printf("INFO: Selected operation: Decoding\n");
     if(read_and_validate_dec_args(argv,&decInfo)==e_failure)
     {
       return 0;
     }
     else
     {
       
        if(do_decoding(&decInfo) == e_failure)
        {
          printf("ERROR: Decoding failed.\n");
          return 0;
        }

     }
     }
    }
     else if(check_operation_type(opt)==e_unsupported)
    {
        return 0;
    }
    return 0;
}
OperationType check_operation_type(char opt)
{
    if(opt=='e')
    {
        return e_encode;
    }
    else if(opt=='d')
    {
      return e_decode;
    }
    else
    {
        return e_unsupported;
    }
}
