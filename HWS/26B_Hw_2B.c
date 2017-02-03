/*********************************************************************************
** CIS 26B                                                            Winter, 2017
** Advanced C
******************
**
** Homework 2: 
**        A Circularly Doubly Linked List of Stacks with a Sentinel Node
**
**********************************************************************************

   This program...

   Save the output as a comment at the end of the program!
**********************************************************
**
**  Written By: Brett Berens
**
**  Date: 1/22/2017
***************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef _MSC_VER
#include <crtdbg.h>  // needed to check for memory leaks (Windows only!)
#endif

#define DUMMY_TRAILER '\177'

typedef struct quote QUOTE;
struct quote
{
	double price;
	QUOTE *next;
};

typedef struct stock STOCK;
struct stock
{
	char  symbol[20];
	QUOTE *quoteStack;
	int numQuotes;
	STOCK *forw;
	STOCK *back;
};

void displayManager(STOCK *list);
STOCK *findStock(STOCK *list, char *symbol);
void getFilename(char *filename, int length);
STOCK *init_list(void);
QUOTE *pop(QUOTE **stack);
void printQuotes(QUOTE *stack, int numQuotes);
QUOTE *push(QUOTE *stack, double price);
int readStocksFromFile(STOCK *stockList);
void searchManager(STOCK *list);
void traverse_forw(STOCK *list);
void traverse_back(STOCK *list);
void traverse_forwRange(STOCK *start, STOCK *finish);
void traverse_backRange(STOCK *start, STOCK *finish);

int main( void )
{
	
	STOCK *stockList;

	stockList = init_list();

	printf("Ascending:\n");
	traverse_forw(stockList);
	
	printf("Descending:\n");
	traverse_back(stockList);

	displayManager(stockList);

	searchManager(stockList);


    #ifdef _MSC_VER
        printf( _CrtDumpMemoryLeaks() ? "Memory Leak\n" : "No Memory Leak\n");
    #endif
	system("pause");
    return 0;
}

/***************************************************
Initialization of a circularly doubly-linked
list with one sentinel node
*/
STOCK *init_list(void)
{
	STOCK *list;

	// allocate the sentinel node
	list = (STOCK *)malloc(sizeof(STOCK));
	if (!list)
	{
		printf("Error in init_list!\n");
		exit(1);
	}
	list->symbol[0] = DUMMY_TRAILER;
	list->symbol[1] = '\0';
	list->forw = list;
	list->back = list;

	return list;
}

/***************************************************
Insert a node in a sorted circularly
doubly-linked list with one sentinel node
return 1 - if duplicate
return 0 - otherwise
*/
int insert(STOCK *list, char *symbol, double price)// need to add a push for the quote stack when allocating a new Stock
{
	STOCK *curr = list->forw;
	STOCK *prev = list;
	STOCK *pnew;
	int   duplicate = 1;

	// search
	while (strcmp(symbol, curr->symbol) > 0)
	{
		prev = curr;
		curr = curr->forw;
	}

	if (strcmp(symbol, curr->symbol))
	{
		duplicate = 0; // not a duplicate
		pnew = (STOCK *)malloc(sizeof(STOCK));
		if (!pnew)
		{
			printf("Fatal memory allocation error in insert!\n");
			exit(3);
		}
		pnew->quoteStack = NULL;	// initialize quote stack
		pnew->quoteStack = push(pnew->quoteStack, price);
		pnew->numQuotes = 1;
		strcpy(pnew->symbol, symbol);
		pnew->forw = curr;
		pnew->back = prev;
		prev->forw = pnew;
		curr->back = pnew;
	}
	else
	{
		curr->quoteStack = push(curr->quoteStack, price);
		curr->numQuotes += 1;
	}
	return duplicate;
}

/***************************************************
Traverses forward a circularly doubly-linked
list with one sentinel node to print out the
contents of each node
*/
void traverse_forw(STOCK *list)
{
	putchar('\n');
	list = list->forw; // skip the dummy node
	while (list->symbol[0] != DUMMY_TRAILER)
	{
		printf("%-6s $%.2f\n", list->symbol, list->quoteStack->price);
		list = list->forw;		
	}
	return;
}

/***************************************************
Traverses backward a circularly doubly-linked
list with one sentinel node to print out the
contents of each node
*/
void traverse_back(STOCK *list)
{
	putchar('\n');
	list = list->back; // skip the dummy node
	while (list->symbol[0] != DUMMY_TRAILER)
	{
		printf("%-6s $%.2f\n", list->symbol, list->quoteStack->price);
		list = list->back;
	}
	return;
}

int readStocksFromFile(STOCK *stockList)
{
	FILE *fpInput;
	double tempPrice;
	char tempsymbol[20];
	char filename[100];
	int isInvalid = 1;

	while (isInvalid)
	{
		getFilename(filename, sizeof(filename));
		isInvalid = readStocksFromFile(filename, stockList);
	}

	if (!(fpInput = fopen(filename, "r")))
	{
		printf("Error opening %s for reading. Try again.\n", filename);
		return 1;
	}
	while (fscanf(fpInput, "%s %lf\n", &tempsymbol, &tempPrice) != EOF) // scan in each stock symbol and share and store in a STOCK
		insert(stockList, tempsymbol, tempPrice);
	fclose(fpInput);
	return 0;
}

