; ModuleID = 'error1.ll'
source_filename = "error1.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @bar(i32 %a, i32* %b) #0 {
entry:
  %retval = alloca i32, align 4
  %a.addr = alloca i32, align 4
  %b.addr = alloca i32*, align 8
  store i32 %a, i32* %a.addr, align 4
  store i32* %b, i32** %b.addr, align 8
  %tmp = load i32*, i32** %b.addr, align 8
  %tmp1 = load i32, i32* %tmp, align 4
  %tmp2 = load i32, i32* %a.addr, align 4
  %cmp = icmp eq i32 %tmp1, %tmp2
  br i1 %cmp, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %tmp3 = load i32, i32* %a.addr, align 4
  store i32 %tmp3, i32* %retval, align 4
  br label %return

if.else:                                          ; preds = %entry
  %tmp4 = load i32*, i32** %b.addr, align 8
  %tmp5 = load i32, i32* %tmp4, align 4
  %add = add nsw i32 %tmp5, 3
  store i32 %add, i32* %a.addr, align 4
  store i32 %add, i32* %retval, align 4
  br label %return

return:                                           ; preds = %if.else, %if.then
  %tmp6 = load i32, i32* %retval, align 4
  ret i32 %tmp6
}

attributes #0 = { noinline nounwind uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 10.0.1 "}
