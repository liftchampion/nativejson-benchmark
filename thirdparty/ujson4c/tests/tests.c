/*
ujson4c decoder helper 1.0
Developed by ESN, an Electronic Arts Inc. studio. 
Copyright (c) 2013, Electronic Arts Inc.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.
* Neither the name of ESN, Electronic Arts Inc. nor the
names of its contributors may be used to endorse or promote products
derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL ELECTRONIC ARTS INC. BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

Uses UltraJSON library:
Copyright (c) 2013, Electronic Arts Inc.
All rights reserved.
www.github.com/esnme/ultrajson
*/

#include "ujdecode.h"
#include <malloc.h>
#include <assert.h>
#include <limits.h>

void test_unpackKeys()
{
	UJObject obj;
	void *state;
	char buffer[32768];
	const char input[] = "{\"name\": \"John Doe\", \"age\": 31, \"number\": 1337.37, \"address\": { \"city\": \"Uppsala\", \"population\": 9223372036854775807 } }";
	size_t cbInput = sizeof(input) - 1;

	const wchar_t *personKeys[] = { L"name", L"age", L"number", L"address"};
	UJObject oName, oAge, oNumber, oAddress;

	UJHeapFuncs hf;
	hf.cbInitialHeap = sizeof(buffer);
	hf.initalHeap = buffer;
	hf.free = free;
	hf.malloc = malloc;
	hf.realloc = realloc;

	obj = UJDecode(input, cbInput, NULL, &state);

	if (UJObjectUnpack(obj, 4, "SNNO", personKeys, &oName, &oAge, &oNumber, &oAddress) == 4)
	{
		const wchar_t *addressKeys[] = { L"city", L"population" };
		UJObject oCity, oPopulation;

		const wchar_t *name = UJReadString(oName, NULL);
		int age = UJNumericInt(oAge);
		double number = UJNumericFloat(oNumber);

		assert(wcscmp(name, L"John Doe") == 0);
		assert(age == 31);
		assert(number == 1337.37);

		if (UJObjectUnpack(oAddress, 2, "SN", addressKeys, &oCity, &oPopulation) == 2)
		{
			const wchar_t *city;
			long long population;
			city = UJReadString(oCity, NULL);
			assert(wcscmp(city, L"Uppsala") == 0);
			population = UJNumericLongLong(oPopulation);
			assert(population == LLONG_MAX);
		}
		else
		{
			assert(0);
		}
	}
	else
	{
		assert(0);
	}


	UJFree(state);
}

#ifndef __BENCHMARK__
int main ()
{
	test_unpackKeys();
	return 0;
}
#endif