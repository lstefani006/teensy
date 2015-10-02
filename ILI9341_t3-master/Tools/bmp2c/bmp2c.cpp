/***********************************************************************
 * Project: BMP to C converter
 *
 * Description:
 *  Converts BMP files to C data files with raw image data
 *
 * Based on work done by Paul Kovitz, Guru Suryan, and Rob Mays
 *
 ***********************************************************************
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * products. This software is supplied "AS IS" without any warranties.
 * NXP Semiconductors assumes no responsibility or liability for the
 * use of the software, conveys no license or title under any patent,
 * copyright, or mask work right to the product. NXP Semiconductors
 * reserves the right to make changes in the software without
 * notification. NXP Semiconductors also make no representation or
 * warranty that such application will be suitable for the specified
 * use without further testing or modification.
 **********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "windows.h"

typedef union tagPixelData
{
    struct
    {
        WORD red:4;
        WORD green:4;
        WORD blue:4;
        WORD unused:4;
    } color754xx;
    struct
    {
        WORD red:5;
        WORD green:5;
        WORD blue:5;
        WORD unused:1;
    } color555;
    struct
    {
        WORD red:5;
        WORD green:6;
        WORD blue:5;
    } color565;
    struct
    {
        DWORD red:8;
        DWORD green:8;
        DWORD blue:8;
        DWORD alpha:8;
    } color888;
    DWORD pixel32;
	WORD pixel16[2];
} PixelData;

#define TO555(pd, r, g, b) \
	pd.color555.red = (r >> 3); \
	pd.color555.green = (g >> 3); \
	pd.color555.blue = (b >> 3); \
	pd.color555.unused = 0;

#define TO565(pd, r, g, b) \
	pd.color565.red = (r >> 3); \
	pd.color565.green = (g >> 2); \
	pd.color565.blue = (b >> 3);

#define TO888(pd, r, g, b) \
	pd.color888.red = r; \
	pd.color888.green = g; \
	pd.color888.blue = b; \
	pd.color888.alpha = 0;

#define TO_lPC754XX(pd, r, g, b) \
	pd.color754xx.red = (r >> 4); \
	pd.color754xx.green = (g >> 4); \
	pd.color754xx.blue = (b >> 4); \
	pd.color754xx.unused = 0;

static void add_underscores(char *name)
{
    while(*name)
    {
        if(isspace((int)*name))
            *name = '_';
        name++;
    }
}

static void print_usage(char *filename)
{
    printf("%s: usage:\n\t%s infile -444/-555/-565/-888 [-sr]\n"
                   "where\n\tinfile is a Microsoft(tm) BMP file\n"
                   "\t-444 will generate RGB444 12-bit colors\n"
                   "\t-555 will generate RGB555 16-bit colors\n"
                   "\t-565 will generate RGB565 16-bit colors\n"
                   "\t-888 will generate RGB888 24-bit colors\n"
                   "\t-s will swap the red and green color fields\n"
                   "\t-r will generate a raw binary files instead of a C file\n",
            filename, filename);
}

/**********************************************************************
 *
 * Function: main()
 *
 * Purpose:
 *  See file header, above
 *
 * Processing:
 *  Extract bitmap file name from argv[1]
 *	Open the bitmap file
 *	Read the bitmap header information and pixel data
 *	Close bitmap file
 *  Extract the output file name from the bitmap file name and change 
 *  ext to .c
 *	Open C file for writing
 *	Write bitmap information to C file
 *
 * Parameters:
 *	argc--must be 3 or 4
 *	argv[0]--function name (supplied by OS)
 *	argv[1]--bitmap input file name
 *	argv[2]--color depth output field
 *  argv[4]--optional -sr flags
 * Outputs:
 *	C-formatted bitmap data to specified file name
 *
 * Returns:
 *
 *	0 normally
 *	-1 if there is a problem
 *
 * Notes:
 *
 **********************************************************************/
