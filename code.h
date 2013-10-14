#include <stdio.h>

typedef struct
{
    unsigned int    bfType;
    unsigned long   bfSize;
    unsigned int    bfReserved1;
    unsigned int    bfReserved2;
    unsigned long   bfOffBits;
} BITMAPFILEHEADERS;
 
typedef struct
{
    unsigned int    biSize;
    int             biWidth;
    int             biHeight;
    unsigned short  biPlanes;
    unsigned short  biBitCount;
    unsigned int    biCompression;
    unsigned int    biSizeImage;
    int             biXPelsPerMeter;
    int             biYPelsPerMeter;
    unsigned int    biClrUsed;
    unsigned int    biClrImportant;
} BITMAPINFOHEADERS;
 
typedef struct
{
    int   rgbBlue;
    int   rgbGreen;
    int   rgbRed;
    int   rgbReserved;
} RGBQUADS;
 
 
static unsigned short read_u16(FILE *fp);
static unsigned int   read_u32(FILE *fp);
static int            read_s32(FILE *fp);
int Encode(char reco[10][20],char fName[]);
int Decode(char reco[10][20], char fName[]);
int BMP_Read(char fName[],BITMAPFILEHEADERS &header,BITMAPINFOHEADERS &bmiHeader,RGBQUADS rgb[1200][1000]);
int BMP_Write(char fName[],BITMAPFILEHEADERS header,BITMAPINFOHEADERS bmiHeader,RGBQUADS rgb[1200][1000]);