#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "General.h"
#include "FileHelper.h"
#include "SuperFile.h"
#include "Product.h"
#include "myMacros.h"

int	saveSuperMarketToFile(const SuperMarket* pMarket, const char* fileName, const char* customersFileName, int saveCompressionMode)
{
	FILE* fp;
	fp = fopen(fileName, "wb");
	//if (!fp) {
	//	printf("Error opening supermarket file for writing\n");
	//	return 0;
	//}

	CHECK_MSG_RETURN_0(fp, "Error opening supermarket file for writing");

	if (saveCompressionMode == 0) {
		// Regular saving
		saveRegularSuperFile(pMarket, fp);
	}
	else {
		// Compressed saving
		saveCompressedSuperFile(pMarket, fp);
	}

	fclose(fp);

	saveCustomersToTextFile(pMarket->customerArr, pMarket->customerCount, customersFileName);

	return 1;
}

int saveRegularSuperFile(const SuperMarket* pMarket, FILE* fp) {
	if (!writeStringToFile(pMarket->name, fp, "Error writing supermarket name\n"))
		return 0;

	if (!writeIntToFile(pMarket->productCount, fp, "Error writing product count\n"))
		return 0;

	for (int i = 0; i < pMarket->productCount; i++) {
		if (!saveProductToFile(pMarket->productArr[i], fp))
			return 0;
	}

	return 1;
}

int saveCompressedSuperFile(const SuperMarket* pMarket, FILE* fp) {
	// First 10 bits: product count, Next 6 bits: name length
	unsigned char byte1, byte2;
	int nameLength = (int)strlen(pMarket->name);
	int productCount = pMarket->productCount;

	// Packing first two bytes
	byte1 = (productCount >> 2) & 0xFF;  // First 8 bits of product count
	byte2 = ((productCount & 0x3) << 6) | (nameLength & 0x3F);  // Remaining 2 bits + name length

	fwrite(&byte1, sizeof(unsigned char), 1, fp);
	fwrite(&byte2, sizeof(unsigned char), 1, fp);

	// Write supermarket name
	fwrite(pMarket->name, sizeof(char), nameLength, fp);

	// Save each product
	for (int i = 0; i < productCount; i++) {
		if (!saveCompressedProduct(pMarket->productArr[i], fp))
			return 0;
	}

	return 1;
}




int	loadSuperMarketFromFile(SuperMarket* pMarket, const char* fileName,const char* customersFileName, int loadCompressionMode)
{
	FILE* fp;
	fp = fopen(fileName, "rb");
	/*if (!fp)
	{
		printf("Error open company file\n");
		return 0;
	}*/
	CHECK_MSG_RETURN_0(fp, "Error opening company file");




	if (loadCompressionMode == 0) {
		loadRegularSuperFile(pMarket, fp);
	}

	else {
		loadCompressedSuperFile(pMarket, fp);
	}

	fclose(fp);

	pMarket->customerArr = loadCustomersFromTextFile(customersFileName, &pMarket->customerCount);
	/*if (!pMarket->customerArr)
		return 0;*/
	CHECK_RETURN_0(pMarket->customerArr);

	return	1;

}

int loadRegularSuperFile(SuperMarket* pMarket, FILE* fp) {
	pMarket->name = readStringFromFile(fp, "Error reading supermarket name\n");
	if (!pMarket->name)
	{
		/*fclose(fp);
		return 0;*/
		CLOSE_RETURN_0(fp);
	}

	int count;

	if (!readIntFromFile(&count, fp, "Error reading product count\n"))
	{
		/*free(pMarket->name);
		fclose(fp);
		return 0;*/
		FREE_CLOSE_FILE_RETURN_0(pMarket->name, fp);
	}

	pMarket->productArr = (Product**)malloc(count * sizeof(Product*));
	if (!pMarket->productArr)
	{
		free(pMarket->name);
		/*fclose(fp);
		return 0;*/
		CLOSE_RETURN_0(fp);
	}

	pMarket->productCount = count;

	for (int i = 0; i < count; i++)
	{
		pMarket->productArr[i] = (Product*)malloc(sizeof(Product));
		if (!pMarket->productArr[i])
		{
			/*free(pMarket->name);
			fclose(fp);
			return 0;*/

			FREE_CLOSE_FILE_RETURN_0(pMarket->name, fp);

		}

		if (!loadProductFromFile(pMarket->productArr[i], fp))
		{
			free(pMarket->productArr[i]);
			/*free(pMarket->name);
			fclose(fp);
			return 0;*/

			FREE_CLOSE_FILE_RETURN_0(pMarket->name, fp);
		}
	}
}







