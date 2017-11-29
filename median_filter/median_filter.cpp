#include <iostream>
#include <fstream>
#include <Windows.h>
#include <math.h>
#include <algorithm>

using namespace std;

unsigned char** MemAlloc2D(int nHeight, int nWidth, unsigned char nInitVal)
{
	unsigned char** rtn = new unsigned char*[nHeight];
	for (int n = 0; n < nHeight; n++)
	{
		rtn[n] = new unsigned char[nWidth];
		memset(rtn[n], nInitVal, sizeof(unsigned char) * nWidth);
	}
	return rtn;
}

void MemFree2D(unsigned char **Mem, int nHeight)
{
	for (int n = 0; n < nHeight; n++)
	{
		delete[] Mem[n];
	}
	delete[] Mem;
}

void MedianFilter(unsigned char **In_Pad, unsigned char **Out, int nHeight, int nWidth, int nFilterSize)
{
	BYTE *arr;
	arr = new BYTE[nFilterSize*nFilterSize];

	FILE *out_Median;
	fopen_s(&out_Median, "median3.raw", "wb");

	for (int h = 0; h < nHeight; h++)
	{
		for (int w = 0; w < nWidth; w++)
		{
			int i = 0;
			for (int n = 0; n < nFilterSize; n++)
			{
				for (int m = 0; m< nFilterSize; m++)
				{
					arr[i] = In_Pad[h + n][w + m];
					i++;
				}
			}
			sort(arr, arr + nFilterSize*nFilterSize);

			Out[h][w] = arr[(int)(nFilterSize*nFilterSize / 2)];
		}
	}

	for (int h = 0; h < 256; h++)
	{
		fwrite(Out[h], sizeof(unsigned char), 256, out_Median);
	}

	fclose(out_Median);
}

unsigned char** Padding(unsigned char** In, int nHeight, int nWidth, int nFilterSize)
{
	int nPadSize = (int)(nFilterSize / 2);
	unsigned char** Pad = MemAlloc2D(nHeight + 2 * nPadSize, nWidth + 2 * nPadSize, 0);

	for (int h = 0; h < nHeight; h++)
	{
		for (int w = 0; w < nWidth; w++)
		{
			Pad[h + nPadSize][w + nPadSize] = In[h][w];
		}
	}

	for (int h = 0; h < nPadSize; h++)
	{
		for (int w = 0; w < nWidth; w++)
		{
			Pad[h][w + nPadSize] = In[0][w];
			Pad[h + (nHeight - 1)][w + nPadSize] = In[nHeight - 1][w];
		}
	}

	for (int h = 0; h < nHeight; h++)
	{
		for (int w = 0; w < nPadSize; w++)
		{
			Pad[h + nPadSize][w] = In[h][0];
			Pad[h + nPadSize][w + (nWidth - 1)] = In[h][nWidth - 1];
		}
	}

	for (int h = 0; h < nPadSize; h++)
	{
		for (int w = 0; w < nPadSize; w++)
		{
			Pad[h][w] = In[0][0];
			Pad[h + (nHeight - 1)][w] = In[nHeight - 1][0];
			Pad[h][w + (nWidth - 1)] = In[0][nWidth - 1];
			Pad[h + (nHeight - 1)][w + (nWidth - 1)] = In[nHeight - 1][nWidth - 1];
		}
	}

	return Pad;
}

int main()
{
	int width = 256;
	int height = 256;

	unsigned char **input_img;
	unsigned char **out_img;
	unsigned char **Pad_img;

	FILE *infile;
	fopen_s(&infile, "lena256_n25.raw", "rb");

	input_img = MemAlloc2D(width, height, 0);
	out_img = MemAlloc2D(width, height, 0);

	for (int h = 0; h < height; h++)
	{
		fread(input_img[h], sizeof(unsigned char), width, infile);
	}

	Pad_img = Padding(input_img, height, width, 3);

	MedianFilter(Pad_img, out_img, height, width, 3);

	MemFree2D(input_img, 256);
	MemFree2D(out_img, 256);
	fclose(infile);

	return 0;
}