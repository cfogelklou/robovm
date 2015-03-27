/*
 ============================================================================
 Name        : main_app_using_library.c
 Author      : Chris Fogelklou
 Version     :
 Copyright   : Copyright 2015 Acorn Technology
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>

extern const char * get_hello_str( void );

int main(void) {
	const char *pHello = get_hello_str();
	puts(pHello); /* prints !!!Hello World!!! */
	return EXIT_SUCCESS;
}
