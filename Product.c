#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "Product.h"
#include "General.h"
#include "FileHelper.h"

#define MIN_DIG 3
#define MAX_DIG 5

static const char* typeStr[eNofProductType] = { "Fruit Vegtable", "Fridge", "Frozen", "Shelf" };
static const char* typePrefix[eNofProductType] = { "FV", "FR", "FZ", "SH" };


void	initProduct(Product* pProduct)
{
	initProductNoBarcode(pProduct);
	generateBarcode(pProduct);
}

void	initProductNoBarcode(Product* pProduct)
{
	initProductName(pProduct);
	pProduct->type = getProductType();
	initDate(&pProduct->expiryDate);
	pProduct->price = getPositiveFloat("Enter product price\t");
	pProduct->count = getPositiveInt("Enter product number of items\t");
}

void initProductName(Product* pProduct)
{
	do {
		printf("enter product name up to %d chars\n", NAME_LENGTH);
		myGets(pProduct->name, sizeof(pProduct->name), stdin);
	} while (checkEmptyString(pProduct->name));
}

int		saveProductToFile(const Product* pProduct, FILE* fp)
{
	if (fwrite(pProduct, sizeof(Product), 1, fp) != 1)
	{
		puts("Error saving product to file\n");
		return 0;
	}
	return 1;
}

int		loadProductFromFile(Product* pProduct, FILE* fp)
{
	if (fread(pProduct, sizeof(Product), 1, fp) != 1)
	{
		puts("Error reading product from file\n");
		return 0;
	}
	return 1;
}

int	compareProductsByName(const void* prod1, const void* prod2)
{
	const Product* pProd1 = *(Product**)prod1;
	const Product* pProd2 = *(Product**)prod2;

	return strcmp(pProd1->name, pProd2->name);
}

int	compareProductsByCount(const void* prod1, const void* prod2)
{
	const Product* pProd1 = *(Product**)prod1;
	const Product* pProd2 = *(Product**)prod2;

	return pProd1->count - pProd2->count;
}

int	compareProductsByPrice(const void* prod1, const void* prod2)
{
	const Product* pProd1 = *(Product**)prod1;
	const Product* pProd2 = *(Product**)prod2;

	if (pProd1->price > pProd2->price)
		return 1;
	if (pProd1->price < pProd2->price)
		return -1;
	return 0;
}

void printProduct(const Product* pProduct)
{
	char* dateStr = getDateStr(&pProduct->expiryDate);
	printf("%-20s %-10s\t", pProduct->name, pProduct->barcode);
	printf("%-20s %5.2f %13d %7s %15s\n", typeStr[pProduct->type], pProduct->price, pProduct->count, " ", dateStr);
	free(dateStr);
}

void printProductPtr(void* v1)
{
	Product* pProduct = *(Product**)v1;
	printProduct(pProduct);
}

void generateBarcode(Product* pProd)
{
	char temp[BARCODE_LENGTH + 1];
	int barcodeNum;

	strcpy(temp, getProductTypePrefix(pProd->type));
	do {
		barcodeNum = MIN_BARCODE + rand() % (RAND_MAX - MIN_BARCODE + 1); //Minimum 5 digits
	} while (barcodeNum > MAX_BARCODE);
	
	sprintf(temp + strlen(temp), "%d", barcodeNum);

	strcpy(pProd->barcode, temp);
}

void getBarcodeCode(char* code)
{
	char temp[MAX_STR_LEN];
	char msg[MAX_STR_LEN];
	sprintf(msg, "Code should be of %d length exactly\n"
				 "Must have %d type prefix letters followed by a %d digits number\n"
				 "For example: FR20301",
				 BARCODE_LENGTH, PREFIX_LENGTH, BARCODE_DIGITS_LENGTH);
	int ok = 1;
	int digCount = 0;
	do {
		ok = 1;
		digCount = 0;
		getsStrFixSize(temp, MAX_STR_LEN, msg);
		if (strlen(temp) != BARCODE_LENGTH)
		{
			puts("Invalid barcode length");
			ok = 0;
		}
		else
		{
			//check first PREFIX_LENGTH letters are upper case and valid type prefix
			char* typeSubStr = (char*)malloc(PREFIX_LENGTH + 1);
			if (!typeSubStr)
				return;
			strncpy(typeSubStr, temp, PREFIX_LENGTH);
			typeSubStr[PREFIX_LENGTH] = '\0';
			int prefixOk = 0;
			int i;

			for (i = 0; i < eNofProductType; i++)
			{
				if (strcmp(typeSubStr, typePrefix[i]) == 0)
				{
					prefixOk = 1;
					break; //found valid type prefix
				}
			}

			free(typeSubStr); //free the allocated memory

			if (!prefixOk)
			{
				puts("Invalid type prefix");
				ok = 0;
			}
			else
			{
				for (i = PREFIX_LENGTH; i < BARCODE_LENGTH; i++)
				{
					if (!isdigit(temp[i]))
					{
						puts("Only digits after type prefix\n");
						puts(msg);
						ok = 0;
						break;
					}
					digCount++;
				}

				if (digCount != BARCODE_DIGITS_LENGTH)
				{
					puts("Incorrect number of digits");
					ok = 0;
				}
			}
		}
	} while (!ok);

	strcpy(code, temp);
}