int main(int argc, char * argv[])
{
    BITMAPFILEHEADER bf;
    BITMAPINFO bmi;

    FILE *infile, *outfile;
    DWORD bytes_read = 0, bmpsize, fillindex, nextlinei, linei, i, offset;
	PixelData *databuffer;
    int lh754xx = 0;
	int use555 = 0;
	int use565 = 0;
	int use888 = 0;
	int swaprgb = 0;
	int binout = 0;

    char drive[_MAX_DRIVE];
    char dir[_MAX_DIR];
    char file[_MAX_FNAME];
	char ufile[_MAX_FNAME];
    char ext[_MAX_EXT];
    char of_name[_MAX_FNAME+_MAX_EXT];

	printf("NXP LPC BMP to source/data conversion utility\n");

    if ((argc < 3) || (argc > 4))
        
    {
        print_usage(argv[0]);
        exit(-1);
    }

    if (strcmp(argv[2], "-444") == 0)
    {
        lh754xx = 1;
    }
    else if (strcmp(argv[2], "-555") == 0)
    {
        use555 = 1;
    }
    else if (strcmp(argv[2], "-565") == 0)
    {
        use565 = 1;
    }
    else if (strcmp(argv[2], "-888") == 0)
    {
        use888 = 1;
    }
    else
    {
        print_usage(argv[0]);
        exit(-1);
    }

    if (argc == 4)
    {
        if (strcmp(argv[3], "-s") == 0)
			swaprgb = 1;
        else if (strcmp(argv[3], "-r") == 0)
			binout = 1;
		else if ((strcmp(argv[3], "-sr") == 0) || (strcmp(argv[3], "-rs") == 0))
		{
			swaprgb = 1;
			binout = 1;
		}
		else
		{
			print_usage(argv[0]);
			exit(-1);
        }
    }

    /* split the string to separate elementss */
    _splitpath(argv[1], drive, dir, file, ext);

	strcpy(ufile,file);
	_strupr(ufile);

    strcpy(file,argv[1]);
    if (strlen(ext) == 0)
    {
        strcat(file,".bmp");
    }

    infile = fopen(file, "rb");
    if (!infile)
    {
		printf("Error opening input file %s\n", file);
		exit(-1);
	}

	if (fread(&bf,sizeof(bf),1,infile) == 1)
    {
        bytes_read += sizeof(bf);
    }
	else
	{
		printf("Error reading BMP info\n");
		fclose(infile);
		exit(-1);
	}

    if (fread(&bmi,sizeof(bmi.bmiHeader),1,infile) == 1)
    {
        bytes_read += sizeof(bmi.bmiHeader);
    }
	else
	{
		printf("Error reading BMP header\n");
		fclose(infile);
		exit(-1);
	}

	if (bmi.bmiHeader.biBitCount != 24)
	{
		printf("Only 24bpp BMP files are supported\n");
		fclose(infile);
		exit(-1);
	}
		
	/* Allocate buffer storage */
	bmpsize = bmi.bmiHeader.biWidth * bmi.bmiHeader.biHeight;
	databuffer = (PixelData *) malloc(sizeof(PixelData) * bmpsize);
	if (databuffer == NULL)
	{
		printf("Error allocating temporary data buffer, is image too big?\n");
		fclose(infile);
		exit(-1);
	}

    /* seek to the beginning of the data */
	fseek(infile, bf.bfOffBits, SEEK_SET);

	/* Read data into array */
	fillindex = bmpsize - 1;
	i = 0;
	while (i < bmpsize)
    {
		BYTE rgb[3];

		if (fread(&rgb, sizeof(rgb), 1, infile) != 1)
		{
			printf("error reading BMP data\n");
 		    fclose(infile);
			free(databuffer);
	        exit(-1);
   	    }

		if (swaprgb)
		{
			BYTE tmp = rgb[2];
			rgb[2] = rgb[0];
			rgb[0] = tmp;
		}

		/* Store in color array for the correct type. BMPs are
		   stored in reverse, so save the data backwards */
		if (use555)
		{
			TO555(databuffer[fillindex], rgb[2], rgb[1], rgb[0]);
		}
		else if (use565)
		{
			TO565(databuffer[fillindex], rgb[2], rgb[1], rgb[0]);
		}
		if (use888)
		{
			TO888(databuffer[fillindex], rgb[2], rgb[1], rgb[0]);
		}
		if (lh754xx)
		{
			TO_lPC754XX(databuffer[fillindex], rgb[2], rgb[1], rgb[0]);
		}

		fillindex--;
		i++;
	}
	
	fclose(infile);

	if (use555)
		sprintf(of_name,"image555");
	else if (use565)
		sprintf(of_name,"image565");
	else if (use888)
		sprintf(of_name,"image888");
	else if (lh754xx)
		sprintf(of_name,"image444");

	if (binout)
	{
		strcat(of_name,".raw");
		printf("Outputting binary file: %s\n", of_name);
	    outfile = fopen(of_name, "wb");
	}
	else
	{
		strcat(of_name,".c");
		printf("Outputting c file: %s\n", of_name);
	    outfile = fopen(of_name,"w");
	}
	if (outfile == NULL)
	{
		printf("error opening out file %s\n", of_name);
		free(databuffer);
	    exit(-1);
	}

	if (!binout)
	{
	    fprintf (outfile,
			"\n\n/*\n * BMP image data converted from 24bpp\n");
		if (use555)
			fprintf (outfile, " * to RGB555\n");
		else if (use565)
			fprintf (outfile, " * to RGB565\n");
		else if (use888)
			fprintf (outfile, " * to RGB888\n");
		else if (lh754xx)
			fprintf (outfile, " * to RGB444\n");
		if (swaprgb)
			fprintf (outfile, " * Red and green data swapped\n");
	    fprintf (outfile, " */\n\n");

		if ((use555) || (use565) || (use888))
		{
	        fprintf (outfile, "#define COLOR_BPP          16\n");
	        fprintf (outfile, "#define COLOR_STORAGE_SIZE 2\n");
		}
		else
		{
	        fprintf (outfile, "#define COLOR_BPP          24\n");
	        fprintf (outfile, "#define COLOR_STORAGE_SIZE 4\n");
		}

	    fprintf (outfile, "#define BMPWIDTH           %d;\n"
			"#define BMPHEIGHT          %d;\n\n",
			bmi.bmiHeader.biWidth, bmi.bmiHeader.biHeight);

		/* Image data */
		if ((use555) || (use565) || (lh754xx))
			fprintf (outfile, "const unsigned short image16[] = {\n    ");
		else
			fprintf (outfile, "const unsigned long image32[] = {\n    ");
	}

	fillindex = 0;
	i = 0;
	linei = bmi.bmiHeader.biWidth - 1;
	nextlinei = 0;
	while (i < bmpsize)
	{
		offset = nextlinei + linei;
//		offset = i;

		if (!binout)
		{
			if (fillindex >= 72)
			{
				fprintf (outfile, ",\n    ");
				fillindex = 0;
			}
			else if (i != 0)
				fprintf (outfile, ", ");

			if ((use555) || (use565) || (lh754xx))
			{
				fprintf (outfile, "0x%04x", databuffer[offset].pixel16[0]);
				fillindex += 6;
			}
			else
			{
				fprintf (outfile, "0x%08x", databuffer[offset].pixel32);
				fillindex += 10;
			}

			fillindex += 3;
		}
		else
		{
			if ((use555) || (use565) || (lh754xx))
			{
				fwrite(&databuffer[offset].pixel16[0],
					sizeof(databuffer[offset].pixel16[0]),
					1, outfile);
			}
			else
			{
				fwrite(&databuffer[offset].pixel32,
					sizeof(databuffer[offset].pixel32),
					1, outfile);
			}
		}

		i++;

		if (linei == 0)
		{
			linei = bmi.bmiHeader.biWidth;
			nextlinei += bmi.bmiHeader.biWidth;
		}
		linei--;
	}

	if (!binout)
		fprintf (outfile, "\n};");
	free(databuffer);
	fclose(outfile);

    return 0;
}