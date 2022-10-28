/*

test Rule1: Code must be indented with at least 4 spaces or exactly 1 tab per nest.
expected output: fail test_indentation

*/

int test(){int a = 10;if (1){a = 5;if (1){a = 9;}   
                    return 1;}else{return 0;}}