eProductType getProductType()
{
	int option;

	printf("\n");
	do {
		printf("Please enter one of the following types\n");
		for (int i = 0; i < eNofProductType; i++)
			printf("%d for %s\n", i, typeStr[i]);
		scanf("%d", &option);
	} while (option < 0 || option >= eNofProductType);

	getchar();

	return (eProductType)option;
}

const char* getProductTypeStr(eProductType type)
{
	if (type < 0 || type >= eNofProductType)
		return NULL;
	return typeStr[type];
}

const char* getProductTypePrefix(eProductType type)
{
	if (type < 0 || type >= eNofProductType)
		return NULL;
	return typePrefix[type];
}

int isProduct(const Product* pProduct, const char* barcode)
{
	if (strcmp(pProduct->barcode, barcode) == 0)
		return 1;
	return 0;
}

void updateProductCount(Product* pProduct)
{
	int count;

	do {
		printf("How many items to add to stock? ");
		scanf("%d", &count);
	} while (count < 1);

	pProduct->count += count;
}


void	freeProduct(Product* pProduct)
{
	//nothing to free!!!!
}

int saveCompressedProduct(const Product* pProduct, FILE* fp) {
	unsigned char barcodeBytes[3];

	// Convert barcode digits to 20-bit packed format
	barcodeBytes[0] = ((pProduct->barcode[2] - '0') << 4) | (pProduct->barcode[3] - '0');
	barcodeBytes[1] = ((pProduct->barcode[4] - '0') << 4) | (pProduct->barcode[5] - '0');
	barcodeBytes[2] = ((pProduct->barcode[6] - '0') << 4);  // Store last barcode digit
	barcodeBytes[2] |= ((pProduct->type & 0x03) << 2);  // Fix: Shift type bits left by 2


	// Compute name length (4 bits)
	int nameLength = strlen(pProduct->name) & 0x0F;

	// Pack name length
	barcodeBytes[2] = (barcodeBytes[2] & 0xFC) | ((nameLength >> 2) & 0x03);
	unsigned char byte3 = (nameLength & 0x03) << 6;

	// Write barcode and byte3
	fwrite(barcodeBytes, sizeof(unsigned char), 3, fp);
	fwrite(&byte3, sizeof(unsigned char), 1, fp);

	// Write name
	fwrite(pProduct->name, sizeof(char), nameLength, fp);

	// Write quantity (8 bits)
	fwrite(&pProduct->count, sizeof(unsigned char), 1, fp);

	// Fix: Correct price packing
	unsigned char priceFraction, priceWholeLow;
	int priceWhole = (int)pProduct->price;
	int priceFractionPart = (int)((pProduct->price - priceWhole) * 100);

	priceFraction = ((priceFractionPart & 0x7F) << 1) | ((priceWhole >> 8) & 0x01);
	priceWholeLow = priceWhole & 0xFF;

	fwrite(&priceFraction, sizeof(unsigned char), 1, fp);
	fwrite(&priceWholeLow, sizeof(unsigned char), 1, fp);

	// Fix: Correct expiry date storage
	unsigned char dateBytes[2];

	dateBytes[0] = ((pProduct->expiryDate.day & 0x1F) << 3) | ((pProduct->expiryDate.month >> 1) & 0x07);
	dateBytes[1] = ((pProduct->expiryDate.month & 0x01) << 7) | (((pProduct->expiryDate.year - 2024) & 0x07) << 4);


	fwrite(dateBytes, sizeof(unsigned char), 2, fp);

	return 1;
}





