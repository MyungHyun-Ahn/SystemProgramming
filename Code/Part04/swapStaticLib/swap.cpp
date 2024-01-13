#include "pch.h"
#include "swap.h"

void swap(int *v1, int *v2)
{
	int temp = *v1;
	*v1 = *v2;
	*v2 = temp;
}