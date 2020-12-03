; ModuleID = '/home/swapnil7/test/testcase4/test.ll'
source_filename = "/home/swapnil7/test/testcase4/testcase4.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @function_call(i32 %x, i32 %y) #0 {
entry:
  %x.addr = alloca i32, align 4
  %y.addr = alloca i32, align 4
  %ret = alloca i32, align 4
  store i32 %x, i32* %x.addr, align 4
  store i32 %y, i32* %y.addr, align 4
  %tmp = load i32, i32* %x.addr, align 4
  %tmp1 = load i32, i32* %y.addr, align 4
  %add = add nsw i32 %tmp, %tmp1
  store i32 %add, i32* %ret, align 4
  %tmp2 = load i32, i32* %ret, align 4
  ret i32 %tmp2
}

; Function Attrs: noinline nounwind uwtable
define dso_local void @myfunction() #0 {
entry:
  %x = alloca i32, align 4
  %y = alloca i32, align 4
  %z = alloca i32, align 4
  %result = alloca i32, align 4
  %tmp = load i32, i32* %x, align 4
  %tmp1 = load i32, i32* %y, align 4
  %add = add nsw i32 %tmp, %tmp1
  %tmp2 = load i32, i32* %z, align 4
  %add1 = add nsw i32 %add, %tmp2
  store i32 %add1, i32* %result, align 4
  %tmp3 = load i32, i32* %result, align 4
  %tmp4 = load i32, i32* %x, align 4
  %call = call i32 @function_call(i32 %tmp3, i32 %tmp4)
  store i32 %call, i32* %z, align 4
  ret void
}

attributes #0 = { noinline nounwind uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 10.0.1 (https://github.com/Swapnil09/CS738_Project 9a39fff9da39962dfb68b2fb310bd11f58fe3b84)"}
