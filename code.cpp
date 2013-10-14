#include "code.h"

int BMP_Read(char fName[],BITMAPFILEHEADERS &header,BITMAPINFOHEADERS &bmiHeader,RGBQUADS rgb[1200][1000])
{
	    FILE * pFile = fopen(fName, "rb");
 
    // считываем заголовок файла
    header.bfType      = read_u16(pFile);
    header.bfSize      = read_u32(pFile);
    header.bfReserved1 = read_u16(pFile);
    header.bfReserved2 = read_u16(pFile);
    header.bfOffBits   = read_u32(pFile);
 
    // считываем заголовок изображения 
    bmiHeader.biSize          = read_u32(pFile);
    bmiHeader.biWidth         = read_s32(pFile);
    bmiHeader.biHeight        = read_s32(pFile);
    bmiHeader.biPlanes        = read_u16(pFile);
    bmiHeader.biBitCount      = read_u16(pFile);
    bmiHeader.biCompression   = read_u32(pFile);
    bmiHeader.biSizeImage     = read_u32(pFile);
    bmiHeader.biXPelsPerMeter = read_s32(pFile);
    bmiHeader.biYPelsPerMeter = read_s32(pFile);
    bmiHeader.biClrUsed       = read_u32(pFile);
    bmiHeader.biClrImportant  = read_u32(pFile);

 
    for (int i = 0; i < bmiHeader.biWidth; i++) 
	{
    for (int j = 0; j < bmiHeader.biHeight; j++) 
		{
            rgb[i][j].rgbBlue = getc(pFile);
            rgb[i][j].rgbGreen = getc(pFile);
            rgb[i][j].rgbRed = getc(pFile);
        } 
        // пропускаем последний байт в строке
        getc(pFile);
    }
	fclose(pFile);
	return 0;
}

int BMP_Write(char fName[],BITMAPFILEHEADERS header,BITMAPINFOHEADERS bmiHeader,RGBQUADS rgb[1200][1000])
{
	FILE *pFile = fopen(fName,"w+b");
	//закатываем заголовки
	fwrite(&header.bfType,2*sizeof(char),1,pFile);
	fwrite(&header.bfSize,sizeof(int),1,pFile);
	fwrite(&header.bfReserved1,sizeof(short),1,pFile);
	fwrite(&header.bfReserved2,sizeof(short),1,pFile);
	fwrite(&header.bfOffBits,sizeof(long),1,pFile);

	fwrite(&bmiHeader.biSize,sizeof(unsigned int),1,pFile);
	fwrite(&bmiHeader.biWidth,sizeof(int),1,pFile);
	fwrite(&bmiHeader.biHeight,sizeof(int),1,pFile);
	fwrite(&bmiHeader.biPlanes,sizeof(unsigned short),1,pFile);
	fwrite(&bmiHeader.biBitCount,sizeof(unsigned short),1,pFile);
	fwrite(&bmiHeader.biCompression,sizeof(unsigned int),1,pFile);
	fwrite(&bmiHeader.biSizeImage,sizeof(unsigned int),1,pFile);
	fwrite(&bmiHeader.biXPelsPerMeter,sizeof(int),1,pFile);
	fwrite(&bmiHeader.biYPelsPerMeter,sizeof(int),1,pFile);
	fwrite(&bmiHeader.biClrUsed,sizeof(unsigned int),1,pFile);
	fwrite(&bmiHeader.biClrImportant,sizeof(unsigned int),1,pFile);
	//заливаем картиночность :3
    for (int i = 0; i < bmiHeader.biWidth; i++) {
    for (int j = 0; j < bmiHeader.biHeight; j++) {
		fwrite(&rgb[i][j].rgbBlue,sizeof(char),1,pFile);
		fwrite(&rgb[i][j].rgbGreen,sizeof(char),1,pFile);
		fwrite(&rgb[i][j].rgbRed,sizeof(char),1,pFile);
	}

	fwrite("g",sizeof(char),1,pFile);
	}
	fclose(pFile);
	return 0;
}

int Encode(char reco[10][20], char fName[])
{
	BITMAPFILEHEADERS header;
	BITMAPINFOHEADERS bmiHeader;
	RGBQUADS rgb[1200][1000];
	BMP_Read(fName,header,bmiHeader,rgb);
	//засовываем символы в пиксели, магия побитовых сдвигов
	for (int i = 0; i < 10; i++)
    for (int j = 0; j < 20; j++) 
	{
		rgb[i][j].rgbRed>>=3;
		rgb[i][j].rgbRed<<=3;
		rgb[i][j].rgbRed|=reco[i][j]>>5;
		rgb[i][j].rgbGreen>>=2;
		rgb[i][j].rgbGreen<<=2;
		rgb[i][j].rgbGreen|=(reco[i][j]&24)>>3;
		rgb[i][j].rgbBlue>>=3;
		rgb[i][j].rgbBlue<<=3;
		rgb[i][j].rgbBlue|=(reco[i][j]&7);
	}
	BMP_Write(fName,header,bmiHeader,rgb);
    return 0;
}
 
int Decode(char reco[10][20], char fName[])
{
	BITMAPFILEHEADERS header;
	BITMAPINFOHEADERS bmiHeader;
	RGBQUADS rgb[1200][1000];
	BMP_Read(fName,header,bmiHeader,rgb);
	//засовываем символы в пиксели, магия побитовых сдвигов
	for (int i = 0; i < 10; i++)
    for (int j = 0; j < 20; j++) 	
			reco[i][j]=((((rgb[i][j].rgbRed&7)<<2)|(rgb[i][j].rgbGreen&3))<<3)|(rgb[i][j].rgbBlue&7);	 //бляя
    return 0;
}

 
static unsigned short read_u16(FILE *fp)
{
    unsigned char b0, b1;
 
    b0 = getc(fp);
    b1 = getc(fp);
 
    return ((b1 << 8) | b0);
}
 
 
static unsigned int read_u32(FILE *fp)
{
    unsigned char b0, b1, b2, b3;
 
    b0 = getc(fp);
    b1 = getc(fp);
    b2 = getc(fp);
    b3 = getc(fp);
 
    return ((((((b3 << 8) | b2) << 8) | b1) << 8) | b0);
}
 
 
static int read_s32(FILE *fp)
{
    unsigned char b0, b1, b2, b3;
 
    b0 = getc(fp);
    b1 = getc(fp);
    b2 = getc(fp);
    b3 = getc(fp);
 
    return ((int)(((((b3 << 8) | b2) << 8) | b1) << 8) | b0);
}