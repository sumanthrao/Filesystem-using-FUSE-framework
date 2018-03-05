// C program for writing 
// struct to file
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
// a struct to read and write
struct person 
{
    int id;
    char fname[20];
    char lname[20];
};
 
int main ()
{
    FILE *outfile;
     
    // open file for writing
    outfile = fopen ("person.dat", "rw+");
    if (outfile == NULL)
    {
        fprintf(stderr, "\nError opend file\n");
        exit (1);
    }
 
    struct person input1 = {1, "new", "shavxrma"};
    struct person input2 = {2, "mahendra", "dhoni"};
     
    // write struct to file
    //rewind(outfile);
    fseek(outfile,0*sizeof(struct person),SEEK_SET);
    printf("%ld",ftell(outfile));
    fwrite (&input1, sizeof(struct person), 1, outfile);
   // fwrite (&input2, sizeof(struct person), 1, outfile);
     
    if(fwrite != 0) 
        printf("contents to file written successfully !\n");
    else
        printf("error writing file !\n");
    return 0;
}
