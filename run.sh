./clang -S -emit-llvm -fno-discard-value-names -Xclang -disable-O0-optnone test/test.c -o test/test.ll
# ./clang -emit-llvm -fno-discard-value-names -Xclang -disable-O0-optnone test/test.c -c -o test/test.bc
./opt -instnamer test/test.ll -S > test/test2.ll
mv test/test2.ll test/test.ll
./opt -instnamer test/test.ll -o test/test.bc
# ./clang -S -emit-llvm -fno-discard-value-names test/test.c -o test/test.ll
# ./opt -mem2reg -instnamer test/test.ll -S > test_temp.ll
# mv test_temp.ll test/test2.ll

# ../../../../build/bin/clang -S -emit-llvm -Xclang -disable-O0-optnone test.c
# ../../../../build/bin/opt -mem2reg -instnamer test.ll -S > test_temp.ll
# ../../../../build/bin/opt -mem2reg -instnamer test.ll -o test.bc
# mv test_temp.ll test.ll
./opt -load /home/nikhil/CS738/CS738_Project/build/lib/LLVMSensitive.so -sensitive_analysis < test/test.bc
# ./opt -load /home/nikhil/CS738/CS738_Project/build/lib/LLVMSensitive.so -sensitive_analysis -branch < test/test.bc