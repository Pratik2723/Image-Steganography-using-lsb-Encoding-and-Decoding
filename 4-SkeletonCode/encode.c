#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "common.h"

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
    printf("width of image = %u pixel\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height of image = %u pixel\n", height);

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

Status open_files(EncodeInfo *encInfo)//opening the file by stucture 
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");//open teh file in teh read mode
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)//condition checking for the source file 
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

        return e_failure;
    }
    printf("File are opened sucessfully : %s\n", encInfo->src_image_fname);
    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

        return e_failure;
    }
    printf("File are opened sucessfully : %s\n", encInfo->secret_fname);
    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

        return e_failure;
    }
    printf("File are opened sucessfully : %s\n", encInfo->stego_image_fname);
    // No failure return e_success
    return e_success;
}

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    //check the aruments preseint in then index return e_success or e_failure
    if (strstr(argv[2], ".bmp") != NULL && strcmp(strstr(argv[2], ".bmp"), ".bmp") == 0)
    {
        encInfo->src_image_fname = argv[2];         //assign the file name in src image name
        printf("source file set to : %s\n", encInfo->src_image_fname);     //print gthe file name
    }
    else
    {
        return e_failure;                      //else return efailure
    }
    if (strstr(argv[3], ".txt") != NULL && strcmp(strstr(argv[3], ".txt"), ".txt") == 0) //check the third index .txt is present or not
    {
        encInfo->secret_fname = argv[3];                       //then assign the .txt file in secret fname
        printf("secret file set to : %s\n", encInfo->secret_fname);        //print the the file name
    }
    else
    {
        return e_failure;                           //else return e_failure
    }
    if (argv[4] == NULL)       //check the 4 the index is null or not
    {
        encInfo->stego_image_fname = "default.bmp";          //then assign the the image name in stego image fname
        printf(" stego file is not provided it created : %s \n", encInfo->stego_image_fname);   //print the file name
    }
    else
    {
        if (strstr(argv[4], ".bmp") != NULL && strcmp(strstr(argv[4], ".bmp"), ".bmp") == 0)    //if file extension is .bmp then assign image name in stego fname
        {
            encInfo->stego_image_fname = argv[4];
            printf("stego file is  provided it created : %s \n", encInfo->stego_image_fname);  //print the filen name
        }
        else
        {
            encInfo->stego_image_fname = "default.bmp";                    //default.bmp file assign in stego_image _fname
            printf("stego file is not provided it created : %s \n", encInfo->stego_image_fname);   //print the file name
        }
    }

    return e_success;                         //return e_success
}

Status do_encoding(EncodeInfo *encInfo)               //then do encoding
{
    int ret = open_files(encInfo);
    if (ret == e_failure)
    {
        printf("Files are cannot opened sucessfully\n");
        return 0;
    }
    // check the ret value is success or failure
    // if it is e_failure stop the operation

    // if it is success continue with next function
    else
    {
        printf("All files are sucessfully opened\n");
        if (check_capacity(encInfo) == e_success)
        {
            printf("Image capcity is greater than expected image size\n");
            if (copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
            {
                printf("Header bytes are sucessfully copied\n");
            }
            else
            {
                printf("Header bytes are unable to copied\n");
                return e_failure;
            }
            if (encode_magic_string(MAGIC_STRING, encInfo) == e_success)
            {
                printf("Magic string are sucessfully encoded\n");
            }
            else
            {
                printf("Magic string string are unable to encoding\n");
                return e_failure;
            }
            if (encode_secret_file_extn(strstr(encInfo->secret_fname, "."), encInfo) == e_success)
            {
                printf("File extension are sucessfully encoded\n");
            }
            else
            {
                printf("File extension are unsucessfully encoded\n");
                return e_failure;
            }
            if (encode_secret_file_data(encInfo) == e_success)
            {
                printf("File data are sucessfully encoded\n");
            }
            else
            {
                printf("File data are unsucessfully encoded\n");
                return e_failure;
            }
            if (copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
            {
                printf("Remaining data are sucessfully encoded\n");
            }
            else
            {
                printf("Remaining data are unsucessfully encoded\n");
                return e_failure;
            }

            return e_success;
        }
        else
        {
            printf("Image capcity is less than expected image size\n");
            return e_failure;
        }
    }
}
// check the capacity of image file to encode the data

Status check_capacity(EncodeInfo *encInfo)
{
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);
    // get the secret file size and store into size_secret_file variable in the structure
    char *ptr = strstr("secret.txt", ".");
    printf("Image capcity is : %d\n", encInfo->image_capacity);
    int ext_size = strlen(ptr);
    printf("Extension size : %d\n", ext_size);
    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);
    printf("Secret File size : %ld\n", encInfo->size_secret_file);
    uint required_size = (54 + (strlen(MAGIC_STRING) + sizeof(int) + ext_size + sizeof(int) + encInfo->size_secret_file) * 8);
    printf("Required size : %u\n", required_size);
    if (encInfo->image_capacity > required_size)
    {

        return e_success;
    }
    return e_failure;
}

