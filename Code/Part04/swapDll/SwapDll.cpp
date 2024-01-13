#include "pch.h"
#include "swapDll.h"

extern "C" __declspec(dllexport);
void swapDll(int *v1, int *v2)
{
	int temp = *v1;
	*v1 = *v2;
	*v2 = temp;
}