; ModuleID = 'FirstAssignmentCompilatori23-24/LoopFusion-Assignment/lfNORMAL.bc'
source_filename = "LoopGuarded.c"
target datalayout = "e-m:o-i64:64-i128:128-n32:64-S128"
target triple = "arm64-apple-macosx14.0.0"

; Function Attrs: noinline nounwind ssp uwtable(sync)
define void @calcoli(ptr noundef %0, ptr noundef %1, ptr noundef %2, ptr noundef %3, i32 noundef %4) #0 !dbg !9 {
  %6 = icmp sgt i32 %4, 0, !dbg !12
  br i1 %6, label %7, label %23, !dbg !13

7:                                                ; preds = %5
  br label %8, !dbg !14

8:                                                ; preds = %20, %7
  %.0 = phi i32 [ 0, %7 ], [ %19, %20 ], !dbg !15
  %9 = sext i32 %.0 to i64, !dbg !16
  %10 = getelementptr inbounds i32, ptr %1, i64 %9, !dbg !16
  %11 = load i32, ptr %10, align 4, !dbg !16
  %12 = sdiv i32 1, %11, !dbg !17
  %13 = sext i32 %.0 to i64, !dbg !18
  %14 = getelementptr inbounds i32, ptr %2, i64 %13, !dbg !18
  %15 = load i32, ptr %14, align 4, !dbg !18
  %16 = mul nsw i32 %12, %15, !dbg !19
  %17 = sext i32 %.0 to i64, !dbg !20
  %18 = getelementptr inbounds i32, ptr %0, i64 %17, !dbg !20
  store i32 %16, ptr %18, align 4, !dbg !21
  %19 = add nsw i32 %.0, 1, !dbg !22
  br label %20, !dbg !23

20:                                               ; preds = %8
  %21 = icmp slt i32 %19, %4, !dbg !24
  br i1 %21, label %8, label %22, !dbg !23, !llvm.loop !25

22:                                               ; preds = %20
  br label %23, !dbg !28

23:                                               ; preds = %22, %5
  %24 = icmp sgt i32 %4, 0, !dbg !29
  br i1 %24, label %25, label %40, !dbg !30

25:                                               ; preds = %23
  br label %26, !dbg !31

26:                                               ; preds = %37, %25
  %.1 = phi i32 [ 0, %25 ], [ %36, %37 ], !dbg !15
  %27 = sext i32 %.1 to i64, !dbg !32
  %28 = getelementptr inbounds i32, ptr %0, i64 %27, !dbg !32
  %29 = load i32, ptr %28, align 4, !dbg !32
  %30 = sext i32 %.1 to i64, !dbg !33
  %31 = getelementptr inbounds i32, ptr %2, i64 %30, !dbg !33
  %32 = load i32, ptr %31, align 4, !dbg !33
  %33 = add nsw i32 %29, %32, !dbg !34
  %34 = sext i32 %.1 to i64, !dbg !35
  %35 = getelementptr inbounds i32, ptr %3, i64 %34, !dbg !35
  store i32 %33, ptr %35, align 4, !dbg !36
  %36 = add nsw i32 %.1, 1, !dbg !37
  br label %37, !dbg !38

37:                                               ; preds = %26
  %38 = icmp slt i32 %36, %4, !dbg !39
  br i1 %38, label %26, label %39, !dbg !38, !llvm.loop !40

39:                                               ; preds = %37
  br label %40, !dbg !42

40:                                               ; preds = %39, %23
  ret void, !dbg !43
}

attributes #0 = { noinline nounwind ssp uwtable(sync) "frame-pointer"="non-leaf" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="apple-m1" "target-features"="+aes,+crc,+dotprod,+fp-armv8,+fp16fml,+fullfp16,+lse,+neon,+ras,+rcpc,+rdm,+sha2,+sha3,+v8.1a,+v8.2a,+v8.3a,+v8.4a,+v8.5a,+v8a,+zcm,+zcz" }

!llvm.module.flags = !{!0, !1, !2, !3, !4, !5}
!llvm.dbg.cu = !{!6}
!llvm.ident = !{!8}

!0 = !{i32 2, !"SDK Version", [2 x i32] [i32 14, i32 5]}
!1 = !{i32 2, !"Debug Info Version", i32 3}
!2 = !{i32 1, !"wchar_size", i32 4}
!3 = !{i32 8, !"PIC Level", i32 2}
!4 = !{i32 7, !"uwtable", i32 1}
!5 = !{i32 7, !"frame-pointer", i32 1}
!6 = distinct !DICompileUnit(language: DW_LANG_C11, file: !7, producer: "clang version 17.0.6 (https://github.com/simomux/LLVM_17.git d8596a1d82a39536b8c1a2a9b456cd5a9d9cfa09)", isOptimized: false, runtimeVersion: 0, emissionKind: NoDebug, splitDebugInlining: false, nameTableKind: Apple, sysroot: "/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX14.5.sdk", sdk: "MacOSX14.5.sdk")
!7 = !DIFile(filename: "LoopGuarded.c", directory: "/Users/simone/LLVM_17/TEST/Assignment4")
!8 = !{!"clang version 17.0.6 (https://github.com/simomux/LLVM_17.git d8596a1d82a39536b8c1a2a9b456cd5a9d9cfa09)"}
!9 = distinct !DISubprogram(name: "calcoli", scope: !7, file: !7, line: 1, type: !10, scopeLine: 1, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !6)
!10 = !DISubroutineType(types: !11)
!11 = !{}
!12 = !DILocation(line: 4, column: 11, scope: !9)
!13 = !DILocation(line: 4, column: 9, scope: !9)
!14 = !DILocation(line: 5, column: 9, scope: !9)
!15 = !DILocation(line: 0, scope: !9)
!16 = !DILocation(line: 6, column: 22, scope: !9)
!17 = !DILocation(line: 6, column: 21, scope: !9)
!18 = !DILocation(line: 6, column: 27, scope: !9)
!19 = !DILocation(line: 6, column: 26, scope: !9)
!20 = !DILocation(line: 6, column: 13, scope: !9)
!21 = !DILocation(line: 6, column: 18, scope: !9)
!22 = !DILocation(line: 7, column: 15, scope: !9)
!23 = !DILocation(line: 8, column: 9, scope: !9)
!24 = !DILocation(line: 8, column: 20, scope: !9)
!25 = distinct !{!25, !14, !26, !27}
!26 = !DILocation(line: 8, column: 23, scope: !9)
!27 = !{!"llvm.loop.mustprogress"}
!28 = !DILocation(line: 9, column: 5, scope: !9)
!29 = !DILocation(line: 11, column: 11, scope: !9)
!30 = !DILocation(line: 11, column: 9, scope: !9)
!31 = !DILocation(line: 12, column: 9, scope: !9)
!32 = !DILocation(line: 13, column: 20, scope: !9)
!33 = !DILocation(line: 13, column: 25, scope: !9)
!34 = !DILocation(line: 13, column: 24, scope: !9)
!35 = !DILocation(line: 13, column: 13, scope: !9)
!36 = !DILocation(line: 13, column: 18, scope: !9)
!37 = !DILocation(line: 14, column: 15, scope: !9)
!38 = !DILocation(line: 15, column: 9, scope: !9)
!39 = !DILocation(line: 15, column: 20, scope: !9)
!40 = distinct !{!40, !31, !41, !27}
!41 = !DILocation(line: 15, column: 23, scope: !9)
!42 = !DILocation(line: 16, column: 5, scope: !9)
!43 = !DILocation(line: 17, column: 1, scope: !9)