uint get_file_size(FILE *fptr)
{
    uint size;
    fseek(fptr, 0, SEEK_END);
    size = ftell(fptr);
 //   fclose(fptr);
    return size;
}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    // move fptr_src_image file pointer to 0th position
    fseek(fptr_src_image, 0, SEEK_SET);
    char arr[54];
    if (fread(arr, 54, 1, fptr_src_image) != 1)
    { // read 54 bytes from tthe src_file
        printf("Error in reading the header bytes\n");
        return e_failure;
    }
    if (fwrite(arr, 54, 1, fptr_dest_image) != 1)
    { // write 54 bytes to the dest file(fptr_dest_file)
        printf("Error in writing the header bytes\n");
        return e_failure;
    }
    return e_success;
}

Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    if (encode_data_to_image((char *)magic_string, (int)strlen(magic_string), encInfo->fptr_src_image, encInfo->fptr_stego_image) != e_success)
    {
        printf("Error in encoding the magic striing\n");
        return e_failure;
    }
    return e_success;
}

Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
    // Buffer to hold 8 bytes of image data
    char image_data[8];

    // Loop through each byte of the data to encode
    for (int i = 0; i < size; i++)
    {
        // Read 8 bytes from the source image
        if (fread(image_data, 8, 1, fptr_src_image) != 1)
        {
            // If reading fails, print an error and return failure
            printf("Error reading image data from source image.\n");
            return e_failure;
        }

        // Encode the byte of data into the image data using the least significant bit
        encode_byte_to_lsb(data[i], image_data);

        // Write the modified 8 bytes to the destination stego image
        if (fwrite(image_data, 8, 1, fptr_stego_image) != 1)
        {
            // If writing fails, print an error and return failure
            printf("Error writing encoded data to stego image.\n");
            return e_failure;
        }
    }
    return e_success;
}

Status encode_byte_to_lsb(char data, char *image_buffer)
{
    for (int i = 0; i < 8; i++)
    {
        image_buffer[i] = (image_buffer[i] & (~1)) | ((data >> (7 - i)) & 1);
    }
    return e_success;
}

Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    // encode the .txt file size in
    char image_buffer[32];
    if (fread(image_buffer, 32, 1, encInfo->fptr_src_image) != 1)
    {
        printf("error in reading from source image\n");
        return e_failure;
    }
    encode_size_to_lsb(strlen(file_extn), image_buffer);
    if (fwrite(image_buffer, 32, 1, encInfo->fptr_stego_image) != 1)
    {
        printf("error in writing data in stego image\n");
        return e_failure;
    }

    char image_data[8];
    // run the loop for upto size
    for (int i = 0; i < strlen(file_extn); i++)
    {
        if (fread(image_data, 8, 1, encInfo->fptr_src_image) != 1)
        {
            printf("error in reading from source image\n");
            return e_failure;
        }
        encode_byte_to_lsb(file_extn[i], image_data);
        if (fwrite(image_data, 8, 1, encInfo->fptr_stego_image) != 1)
        {
            printf("error in writing data in stego image\n");
            return e_failure;
        }
    }
    return e_success;
}
Status encode_secret_file_data(EncodeInfo *encInfo)
{

  if (encode_secret_file_size(encInfo->size_secret_file, encInfo) == e_failure)
    {
        // If encoding the secret file size fails, return failure
        printf("Error encoding secret file size.\n");
        return e_failure;
    }

    // Buffer to hold the secret file data
   
    char file_data[100];

    // Read the secret file data into the buffer
    fseek(encInfo->fptr_secret,0,SEEK_SET);
    if ( fread(file_data, encInfo->size_secret_file, 1, encInfo->fptr_secret) != 1)
    {
        // If reading the secret file fails, print an error and return failure
        printf("Error reading secret file data.\n");
        return e_failure;
    }
   
         
    // Encode the secret file data into the stego image
    if (encode_data_to_image(file_data, encInfo->size_secret_file, encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure)
    {
        // If encoding the file data fails, return failure
        printf("Error encoding secret file data into stego image.\n");
        return e_failure;
    }

    // Return success as the secret file data encoding was completed successfully
    return e_success;
}

Status encode_secret_file_size(long file_size, EncodeInfo* encInfo)
{

    // Buffer to hold 32 bytes of data
    char image_buffer[32];

    // Read 32 bytes of data from the source image
    if (fread(image_buffer, 32, 1, encInfo->fptr_src_image) != 1)
    {
        // If reading fails, print an error and return failure
        printf("Error reading image data from source image while encoding file size.\n");
        return e_failure;
    }

    // Encode the file size into the image buffer
    if (encode_size_to_lsb(file_size, image_buffer) == e_failure)
    {
        // If encoding the file size fails, return failure
        printf("Error encoding the file size.\n");
        return e_failure;
    }

    // Write the 32 bytes of image data to the destination image
    if (fwrite(image_buffer, 32, 1, encInfo->fptr_stego_image) != 1)
    {
        // If writing fails, print an error and return failure
        printf("Error writing encoded file size to stego image.\n");
        return e_failure;
    }
  
    // Return success as the file size encoding was completed successfully
    return e_success;
}
Status encode_size_to_lsb(int data, char *image_buffer)
{
    // write the logic tto encode size of int image_buffer
    for (int i = 0; i < 32; i++)
    {
        image_buffer[i] = (image_buffer[i] & (~1)) | ((data >> (31 - i)) & 1);
    }
    
}
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    char ch;
    // while(repeat untill scr file reaches EOF)
    // read one bytes from the src file
    //  write one byte the dest file
    while (fread(&ch, 1, 1, fptr_src) != 0)
    {

        if (fwrite(&ch, 1, 1, fptr_dest) != 1)
        {
            return e_failure;
        }
    }
    return e_success;
}