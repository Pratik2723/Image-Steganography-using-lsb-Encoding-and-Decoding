/*
Name : Harshvardhan Patil
Roll Num. - 24021_160
Batch - 24021
Date - 26/11/2024
 */
#include <stdio.h>
#include "encode.h"
#include "decode.h"
#include "types.h"
#include <stdlib.h>
#include <string.h>
int main(int argc, char *argv[])              //command line arguments in main function
{
    //declared structure variable
    EncodeInfo encInfo;
    DecodeInfo dncInfo;
    //if argc is less than 3 then print insufficient argument
    if (argc < 3)
    {
        printf("Error : Insufficient aguments count\n");
        return 1;
    }
    //function call of check operation type and store the value in ret variable
    int ret = check_operation_type(argv);
    if (ret == e_encode)
    {
        //check the read_and_alidate the arguments 
        if (read_and_validate_encode_args(argv, &encInfo) == e_success)
        {
            printf("Read and validation of argument is successful\n");
            if (do_encoding(&encInfo) == e_success)    //if success then do encodimng
            {
                printf("encoding successful\n");
            }
            else
            {
                printf("encoding unsuccessful\n");     //else encoding unsecssful
            }
        }
        else
        {
             printf("Read and validation of argument is unsuccessful\n");            //read and validation is unsecssfull
        }
    }
    else if (ret == e_decode)                     //if decode then read and validate the argument
    {
        if(read_and_validate_dncode_args(argv, &dncInfo)==e_success){   //if sucesses then do decoding
            if(do_decoding(&dncInfo)==e_success){
                printf("Decoding sucessfully\n");               //if success then decoding successful
            }
            else{
              printf("Decoding unsucessfully\n");              //else decoding unsecssfull
            }
        }
        else{
            printf("Read and validation of argument is unsuccessful\n");        //print the read and validation unseccssfully
        }
        
    }
    else
    {
        printf("Error for invalid operation.\n");                   //error for invalid operation
    }
    return 0;
}

OperationType check_operation_type(char *argv[])           //check operation type
{
    if (strcmp(argv[1], "-e") == 0)                  //check the argument is -e or -d then do decoding or encoding

    {
        printf("operation type is encoding\n");
        return e_encode;
    }
    else if (strcmp(argv[1], "-d") == 0)
    {
        printf("operation type is decoding\n");  
        return e_decode;
    }
    else
    {
        printf("operation type is unsupported\n");        //operation is unsupported 
        return e_unsupported;
    }
}
