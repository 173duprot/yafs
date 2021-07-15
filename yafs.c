/* yafs - yet another forth shell */

/* License CC0: 

 To the greatest extent permitted by, but not in contravention
 of, applicable law, Affirmer hereby overtly, fully, permanently,
 irrevocably and unconditionally waives, abandons, and surrenders all of
 Affirmer's Copyright and Related Rights (patents, trademark etc.)
 and associated claims and causes of action, whether now known or unknown
 (including existing as well as future claims and causes of action),
 in the Work (i) in all territories worldwide, (ii) for the maximum
 duration provided by applicable law or treaty (including future time
 extensions), (iii) in any current or future medium and for any number
 of copies, and (iv) for any purpose whatsoever, including without
 limitation commercial, advertising or promotional purposes.

******************************************************************************/

#include <stdlib.h> /* Needed for exit(); */
#include <stdio.h> /* Needed for putchar */
#include <string.h> /* Needed for strlen */

#define MEM_SIZE 10000
int mem[MEM_SIZE];

/* Memory map
|----- TOP -----|_mem[MEM_SIZE]
|		|
|    Stack	|
|_______________|_mem[STACK]
|		|
| Return Stack	|
|_______________|_mem[RSTACK]
|		|
|  Dictionary	|	// This is array of pointers to all the dictionary entries
|  Address	|
|  Stack	|
|_______________|_mem[DICT]
|		|
|		|
|		|
|  Dictionary	|
|  Memory	|
|		|
|		|
|_______________|_mem[DICT_MEM]
|		|
|   Pointers	|
|		|
|--- BOTTOM ----|_mem[PTR]
*/

#define STACK_SIZE 100
#define RETURN_STACK_SIZE 100
#define DICTIONARY_SIZE 1000
#define NUMBER_OF_POINTERS 5

/* locations */

int const STACK =	MEM_SIZE - STACK_SIZE;
int const RSTACK =	MEM_SIZE - STACK_SIZE - RETURN_STACK_SIZE;
int const DSTACK = 	MEM_SIZE - STACK_SIZE - RETURN_STACK_SIZE - DICTIONARY_SIZE;
int const DICT_MEM =	NUMBER_OF_POINTERS;
int const PTR =		0;

/* pointers */

#define stack_ptr	mem[0]	// Points to the top of the stack
#define rstack_ptr	mem[1]	// Points to the top of the return stack
#define dstack_ptr	mem[2]	// Points to the top of the dictionary address stack
#define dict_mem_ptr	mem[3]	// Points to the next free space in dictionary memory
#define pc		mem[4]	// Program counter - points to the next place in memory being interprited
/* 	...		...
	bruh_ptr	mem[NUMBER_OF_POINTERS] */

void ptr_init(void)
{
	stack_ptr = STACK;	
	rstack_ptr = RSTACK;	
	dstack_ptr = DSTACK;	
	dict_mem_ptr = DICT_MEM;
	pc = STACK; /* doenst init to anything */
}

/* __ first functions __
*
* stack
* 	iterate_stack_ptr()		__memory safe removal from stack
* 	degradate_stack_ptr()		__memory safe adding to stack
*
* rstack
* 	iterate_rstack_ptr()		__memory safe removal from rstack
* 	degradate_rstack_ptr()		__memory safe adding to rstack
* 	
* dict	
* 	iterate_dict_stack_ptr()	__memory safe adding to dictionary address stack
*	iterate_dict_mem_ptr(x)		__memory safe adding to dictionary memory
*	dict_mem_write(data, addr)	__memory safe write to dictionary memory space
*
* pc
*	iterate_pc(x)
*	pc_write(arrd)
*/ 	


/* stack */

void iterate_stack_ptr (void)
{
	if(stack_ptr < MEM_SIZE)
	{
		stack_ptr = stack_ptr + 1;
	}
	else
	{
		putchar('F');	/* Rip */
		putchar('\n');
	}
}

void degradate_stack_ptr (void)
{
	if(STACK < stack_ptr)
	{
		stack_ptr = stack_ptr - 1;
	}
	else
	{
		putchar('F');	/* Rip */
		putchar('\n');
	}
}


/* rstack */

void iterate_rstack_ptr (void)
{
	if(rstack_ptr < STACK)
	{
		stack_ptr = stack_ptr + 1;
	}
	else
	{
		putchar('F');	/* Rip */
		putchar('\n');
	}
}

void degradate_rstack_ptr (void)
{
	if(RSTACK < rstack_ptr)
	{
		stack_ptr = stack_ptr - 1;
	}
	else
	{
		putchar('F');	/* Rip */
		putchar('\n');
	}
}


/* dict */

void iterate_dstack_ptr (void)
{
	if(dstack_ptr < RSTACK)
	{
		dstack_ptr = stack_ptr - 1;
	}
	else
	{
		putchar('F');	/* Rip */
		putchar('\n');
	}
}

void iterate_dict_mem_ptr (int amount)
{
	if(dict_mem_ptr < DSTACK)
	{
		dstack_ptr = dstack_ptr + amount;
	}
	else
	{
		putchar('F');	/* Rip */
		putchar('\n');
	}
}

