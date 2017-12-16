

/* This is auto-generated code. Edit at your own peril. */
#include <stdio.h>
#include <stdlib.h>

#include "CuTest.h"


extern TEST_txtest(CuTest*);


int txtest_main(void)
{
    CuString *output = CuStringNew();
    CuSuite* suite = CuSuiteNew();
    int fails;


    SUITE_ADD_TEST(suite, TEST_txtest);

    CuSuiteRun(suite);
    CuSuiteSummary(suite, output);
    CuSuiteDetails(suite, output);
    printf("%s\n", output->buffer);
    CuStringDelete(output);
    fails = suite->failCount;
    CuSuiteDelete(suite);
    return (fails);
}

