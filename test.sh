chmod +x compile.sh
./compile.sh
echo "\n========== test invalid input ==========\n"
./ceval nonexist.c
./ceval -nonexist 
echo "\n========== test -help ==========\n"
./ceval -help

echo "\n========== Code indentation==========\n"
echo "\n---------- expected output: pass test_indentation ----------\n"
./ceval -novars -ssf -nodoc -notest testcases/indentation/test1.c
echo "\n---------- expected output: fail Rule1 & Rule2 ----------\n"
./ceval -novars -ssf -nodoc -notest testcases/indentation/test2.c
echo "\n---------- expected output: fail Rule1  ----------\n"
./ceval -novars -ssf -nodoc -notest testcases/indentation/test3.c
echo "\n---------- expected output: fail Rule4  ----------\n"
./ceval -novars -ssf -nodoc -notest testcases/indentation/test4.c


echo "\n========== Variable ==========\n"
echo "\n---------- expected output: pass rate_variable  ----------\n"
./ceval -ssf -nodoc -notest testcases/variable/test2.c
echo "\n---------- expected output: fail Rule1  ----------\n"
./ceval -ssf -nodoc -notest testcases/variable/test1.c 
echo "\n---------- expected output: faile Rule2 & Rule3  ----------\n"
./ceval -ssf -nodoc -notest testcases/variable/test3.c


echo "\n========== Documentation ==========\n"
echo "\n---------- expected output: pass rate_documentation ---------- \n"
./ceval -ssf -novars -notest testcases/documentation/test1.c
./ceval -ssf -novars -notest testcases/documentation/test2.c

echo "\n========== Built-in test cases ==========\n"
echo "\n---------- expected output: missing testTEST1(), testTEST1() is not called, testTEST3() is not called  ----------\n"
./ceval -nodoc -ssf -novars testcases/testcases/main.c testcases/testcases/test1.c testcases/testcases/test2.c testcases/testcases/test3.c
echo "\n---------- expected output: main() missing ----------\n" 
./ceval -nodoc -ssf -novars testcases/testcases/test2.c
echo "\n---------- expected output: more than one main() ----------\n" 
./ceval -nodoc -ssf -novars testcases/testcases/main.c testcases/testcases/main2.c 



