; ModuleID = '/home/swapnil7/test/testcase6/test.ll'
source_filename = "/home/swapnil7/test/testcase6/testcase6.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@global_var = dso_local global i32 0, align 4

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @fetch_or_zero(i32* %ptr, i32 %a, i32 %b) #0 {
entry:
  %retval = alloca i32, align 4
  %ptr.addr = alloca i32*, align 8
  %a.addr = alloca i32, align 4
  %b.addr = alloca i32, align 4
  %tmp_copy = alloca i32*, align 8
  %a_num = alloca i32, align 4
  store i32* %ptr, i32** %ptr.addr, align 8
  store i32 %a, i32* %a.addr, align 4
  store i32 %b, i32* %b.addr, align 4
  %tmp = load i32, i32* @global_var, align 4
  store i32 %tmp, i32* %b.addr, align 4
  %tmp1 = load i32*, i32** %ptr.addr, align 8
  store i32* %tmp1, i32** %tmp_copy, align 8
  %tmp2 = load i32*, i32** %ptr.addr, align 8
  %cmp = icmp eq i32* %tmp2, null
  br i1 %cmp, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  store i32 0, i32* %retval, align 4
  br label %return

if.else:                                          ; preds = %entry
  %tmp3 = load i32*, i32** %tmp_copy, align 8
  %tmp4 = load i32, i32* %tmp3, align 4
  store i32 %tmp4, i32* %a_num, align 4
  %tmp5 = load i32, i32* %a_num, align 4
  %tmp6 = load i32, i32* @global_var, align 4
  %add = add nsw i32 %tmp5, %tmp6
  store i32 %add, i32* %retval, align 4
  br label %return

return:                                           ; preds = %if.else, %if.then
  %tmp7 = load i32, i32* %retval, align 4
  ret i32 %tmp7
}

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @call_function(i32* %a, i32* %b, i32* %c) #0 {
entry:
  %a.addr = alloca i32*, align 8
  %b.addr = alloca i32*, align 8
  %c.addr = alloca i32*, align 8
  %r1 = alloca i32, align 4
  %r2 = alloca i32, align 4
  %r3 = alloca i32, align 4
  store i32* %a, i32** %a.addr, align 8
  store i32* %b, i32** %b.addr, align 8
  store i32* %c, i32** %c.addr, align 8
  %tmp = load i32*, i32** %a.addr, align 8
  %call = call i32 @fetch_or_zero(i32* %tmp, i32 0, i32 0)
  store i32 %call, i32* %r1, align 4
  %tmp1 = load i32*, i32** %b.addr, align 8
  %call1 = call i32 @fetch_or_zero(i32* %tmp1, i32 0, i32 0)
  store i32 %call1, i32* %r2, align 4
  %tmp2 = load i32*, i32** %c.addr, align 8
  %call2 = call i32 @fetch_or_zero(i32* %tmp2, i32 0, i32 0)
  store i32 %call2, i32* %r3, align 4
  %tmp3 = load i32, i32* %r1, align 4
  %tmp4 = load i32, i32* %r2, align 4
  %add = add nsw i32 %tmp3, %tmp4
  %tmp5 = load i32, i32* %r3, align 4
  %add3 = add nsw i32 %add, %tmp5
  ret i32 %add3
}

attributes #0 = { noinline nounwind uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 10.0.1 (https://github.com/Swapnil09/CS738_Project 9a39fff9da39962dfb68b2fb310bd11f58fe3b84)"}
