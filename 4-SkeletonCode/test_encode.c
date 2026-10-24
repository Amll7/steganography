#include <stdio.h>
#include "encode.h"
#include "types.h"

int main(int argc,char **argv)
{
    EncodeInfo encInfo;
    char opt=argv[1][1];
    check_operation_type(opt);
    read_and_validate_encode_args(argv,&encInfo);
    return 0;
}
OperationType check_operation_type(char opt)
{
    if(opt=='e')
    {
        printf("encode");
        return e_encode;
    }
    else if(opt=='d')
    {
      printf("decode");
      return e_decode;
    }
    else
    {
        printf("unsupported");
        return e_unsupported;
    }
}
