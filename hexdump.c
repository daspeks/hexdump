#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void summary(const int p1, const int p2)
{
    float percentage = p1 / (p1 + p2);

    if (percentage >= 0.75)
    {
        printf("This is definitely a text file");
    }
    else if ((percentage < 0.75) && (percentage >= 0.25))
    {
        printf("This was possibly a text file");
    }
    else
    {
        printf("This is definitely a binary file");
    }
    printf("\n\n");
}


void magicnum(const void * data, long size)
{
    const unsigned char * byte = data;
    int counter = 0;

    // MAGIC NUMBER
    printf("Ox");
    while (size > 0 && counter < 4)
    {
        printf("%.2x", *byte);
        size--;
        byte++;
        counter++;
    }
    printf("\n");
}


long getFileSize(FILE * f)
{
    long size;
    fseek (f , 0 , SEEK_END); // set file pointer to the end of the file
    size = ftell (f); // no of bytes in the file
    rewind (f); // set file pointer to the start of the file
    return size;
}


void * file_in_bytes(FILE * fp, const long file_size)
{
    // long file_size;
    void * buffer;
    size_t result;

    // allocate memory to contain the whole file:
    buffer = malloc (file_size);
    if (buffer == NULL)
    {
        fputs ("Memory error", stderr);
        exit (2);
    }

    // copy the file into the buffer:
    result = fread (buffer, 1, file_size, fp);
    if (result != file_size)
    {
        fputs ("Reading error", stderr);
        exit (3);
    }

    return buffer;
}


void file_stats(const long size, int * rem, int * hex)
{
    * rem = size % 16;
    if (* rem == 0)
    {
        * hex = size / 16;
    }
    else
    {
        * hex = (size / 16) + 1;
    }
}


int main()
{
    FILE *fp;
    char ch, file_name[25];
    unsigned char array[17];

    int hex_lines = 0;
    int rem_bytes = 0;
    int line_counter = 0;
    
    long file_size;
    void * buffer;

    // get file name from user
    printf("Enter the name of file you wish to see\n");
    fgets(file_name, sizeof(file_name), stdin);
    file_name[strlen(file_name)-1] = 0x00;

    // read file in binary mode
    fp = fopen(file_name,"rb");
    if( fp == NULL )
    {
        perror("Error while opening the file\n");
        exit(EXIT_FAILURE);
    }

    // get file size
    file_size = getFileSize(fp);

    // get file in byte array
    buffer = file_in_bytes(fp, file_size);

    // print magic number
    magicnum(buffer, file_size);

    // get no of hex lines and remaining bytes from file
    file_stats(file_size, &rem_bytes, &hex_lines);

    // print heading
    printf("Offset\tHexadecimal Data Format \t \t \t \tCharacter Format\n");

    int i = 0;
    int k = 0;
    int offset = 0;
    int printable = 0;
    int nonprintable = 0;

    rewind (fp);
    while( ( ch = fgetc(fp) ) != EOF )
    {
        // check for printable / non printable ascii
        if ((ch < 0x20) || (ch > 0x7e)) 
        {
            array[k] = '.';
            k++;
            nonprintable = nonprintable + 1;
        }
        else
        {
            array[k] = ch;
            k++;
            printable = printable + 1;
        }

        if ((k == 16) && (line_counter <= hex_lines))
        {
            array[k] = '\0';
            k = 0;
            line_counter++;
        }

        // print first offset
        if (i == 0)
        {
            printf("%05x\t", offset);
        }

        // print hex of ch
        printf("%02X ", (unsigned char) ch);

        // print char array
        if (!(k%16))
        {
            printf("\t%s", array);
            k = 0;
        }

        // break line and write offset of next line
        if (!(++i % 16))
        {
            putc('\n', stdout);
            printf("%05x\t", offset);
        }

        // update offset
        offset = 0 + i + 1;
    }

    // fill the remaining with 00
    int sindex = rem_bytes;
    
    // fill up the char array
    for (int t = 0; t < 16 - rem_bytes; t++)
    {
        printf("00 ");
        array[sindex] = '.';
        sindex++;
    }

    array[sindex] = '\0';
    printf("\t%s", array);
    printf("\n");
    fclose(fp);

    // print magic number
    printf("Magic number: ");
    magicnum(buffer, file_size);

    // print summary
    summary(printable, nonprintable);
    
    return 0;
}