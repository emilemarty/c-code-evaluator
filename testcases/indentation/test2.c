/*

test Rule1: Code must be indented with at least 4 spaces or exactly 1 tab per nest.
expected output: fail test_indentation
Rule 2: The open and close curly brackets must be aligned with the beginning of the
statement. 

*/

int test(){
 int a = 10;
if (1){
 a = 5;
if (1){
a = 9;
}
 return 1;
}
else{
            return 0;
}
}