void dict_mem_write (int data)
{
	if(DICT_MEM < dict_mem_ptr && dict_mem_ptr < DSTACK)
	{
		mem[dict_mem_ptr] = data;
	}
	else
	{
		putchar('F');	/* Rip */
		putchar('\n');

		exit(1);	/* if the dict memory overflows, just end the program */
	}
}

/* pc */
void iterate_pc (int amount)
{
	mem[pc] = mem[pc] + amount;
}

/* __ base functions __
*
* stack
* 	push(x)		__pushes int x onto stack
* 	pop()		__pops x off stack and returns it
*
* rstack	
* 	rpush(x)	__pushes addr x onto stack
* 	rpop()		__pops addr x off stack and returns it
*
* dict		__Read "Dictionary structure" comment to understand this bit
*	dpush(x)	__pushes an address onto the dictionary address stack
*/ 	


/* stack */

void push(int val)
{
	iterate_stack_ptr();
	mem[stack_ptr] = val;
}

int pop(void)
{
	int val = mem[stack_ptr];
	degradate_stack_ptr();
	return val;
}


/* rstack */

void rpush(int val)
{
	iterate_rstack_ptr();
	mem[rstack_ptr] = val;
}

int rpop(void)
{
	int val = mem[rstack_ptr];
	degradate_rstack_ptr();
	return val;
}


/* dict */

void dpush(int val)
{
	iterate_dstack_ptr();
	mem[dstack_ptr] = val;
}

/* Dictionary structure
* 
* word			string (null-terminated)
* immediate flag	int
* hidden flag		int
* function pointer  <----------------------------(* for interprited words, this
* data		       	string (null-terminated) (* calls the interiter 
* 						 (* for builtins, this calls a
*						 (* function directly
*/

/* __ advanced functions __
*
* dict
*	dict_init(word)	__puts a word in the dictionary and adds its addr to the dict stack
*	immediate(0|1)	__puts flag on stack
*		void (*f)(void *), 
*		char* data
*	      )
*
* char* word,		/* string, null terminated already
* int immediate,
* int hidden,
* void (*fp)(void *),
*/

/* This is for reading and writing the function pointer into mem[] */
#define	FP_READ(fp, addr)	memcpy(&fp, &mem[addr], sizeof(fp))
#define	FP_WRITE(fp, addr)	memcpy(&mem[addr], &fp, sizeof(fp))
#define FP_SIZE			sizeof(void(*)())/sizeof(int)

void dict_init(char* word)
{
	/* push the current dict_mem_ptr location onto the dictionary stack */
	dpush(dict_mem_ptr);


	/* NOTE: dict_mem_write() writes to the dict_mem_ptr location */


	/* write the word */
	int word_size = strlen(word); /* i could have this pointer shit wrong */
	for(int i = 0; i < word_size + 1; i++) 	/* word_size + 1 because we want the */
	{					/* null (\0) character written too */
		dict_mem_write(word[i]);
		iterate_dict_mem_ptr(1);
	}

}

void dict_immediate(int immediate)
{
	/* write the immediate flag */
	dict_mem_write(immediate);
	iterate_dict_mem_ptr(1);
}

/*void dict_hidden(int immediate)
{
	/* write the hidden flag *
	dict_mem_write(hidden);
	iterate_dict_mem_ptr(1);
}*/


void dict_link( void (*fp)(void *))
{	
	/* write the function pointer */
	FP_WRITE(fp, dict_mem_ptr);
	iterate_dict_mem_ptr(FP_SIZE);
}

void dict_close()
{
	/*  */
	
}

/* __ Builtins__
* 
* compiling
*    :	define	__read in the next space-delimited word, add it to
*			the end of our string storage, and generate
*			a header for the new word so that when it
*			is typed it compiles a pointer to itself
*			so that it can be executed.		
* 
* stack
*	drop	(a -- ) 	__deletes the top of the stack
8	dup	(a -- a a)	__duplicates the top stack item
*	swap	(a b -- b a)	__reverses the top two stack items
*	over	(a b -- a b a)	__copies second item to top
*	rot	(a b c -- b c a)__rotates third item to top
* 	
* math
*    +	sum	(a b -- sum)
*    -	diff	(a b -- diff)
*    *	mult	(a b -- mult)
*    /	div	(a b -- div)
*	mod	(a b -- reminder)
*    <	lt	(a b -- lt?)
*    >	gt	(a b -- gt?)
*    =  eq	(a b -- eq?)
*
* ascii 		
* 	dot	(a -- )		__prints whats on the top of the stack with putchar()
* 				
* 		
* 		
* 		
 */

/* stack */

void drop(void)
{
	degradate_stack_ptr();
}
/* pointer */
void (*drop_ptr)(void) = &drop;


void dot(void)
{
	putchar(pop());
}
/* pointer */
void (*dot_ptr)(void) = &dot;



/* Forth */

void builtin_init(void)
{
	
}

void interpriter(void)
{
	
}

int main(void)
{
	ptr_init();
	return 0;
}
