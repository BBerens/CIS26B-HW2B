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



void getFilename(char *filename, int length, char *type);
STOCK *init_list(void);
int readStocksFromFile(char *filename, STOCK *stockList);
void traverse_forw(STOCK *list);
void traverse_back(STOCK *list);
QUOTE *push(QUOTE *stack, double price);
QUOTE *pop(QUOTE **stack);
void printQuotes(QUOTE *stack);

int main( void )
{
	
	char filename[100];
	STOCK *stockList;
	int isInvalid = 1;

	stockList = init_list();

	while (isInvalid)
	{
		getFilename(filename, sizeof(filename), "in");
		isInvalid = readStocksFromFile(filename, stockList);
	}

	printf("Ascending:\n");
	traverse_forw(stockList);

	//printf("Descending:\n");
	//traverse_back(stockList);


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
		printQuotes(list->quoteStack);
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

int readStocksFromFile(char *filename, STOCK *stockList)
{
	FILE *fpInput;
	double tempPrice;
	char tempsymbol[20];

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
Post	filename now contains either the user's input or one default strings "in.txt" or "out.txt"
*/
void getFilename(char *filename, int length, char *inOrOut)
{
	char defaultName[10];
	strcpy(defaultName, inOrOut);
	strcat(defaultName, ".txt");
	printf("Enter the name of the file to be used as %sput. Alternatively press 'enter' to use the default file.\n", inOrOut);
	fgets(filename, length, stdin);
	*(filename + strlen(filename) - 1) = '\0';
	if (!strcmp(filename, ""))
	{
		strcpy(filename, defaultName);
		printf("Using %s for %sput.\n", defaultName, inOrOut);
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
void printQuotes(QUOTE *stack)
{
	while (stack)
	{
		printf("       $%.2f\n", stack->price);
		stack = stack->next;
	}
}