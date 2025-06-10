#pragma once

#include <stdio.h>
#include "Supermarket.h"
#include "Customer.h"

int			saveSuperMarketToFile(const SuperMarket* pMarket, const char* fileName, const char* customersFileName, int saveCompressionMode);
int			loadSuperMarketFromFile(SuperMarket* pMarket, const char* fileName, const char* customersFileName, int loadCompressionMode);
int			saveCustomersToTextFile(const Customer* customerArr, int customerCount,const char* customersFileName);
Customer*	loadCustomersFromTextFile(const char* customersFileName, int* pCount);
void		freeCustomerCloseFile(Customer* customerArr, int customerIndex, FILE* fp);