/*	========================================================
Prompt user to enter a filename and stores the user input or default value
Pre		filename may or may not be empty when passed in
inOrOut indicates whether the filename will be used as input or output and determines the default string to be used if necessary
Post	filename now contains either the user's input or the default string "stocks.txt"
*/
void getFilename(char *filename, int length)
{
	char defaultName[] = "stocks.txt";
	printf("Enter the name of the file to read stock information from. Alternatively press 'enter' to use the default file.\n");
	fgets(filename, length, stdin);
	*(filename + strlen(filename) - 1) = '\0';
	if (!strcmp(filename, ""))
	{
		strcpy(filename, defaultName);
		printf("Using %s for input.\n", defaultName);
	}
}

/***************************************************
Stack Insert: insert in the beginning
*/
QUOTE *push(QUOTE *stack, double price)
{
	QUOTE *pnew;

	pnew = (QUOTE *)malloc(sizeof(QUOTE));
	if (!pnew)
	{
		printf("... error in push!\n");
		exit(1);
	}
	pnew->price = price;
	pnew->next = stack;
	stack = pnew;

	return stack;
}

/***************************************************
Stack Delete: delete the first node
*/
QUOTE *pop(QUOTE **stack)
{
	QUOTE *first;

	if (*stack == NULL) return NULL;
	first = *stack;
	*stack = (*stack)->next;
	first->next = NULL;

	return first;
}

/***************************************************
Stack Print: delete the first node
*/
void printQuotes(QUOTE *stack, int numQuotes)
{
	for(int quote = 1; quote <= numQuotes; quote++)
	{
		printf("       $%.2f\n", stack->price);
		stack = stack->next;
	}
}

void displayManager(STOCK *list)
{
	char startSymbol[100] = {0};
	char finishSymbol[100];
	STOCK *startStock, *finishStock;
	
	while (strcmp(startSymbol, "QUIT"))
	{
		printf("Enter the stock symbol to start from or enter 'QUIT' to stop: ");
		fgets(startSymbol, 99, stdin);
		*(startSymbol + strlen(startSymbol) - 1) = '\0';
		if (!strcmp(startSymbol, "QUIT")) break;
		printf("Enter the stock symbol to finish at: ");
		fgets(finishSymbol, 99, stdin);
		*(finishSymbol + strlen(finishSymbol) - 1) = '\0';

		startStock = findStock(list, startSymbol);
		finishStock = findStock(list, finishSymbol);

		if (startStock && finishStock)
		{
			printf("Ascending:\n");
			traverse_forwRange(startStock, finishStock);
			printf("Descending:\n");
			traverse_backRange(startStock, finishStock);
		}
		else if (startStock) traverse_forwRange(startStock, startStock);
		else if (finishStock) traverse_forwRange(finishStock, finishStock);
		else printf("Neither stock symbol is found.\n");
	}
}

/***************************************************
Traverses forward a circularly doubly-linked
list with one sentinel node to print out the
contents of each node
*/
void traverse_forwRange(STOCK *start,STOCK *finish)
{
	STOCK *currStock = start;
	while (strcmp(currStock->symbol, finish->symbol))
	{
		if(currStock->symbol[0] != DUMMY_TRAILER) 
			printf("%-6s $%.2f\n", currStock->symbol, currStock->quoteStack->price);
		currStock = currStock->forw;
	}
	printf("%-6s $%.2f\n", currStock->symbol, currStock->quoteStack->price);
	
	return;
}


void traverse_backRange(STOCK *start, STOCK *finish)
{
	STOCK *currStock = start;
	while (strcmp(currStock->symbol, finish->symbol))
	{
		if (currStock->symbol[0] != DUMMY_TRAILER)
		{
			printf("%-6s $%.2f\n", currStock->symbol, currStock->quoteStack->price);
		}
		currStock = currStock->back;
	}
	printf("%-6s $%.2f\n", currStock->symbol, currStock->quoteStack->price);

	return;
}

STOCK * findStock(STOCK *list, char *symbol)
{
	STOCK *foundStock = NULL;
	list = list->forw; // skip the dummy node
	while (strcmp(list->symbol, symbol) && list->symbol[0] != DUMMY_TRAILER) list = list->forw;
	if (list->symbol[0] == DUMMY_TRAILER) return foundStock;
	return list;
}

void searchManager(STOCK *list)
{
	char stockSymbol[100] = { 0 };
	STOCK *searchStock;
	int numQuotes;

	while (strcmp(stockSymbol, "QUIT"))
	{
		printf("Enter the stock symbol to search for or enter 'QUIT' to stop: ");
		fgets(stockSymbol, 99, stdin);
		*(stockSymbol + strlen(stockSymbol) - 1) = '\0';
		if (!strcmp(stockSymbol, "QUIT")) break;

		if (searchStock = findStock(list, stockSymbol))
		{
			printf("Enter the number of quotes to print: ");
			scanf("%d", &numQuotes);							// FIX THIS
			if (numQuotes <= searchStock->numQuotes)
				printQuotes(searchStock->quoteStack, numQuotes);
		}
		else printf("Stock %s couldn't be found.\n", stockSymbol);
	}
}