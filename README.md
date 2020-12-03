# CS738_Project

Build the project using standard LLVM build commands.  

To run a test case, follow these steps:  

Change CWD to the build/bin directory.  

Run: ./clang -S -emit-llvm -fno-discard-value-names -Xclang -disable-O0-optnone test.c -o test.ll  

Copy the output to a file, say data.txt. The file must have an empty line at the end. Then run the command below:  

python3 ../../format_data.py test.c data.txt > Metadata.csv  

Then run the commands:  

./opt -instnamer test.ll -S > test2.ll  

mv test2.ll test.ll  

./opt -instnamer test.ll -o test.bc  

Finally run the pass using the command:  

./opt -load /home/nikhil/CS738/CS738_Project/build/lib/LLVMSensitive.so -sensitive_analysis < test.bc  

If you wish to enable branch check, pass the branch option as follows:  

./opt -load /home/nikhil/CS738/CS738_Project/build/lib/LLVMSensitive.so -sensitive_analysis -branch < test.bc   

 

