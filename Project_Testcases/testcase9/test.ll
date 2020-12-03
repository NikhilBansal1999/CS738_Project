; ModuleID = '/home/swapnil7/test/testcase9/test.ll'
source_filename = "/home/swapnil7/test/testcase9/testcase9.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@s1 = dso_local global [100 x i8] c"Welcome to \00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00", align 16
@__const.function_call.s2 = private unnamed_addr constant [6 x i8] c"cs738\00", align 1

; Function Attrs: noinline nounwind uwtable
define dso_local i8* @function_call() #0 {
entry:
  %s2 = alloca [6 x i8], align 1
  %length = alloca i32, align 4
  %i = alloca i32, align 4
  %tmp = bitcast [6 x i8]* %s2 to i8*
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* align 1 %tmp, i8* align 1 getelementptr inbounds ([6 x i8], [6 x i8]* @__const.function_call.s2, i32 0, i32 0), i64 6, i1 false)
  store i32 0, i32* %length, align 4
  br label %while.cond

while.cond:                                       ; preds = %while.body, %entry
  %tmp1 = load i32, i32* %length, align 4
  %idxprom = sext i32 %tmp1 to i64
  %arrayidx = getelementptr inbounds [100 x i8], [100 x i8]* @s1, i64 0, i64 %idxprom
  %tmp2 = load i8, i8* %arrayidx, align 1
  %conv = sext i8 %tmp2 to i32
  %cmp = icmp ne i32 %conv, 0
  br i1 %cmp, label %while.body, label %while.end

while.body:                                       ; preds = %while.cond
  %tmp3 = load i32, i32* %length, align 4
  %inc = add nsw i32 %tmp3, 1
  store i32 %inc, i32* %length, align 4
  br label %while.cond

while.end:                                        ; preds = %while.cond
  store i32 0, i32* %i, align 4
  br label %for.cond

for.cond:                                         ; preds = %for.inc, %while.end
  %tmp4 = load i32, i32* %i, align 4
  %idxprom2 = sext i32 %tmp4 to i64
  %arrayidx3 = getelementptr inbounds [6 x i8], [6 x i8]* %s2, i64 0, i64 %idxprom2
  %tmp5 = load i8, i8* %arrayidx3, align 1
  %conv4 = sext i8 %tmp5 to i32
  %cmp5 = icmp ne i32 %conv4, 0
  br i1 %cmp5, label %for.body, label %for.end

for.body:                                         ; preds = %for.cond
  %tmp6 = load i32, i32* %i, align 4
  %idxprom7 = sext i32 %tmp6 to i64
  %arrayidx8 = getelementptr inbounds [6 x i8], [6 x i8]* %s2, i64 0, i64 %idxprom7
  %tmp7 = load i8, i8* %arrayidx8, align 1
  %tmp8 = load i32, i32* %length, align 4
  %idxprom9 = sext i32 %tmp8 to i64
  %arrayidx10 = getelementptr inbounds [100 x i8], [100 x i8]* @s1, i64 0, i64 %idxprom9
  store i8 %tmp7, i8* %arrayidx10, align 1
  br label %for.inc

for.inc:                                          ; preds = %for.body
  %tmp9 = load i32, i32* %i, align 4
  %inc11 = add nsw i32 %tmp9, 1
  store i32 %inc11, i32* %i, align 4
  %tmp10 = load i32, i32* %length, align 4
  %inc12 = add nsw i32 %tmp10, 1
  store i32 %inc12, i32* %length, align 4
  br label %for.cond

for.end:                                          ; preds = %for.cond
  %tmp11 = load i32, i32* %length, align 4
  %idxprom13 = sext i32 %tmp11 to i64
  %arrayidx14 = getelementptr inbounds [100 x i8], [100 x i8]* @s1, i64 0, i64 %idxprom13
  store i8 0, i8* %arrayidx14, align 1
  ret i8* getelementptr inbounds ([100 x i8], [100 x i8]* @s1, i64 0, i64 0)
}

; Function Attrs: argmemonly nounwind willreturn
declare void @llvm.memcpy.p0i8.p0i8.i64(i8* noalias nocapture writeonly, i8* noalias nocapture readonly, i64, i1 immarg) #1

; Function Attrs: noinline nounwind uwtable
define dso_local void @myfunction() #0 {
entry:
  %a = alloca i8*, align 8
  %call = call i8* @function_call()
  store i8* %call, i8** %a, align 8
  ret void
}

attributes #0 = { noinline nounwind uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { argmemonly nounwind willreturn }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 10.0.1 (https://github.com/Swapnil09/CS738_Project 9a39fff9da39962dfb68b2fb310bd11f58fe3b84)"}
