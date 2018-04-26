// Copies a BMP file

#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"

int main(int argc, char *argv[])
{
    // ensure proper usage
    if (argc != 4)
    {
        fprintf(stderr, "Usage: resize n infile outfile\n");
        return 1;
    }

    else if (atoi(argv[1]) > 100 || argv[1] <= 0)
    {
        fprintf(stderr, "n should be a postitive integer less than or equal to 100");
        return 1;
    }

    // n is the resize factor
    int n = atoi(argv[1]);

    // remember filenames
    char *infile = argv[2];
    char *outfile = argv[3];

    // open input file
    FILE *inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        fprintf(stderr, "Could not open %s.\n", infile);
        return 2;
    }

    // open output file
    FILE *outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 3;
    }

    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 ||
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }

    //Update height & width
    bi.biWidth *= n;
    bi.biHeight *= n;

    // determine padding for scanlines
    int padding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;

    // Update & write outfile's BITMAPFILEHEADER
    bf.bfSize = ((sizeof(RGBTRIPLE) * bi.biWidth) + padding) * abs(bi.biHeight) +
                sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);

    // Update & write outfile's BITMAPINFOHEADER
    bi.biSizeImage = ((sizeof(RGBTRIPLE) * bi.biWidth) + padding) * abs(bi.biHeight);
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);

    RGBTRIPLE array[bi.biWidth * n];

    // iterate over infile's scanlines
    for (int i = 0, biHeight = abs(bi.biHeight); i < biHeight; i++)
    {
        int counter = 0;

        // iterate over pixels in scanline
        for (int j = 0; j < bi.biWidth; j++)
        {
            // temporary storage
            RGBTRIPLE triple;

            // read RGB triple from infile
            fread(&triple, sizeof(RGBTRIPLE), 1, inptr);

            // write triple into array n times for each pixel
            for (int k = 0; k < n; k++)
            {
                array[counter] = triple;
                
                //error testing print statement
                printf("%i, %i, %i, %i\n", triple.rgbtBlue, triple.rgbtGreen, triple.rgbtRed, counter);
                
                counter++;

            }
        }

        // resize vertically
        int h = abs(bi.biHeight);

        // write outfile lines n times (if initial height > 1)
        if (h > 1)
        {
            for (int k = 0; k < n; k++)
            {
                //write the array to outptr
                fwrite(&array, sizeof(array), 1, outptr);
                for (int a = 0; a < padding; a++)
                {
                    fputc(0x00, outptr);
                }
            }
        }

        else
        {
            // write array to line of the resized image
            fwrite(&array, sizeof(RGBTRIPLE), 1, outptr);
        }

        // fseek(inptr, padding, SEEK_CUR);

        // // then add it back (to demonstrate how)
        // for (int k = 0; k < padding; k++)
        // {
        //     fputc(0x00, outptr);
        // }
    }



        // once pixels have been read and copied into array, time to write them to the out file

        // // resize vertically
        // int h = abs(bi.biHeight);
        // // write outfile lines n times (if initial height > 1)
        // if (h > 1)
        // {
        //     for (int k = 0; k < n; k++)
        //     {
        //         //write the array to outptr
        //         fwrite(&array, sizeof(RGBTRIPLE), 1, outptr);
        //         for (int a = 0; a < padding; a++)
        //         {
        //             fputc(0x00, outptr);
        //         }
        //     }
        // }
        // else
        // {
        //     // write array to line of the resized image
        //     fwrite(&array, sizeof(RGBTRIPLE), 1, outptr);
        // }

        // skip over padding, if any
        // fseek(inptr, padding, SEEK_CUR);

        // // then add it back (to demonstrate how)
        // for (int k = 0; k < padding; k++)
        // {
        //     fputc(0x00, outptr);
        // }


    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // success
    return 0;
}