int loadCompressedSuperFile(SuperMarket* pMarket, FILE* fp) {
	unsigned char byte1, byte2;
	int productCount, nameLength;

	if (fread(&byte1, sizeof(unsigned char), 1, fp) != 1 ||
		fread(&byte2, sizeof(unsigned char), 1, fp) != 1) {
		printf("Error reading header\n");
		return 0;
	}

	productCount = ((byte1 << 2) | (byte2 >> 6)) & 0x3FF;
	nameLength = byte2 & 0x3F;

	pMarket->productCount = productCount;
	pMarket->productArr = (Product**)malloc(productCount * sizeof(Product*));
	if (!pMarket->productArr) {
		printf("Memory allocation failed for products array\n");
		return 0;
	}

	pMarket->name = (char*)malloc(nameLength + 1);
	fread(pMarket->name, sizeof(char), nameLength, fp);
	pMarket->name[nameLength] = '\0';

	for (int i = 0; i < productCount; i++) {
		pMarket->productArr[i] = (Product*)malloc(sizeof(Product));
		Product* pProduct = pMarket->productArr[i];

		unsigned char barcodeBytes[3];
		fread(barcodeBytes, sizeof(unsigned char), 3, fp);

		pProduct->barcode[2] = '0' + ((barcodeBytes[0] >> 4) & 0xF); // First digit
		pProduct->barcode[3] = '0' + (barcodeBytes[0] & 0xF);        // Second digit
		pProduct->barcode[4] = '0' + ((barcodeBytes[1] >> 4) & 0xF); // Third digit
		pProduct->barcode[5] = '0' + (barcodeBytes[1] & 0xF);        // Fourth digit
		pProduct->barcode[6] = '0' + ((barcodeBytes[2] >> 4) & 0xF); // Fifth digit
		pProduct->barcode[7] = '\0';  // Null-terminate

		pProduct->type = (barcodeBytes[2] >> 2) & 0x3;
		const char* typePrefixes[] = { "FV", "FR", "FZ", "SH" };

		if (pProduct->type >= 0 && pProduct->type < 4) {
			pProduct->barcode[0] = typePrefixes[pProduct->type][0];
			pProduct->barcode[1] = typePrefixes[pProduct->type][1];
		}
		else {
			pProduct->barcode[0] = 'X';  // Invalid type marker
			pProduct->barcode[1] = 'X';
		}

		unsigned char byte3;
		fread(&byte3, sizeof(unsigned char), 1, fp);

		int productNameLength = ((barcodeBytes[2] & 0x3) << 2) | ((byte3 >> 6) & 0x3);

		if (productNameLength > 0 && productNameLength <= NAME_LENGTH) {
			memset(pProduct->name, 0, NAME_LENGTH + 1);  // Clear name buffer
			fread(pProduct->name, sizeof(char), productNameLength, fp);
			pProduct->name[productNameLength] = '\0';  // Null-terminate
		}
		else {
			printf("[ERROR] Invalid name length: %d\n", productNameLength);
		}

		unsigned char quantity;
		fread(&quantity, sizeof(unsigned char), 1, fp);
		pProduct->count = quantity;

		unsigned char priceFraction;
		unsigned char priceWholeLow;
		fread(&priceFraction, sizeof(unsigned char), 1, fp);
		fread(&priceWholeLow, sizeof(unsigned char), 1, fp);
		int priceWhole = ((priceWholeLow & 0xFF) | ((priceFraction & 0x80) << 1)) & 0x1FF;  // 9-bit mask
		int priceFractionPart = (priceFraction & 0x7F) / 2;  // Divide by 2 to correct scaling
		pProduct->price = priceWhole + (priceFractionPart / 100.0f);
		unsigned char dateBytes[2];
		fread(dateBytes, sizeof(unsigned char), 2, fp);
		pProduct->expiryDate.day = (dateBytes[0] >> 3) & 0x1F;
		pProduct->expiryDate.month = ((dateBytes[0] & 0x07) << 1) | ((dateBytes[1] >> 7) & 0x01);
		pProduct->expiryDate.year = 2024 + ((dateBytes[1] >> 4) & 0x07);  // Extract bits 4-6 for the year
	}
	return 1;
}






int	saveCustomersToTextFile(const Customer* customerArr, int customerCount, const char* customersFileName)
{
	FILE* fp;

	fp = fopen(customersFileName, "w");
	if (!fp) {
		printf("Error opening customers file to write\n");
		return 0;
	}

	fprintf(fp, "%d\n", customerCount);
	for (int i = 0; i < customerCount; i++)
		customerArr[i].vTable.saveToFile(&customerArr[i], fp);

	fclose(fp);
	return 1;
}

Customer* loadCustomersFromTextFile(const char* customersFileName, int* pCount)
{
	FILE* fp;

	fp = fopen(customersFileName, "r");
	if (!fp) {
		printf("Error open customers file to write\n");
		return NULL;
	}


	Customer* customerArr = NULL;
	int customerCount;

	fscanf(fp, "%d\n", &customerCount);

	if (customerCount > 0)
	{
		customerArr = (Customer*)calloc(customerCount, sizeof(Customer)); //cart will be NULL!!!
		if (!customerArr)
		{
			fclose(fp);
			return NULL;
		}

		for (int i = 0; i < customerCount; i++)
		{
			if (!loadCustomerFromFile(&customerArr[i], fp))
			{
				freeCustomerCloseFile(customerArr, i, fp);
				return NULL;
			}
		}
	}

	fclose(fp);
	*pCount = customerCount;
	return customerArr;
}


void freeCustomerCloseFile(Customer* customerArr, int count, FILE* fp)
{
	for (int i = 0; i < count; i++)
	{
		free(customerArr[i].name);
		customerArr[i].name = NULL;
		if (customerArr[i].pDerivedObj)
		{
			free(customerArr[i].pDerivedObj);
			customerArr[i].pDerivedObj = NULL;
		}
	}
	free(customerArr);
	fclose(fp);
}