; ModuleID = '/home/swapnil7/test/testcase8/test.ll'
source_filename = "/home/swapnil7/test/testcase8/testcase8.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @myfunction() #0 {
entry:
  %x = alloca i32, align 4
  %num = alloca i32, align 4
  store i32 1, i32* %x, align 4
  store i32 10, i32* %num, align 4
  br label %while.cond

while.cond:                                       ; preds = %while.body, %entry
  %tmp = load i32, i32* %x, align 4
  %cmp = icmp slt i32 %tmp, 10
  br i1 %cmp, label %while.body, label %while.end

while.body:                                       ; preds = %while.cond
  %tmp1 = load i32, i32* %x, align 4
  %inc = add nsw i32 %tmp1, 1
  store i32 %inc, i32* %x, align 4
  br label %while.cond

while.end:                                        ; preds = %while.cond
  %tmp2 = load i32, i32* %num, align 4
  ret i32 %tmp2
}

attributes #0 = { noinline nounwind uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 10.0.1 (https://github.com/Swapnil09/CS738_Project 9a39fff9da39962dfb68b2fb310bd11f58fe3b84)"}
