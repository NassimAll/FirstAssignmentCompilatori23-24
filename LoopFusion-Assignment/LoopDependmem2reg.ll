; ModuleID = 'FirstAssignmentCompilatori23-24/LoopFusion-Assignment/lf.bc'
source_filename = "FirstAssignmentCompilatori23-24/LoopFusion-Assignment/LoopDepend.c"
target datalayout = "e-m:o-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx14.0.0"

define void @calcoli(ptr noundef %0, ptr noundef %1, ptr noundef %2, ptr noundef %3, i32 noundef %4) {
  br label %6

6:                                                ; preds = %19, %5
  %.0 = phi i32 [ 0, %5 ], [ %20, %19 ]
  %7 = icmp slt i32 %.0, %4
  br i1 %7, label %8, label %21

8:                                                ; preds = %6
  %9 = sext i32 %.0 to i64
  %10 = getelementptr inbounds i32, ptr %1, i64 %9
  %11 = load i32, ptr %10, align 4
  %12 = sdiv i32 1, %11
  %13 = sext i32 %.0 to i64
  %14 = getelementptr inbounds i32, ptr %2, i64 %13
  %15 = load i32, ptr %14, align 4
  %16 = mul nsw i32 %12, %15
  %17 = sext i32 %.0 to i64
  %18 = getelementptr inbounds i32, ptr %0, i64 %17
  store i32 %16, ptr %18, align 4
  br label %19

19:                                               ; preds = %8
  %20 = add nsw i32 %.0, 1
  br label %6, !llvm.loop !5

21:                                               ; preds = %6
  br label %22

22:                                               ; preds = %35, %21
  %.1 = phi i32 [ 0, %21 ], [ %36, %35 ]
  %23 = icmp slt i32 %.1, %4
  br i1 %23, label %24, label %37

24:                                               ; preds = %22
  %25 = add nsw i32 %.1, 2
  %26 = sext i32 %25 to i64
  %27 = getelementptr inbounds i32, ptr %0, i64 %26
  %28 = load i32, ptr %27, align 4
  %29 = sext i32 %.1 to i64
  %30 = getelementptr inbounds i32, ptr %2, i64 %29
  %31 = load i32, ptr %30, align 4
  %32 = add nsw i32 %28, %31
  %33 = sext i32 %.1 to i64
  %34 = getelementptr inbounds i32, ptr %3, i64 %33
  store i32 %32, ptr %34, align 4
  br label %35

35:                                               ; preds = %24
  %36 = add nsw i32 %.1, 1
  br label %22, !llvm.loop !7

37:                                               ; preds = %22
  ret void
}

!llvm.module.flags = !{!0, !1, !2, !3}
!llvm.ident = !{!4}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"uwtable", i32 2}
!3 = !{i32 7, !"frame-pointer", i32 2}
!4 = !{!"clang version 17.0.6"}
!5 = distinct !{!5, !6}
!6 = !{!"llvm.loop.mustprogress"}
!7 = distinct !{!7, !6}
