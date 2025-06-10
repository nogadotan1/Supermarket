#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "main.h"
#include "General.h"
#include "Supermarket.h"
#include "SuperFile.h"
#include "Variadics.h"

int main(int argc, char*argv[])
{
	SuperMarket	market;
	const char* superFileName = NULL;
	int compressionMode = 0;

	if (argc > 1) {
		if (sscanf(argv[1], "%d", &compressionMode) != 1 || (compressionMode != 0 && compressionMode != 1)) {
			printf("Invalid argument. Use 0 for uncompressed, 1 for compressed.\n");
			return 1;
		}
	}

	printf("There are %d arguments.\n", argc);
	printf("Compression mode is: %s\n", compressionMode ? "COMPRESSED" : "UNCOMPRESSED");

	if (argc > 2) {
		superFileName = argv[2];
		printf("File name received: %s\n", superFileName);
	}
	else {
		printf("Error: No file name provided.\n");
		return 1;
	}

	// Try opening the file to check if it exists
	FILE* testFile = fopen(superFileName, "rb");
	if (!testFile) {
		printf("Error: Cannot open file %s. Please check the file path.\n", superFileName);
		return 1;
	}
	else {
		printf("Success: File %s found!\n", superFileName);
		fclose(testFile);
	}




	srand((unsigned int)time(NULL));

	if (!initSuperMarket(&market, superFileName, CUSTOMER_FILE_NAME, compressionMode))
	{
		printf("error init Super Market");
		return 0;
	}

	int option;
	int stop = 0;
	
	do
	{
		option = menu();
		switch (option)
		{
		case eShowSuperMarket:
			printSuperMarket(&market);
			break;

		case eAddProduct:
			if (!addProduct(&market))
				printf("Error adding product\n");
			break;

		case eAddCustomer:
			if (!addCustomer(&market))
				printf("Error adding customer\n");
			break;

		case eCustomerDoShopping:
			if(!doShopping(&market))
				printf("Error in shopping\n");
			break;

		case ePrintCart:
			doPrintCart(&market);
			break;

		case eCustomerManageShoppingCart:
			if(!manageShoppingCart(&market))
				printf("Error in shopping cart managment\n");
			break;

		case eSortProducts:
			sortProducts(&market);
			break;

		case eSearchProduct:
			findProduct(&market);
			break;

		case ePrintProductByType:
			printProductByType(&market);
			break;

		case EXIT:
			printMessage("Thank", "You", "For", "Shopping", "With", "Us", NULL);
			stop = 1;
			break;

		default:
			printf("Wrong option\n");
			break;
		}
	} while (!stop);

	handleCustomerStillShoppingAtExit(&market);

	if (!saveSuperMarketToFile(&market, superFileName, CUSTOMER_FILE_NAME,compressionMode))
		printf("Error saving supermarket to file\n");

	freeMarket(&market);
	
	return 1;
}

int menu()
{
	int option;
	printf("\n");
	printf("Please choose one of the following options\n");
	for(int i = 0 ; i < eNofOptions ; i++)
		printf("%d - %s\n",i, menuStrings[i]);
	printf("%d - Quit\n", EXIT);
	scanf("%d", &option);
	//clean buffer
	char tav;
	scanf("%c", &tav);
	return option;
}

