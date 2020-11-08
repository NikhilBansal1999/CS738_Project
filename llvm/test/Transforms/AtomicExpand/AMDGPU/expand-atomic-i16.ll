; NOTE: Assertions have been autogenerated by utils/update_test_checks.py
; RUN: opt -mtriple=amdgcn-amd-amdhsa -S -atomic-expand %s | FileCheck %s
; RUN: opt -mtriple=r600-mesa-mesa3d -S -atomic-expand %s | FileCheck %s

define i16 @test_atomicrmw_xchg_i16_global(i16 addrspace(1)* %ptr, i16 %value) {
; CHECK-LABEL: @test_atomicrmw_xchg_i16_global(
; CHECK-NEXT:    [[RES:%.*]] = atomicrmw xchg i16 addrspace(1)* [[PTR:%.*]], i16 [[VALUE:%.*]] seq_cst
; CHECK-NEXT:    ret i16 [[RES]]
;
  %res = atomicrmw xchg i16 addrspace(1)* %ptr, i16 %value seq_cst
  ret i16 %res
}

define i16 @test_atomicrmw_add_i16_global(i16 addrspace(1)* %ptr, i16 %value) {
; CHECK-LABEL: @test_atomicrmw_add_i16_global(
; CHECK-NEXT:    [[RES:%.*]] = atomicrmw add i16 addrspace(1)* [[PTR:%.*]], i16 [[VALUE:%.*]] seq_cst
; CHECK-NEXT:    ret i16 [[RES]]
;
  %res = atomicrmw add i16 addrspace(1)* %ptr, i16 %value seq_cst
  ret i16 %res
}

define i16 @test_atomicrmw_sub_i16_global(i16 addrspace(1)* %ptr, i16 %value) {
; CHECK-LABEL: @test_atomicrmw_sub_i16_global(
; CHECK-NEXT:    [[RES:%.*]] = atomicrmw sub i16 addrspace(1)* [[PTR:%.*]], i16 [[VALUE:%.*]] seq_cst
; CHECK-NEXT:    ret i16 [[RES]]
;
  %res = atomicrmw sub i16 addrspace(1)* %ptr, i16 %value seq_cst
  ret i16 %res
}

define i16 @test_atomicrmw_and_i16_global(i16 addrspace(1)* %ptr, i16 %value) {
; CHECK-LABEL: @test_atomicrmw_and_i16_global(
; CHECK-NEXT:    [[TMP1:%.*]] = ptrtoint i16 addrspace(1)* [[PTR:%.*]] to i64
; CHECK-NEXT:    [[TMP2:%.*]] = and i64 [[TMP1]], -4
; CHECK-NEXT:    [[ALIGNEDADDR:%.*]] = inttoptr i64 [[TMP2]] to i32 addrspace(1)*
; CHECK-NEXT:    [[PTRLSB:%.*]] = and i64 [[TMP1]], 3
; CHECK-NEXT:    [[TMP3:%.*]] = shl i64 [[PTRLSB]], 3
; CHECK-NEXT:    [[SHIFTAMT:%.*]] = trunc i64 [[TMP3]] to i32
; CHECK-NEXT:    [[MASK:%.*]] = shl i32 65535, [[SHIFTAMT]]
; CHECK-NEXT:    [[INV_MASK:%.*]] = xor i32 [[MASK]], -1
; CHECK-NEXT:    [[TMP4:%.*]] = zext i16 [[VALUE:%.*]] to i32
; CHECK-NEXT:    [[VALOPERAND_SHIFTED:%.*]] = shl i32 [[TMP4]], [[SHIFTAMT]]
; CHECK-NEXT:    [[ANDOPERAND:%.*]] = or i32 [[INV_MASK]], [[VALOPERAND_SHIFTED]]
; CHECK-NEXT:    [[TMP5:%.*]] = atomicrmw and i32 addrspace(1)* [[ALIGNEDADDR]], i32 [[ANDOPERAND]] seq_cst
; CHECK-NEXT:    [[TMP6:%.*]] = lshr i32 [[TMP5]], [[SHIFTAMT]]
; CHECK-NEXT:    [[TMP7:%.*]] = trunc i32 [[TMP6]] to i16
; CHECK-NEXT:    ret i16 [[TMP7]]
;
  %res = atomicrmw and i16 addrspace(1)* %ptr, i16 %value seq_cst
  ret i16 %res
}

define i16 @test_atomicrmw_nand_i16_global(i16 addrspace(1)* %ptr, i16 %value) {
; CHECK-LABEL: @test_atomicrmw_nand_i16_global(
; CHECK-NEXT:    [[TMP1:%.*]] = ptrtoint i16 addrspace(1)* [[PTR:%.*]] to i64
; CHECK-NEXT:    [[TMP2:%.*]] = and i64 [[TMP1]], -4
; CHECK-NEXT:    [[ALIGNEDADDR:%.*]] = inttoptr i64 [[TMP2]] to i32 addrspace(1)*
; CHECK-NEXT:    [[PTRLSB:%.*]] = and i64 [[TMP1]], 3
; CHECK-NEXT:    [[TMP3:%.*]] = shl i64 [[PTRLSB]], 3
; CHECK-NEXT:    [[SHIFTAMT:%.*]] = trunc i64 [[TMP3]] to i32
; CHECK-NEXT:    [[MASK:%.*]] = shl i32 65535, [[SHIFTAMT]]
; CHECK-NEXT:    [[INV_MASK:%.*]] = xor i32 [[MASK]], -1
; CHECK-NEXT:    [[TMP4:%.*]] = zext i16 [[VALUE:%.*]] to i32
; CHECK-NEXT:    [[VALOPERAND_SHIFTED:%.*]] = shl i32 [[TMP4]], [[SHIFTAMT]]
; CHECK-NEXT:    [[TMP5:%.*]] = load i32, i32 addrspace(1)* [[ALIGNEDADDR]], align 4
; CHECK-NEXT:    br label [[ATOMICRMW_START:%.*]]
; CHECK:       atomicrmw.start:
; CHECK-NEXT:    [[LOADED:%.*]] = phi i32 [ [[TMP5]], [[TMP0:%.*]] ], [ [[NEWLOADED:%.*]], [[ATOMICRMW_START]] ]
; CHECK-NEXT:    [[TMP6:%.*]] = and i32 [[LOADED]], [[VALOPERAND_SHIFTED]]
; CHECK-NEXT:    [[NEW:%.*]] = xor i32 [[TMP6]], -1
; CHECK-NEXT:    [[TMP7:%.*]] = and i32 [[NEW]], [[MASK]]
; CHECK-NEXT:    [[TMP8:%.*]] = and i32 [[LOADED]], [[INV_MASK]]
; CHECK-NEXT:    [[TMP9:%.*]] = or i32 [[TMP8]], [[TMP7]]
; CHECK-NEXT:    [[TMP10:%.*]] = cmpxchg i32 addrspace(1)* [[ALIGNEDADDR]], i32 [[LOADED]], i32 [[TMP9]] seq_cst seq_cst
; CHECK-NEXT:    [[SUCCESS:%.*]] = extractvalue { i32, i1 } [[TMP10]], 1
; CHECK-NEXT:    [[NEWLOADED]] = extractvalue { i32, i1 } [[TMP10]], 0
; CHECK-NEXT:    br i1 [[SUCCESS]], label [[ATOMICRMW_END:%.*]], label [[ATOMICRMW_START]]
; CHECK:       atomicrmw.end:
; CHECK-NEXT:    [[TMP11:%.*]] = lshr i32 [[NEWLOADED]], [[SHIFTAMT]]
; CHECK-NEXT:    [[TMP12:%.*]] = trunc i32 [[TMP11]] to i16
; CHECK-NEXT:    ret i16 [[TMP12]]
;
  %res = atomicrmw nand i16 addrspace(1)* %ptr, i16 %value seq_cst
  ret i16 %res
}

define i16 @test_atomicrmw_or_i16_global(i16 addrspace(1)* %ptr, i16 %value) {
; CHECK-LABEL: @test_atomicrmw_or_i16_global(
; CHECK-NEXT:    [[TMP1:%.*]] = ptrtoint i16 addrspace(1)* [[PTR:%.*]] to i64
; CHECK-NEXT:    [[TMP2:%.*]] = and i64 [[TMP1]], -4
; CHECK-NEXT:    [[ALIGNEDADDR:%.*]] = inttoptr i64 [[TMP2]] to i32 addrspace(1)*
; CHECK-NEXT:    [[PTRLSB:%.*]] = and i64 [[TMP1]], 3
; CHECK-NEXT:    [[TMP3:%.*]] = shl i64 [[PTRLSB]], 3
; CHECK-NEXT:    [[SHIFTAMT:%.*]] = trunc i64 [[TMP3]] to i32
; CHECK-NEXT:    [[MASK:%.*]] = shl i32 65535, [[SHIFTAMT]]
; CHECK-NEXT:    [[INV_MASK:%.*]] = xor i32 [[MASK]], -1
; CHECK-NEXT:    [[TMP4:%.*]] = zext i16 [[VALUE:%.*]] to i32
; CHECK-NEXT:    [[VALOPERAND_SHIFTED:%.*]] = shl i32 [[TMP4]], [[SHIFTAMT]]
; CHECK-NEXT:    [[TMP5:%.*]] = atomicrmw or i32 addrspace(1)* [[ALIGNEDADDR]], i32 [[VALOPERAND_SHIFTED]] seq_cst
; CHECK-NEXT:    [[TMP6:%.*]] = lshr i32 [[TMP5]], [[SHIFTAMT]]
; CHECK-NEXT:    [[TMP7:%.*]] = trunc i32 [[TMP6]] to i16
; CHECK-NEXT:    ret i16 [[TMP7]]
;
  %res = atomicrmw or i16 addrspace(1)* %ptr, i16 %value seq_cst
  ret i16 %res
}

define i16 @test_atomicrmw_xor_i16_global(i16 addrspace(1)* %ptr, i16 %value) {
; CHECK-LABEL: @test_atomicrmw_xor_i16_global(
; CHECK-NEXT:    [[TMP1:%.*]] = ptrtoint i16 addrspace(1)* [[PTR:%.*]] to i64
; CHECK-NEXT:    [[TMP2:%.*]] = and i64 [[TMP1]], -4
; CHECK-NEXT:    [[ALIGNEDADDR:%.*]] = inttoptr i64 [[TMP2]] to i32 addrspace(1)*
; CHECK-NEXT:    [[PTRLSB:%.*]] = and i64 [[TMP1]], 3
; CHECK-NEXT:    [[TMP3:%.*]] = shl i64 [[PTRLSB]], 3
; CHECK-NEXT:    [[SHIFTAMT:%.*]] = trunc i64 [[TMP3]] to i32
; CHECK-NEXT:    [[MASK:%.*]] = shl i32 65535, [[SHIFTAMT]]
; CHECK-NEXT:    [[INV_MASK:%.*]] = xor i32 [[MASK]], -1
; CHECK-NEXT:    [[TMP4:%.*]] = zext i16 [[VALUE:%.*]] to i32
; CHECK-NEXT:    [[VALOPERAND_SHIFTED:%.*]] = shl i32 [[TMP4]], [[SHIFTAMT]]
; CHECK-NEXT:    [[TMP5:%.*]] = atomicrmw xor i32 addrspace(1)* [[ALIGNEDADDR]], i32 [[VALOPERAND_SHIFTED]] seq_cst
; CHECK-NEXT:    [[TMP6:%.*]] = lshr i32 [[TMP5]], [[SHIFTAMT]]
; CHECK-NEXT:    [[TMP7:%.*]] = trunc i32 [[TMP6]] to i16
; CHECK-NEXT:    ret i16 [[TMP7]]
;
  %res = atomicrmw xor i16 addrspace(1)* %ptr, i16 %value seq_cst
  ret i16 %res
}

define i16 @test_atomicrmw_max_i16_global(i16 addrspace(1)* %ptr, i16 %value) {
; CHECK-LABEL: @test_atomicrmw_max_i16_global(
; CHECK-NEXT:    [[RES:%.*]] = atomicrmw max i16 addrspace(1)* [[PTR:%.*]], i16 [[VALUE:%.*]] seq_cst
; CHECK-NEXT:    ret i16 [[RES]]
;
  %res = atomicrmw max i16 addrspace(1)* %ptr, i16 %value seq_cst
  ret i16 %res
}

define i16 @test_atomicrmw_min_i16_global(i16 addrspace(1)* %ptr, i16 %value) {
; CHECK-LABEL: @test_atomicrmw_min_i16_global(
; CHECK-NEXT:    [[RES:%.*]] = atomicrmw min i16 addrspace(1)* [[PTR:%.*]], i16 [[VALUE:%.*]] seq_cst
; CHECK-NEXT:    ret i16 [[RES]]
;
  %res = atomicrmw min i16 addrspace(1)* %ptr, i16 %value seq_cst
  ret i16 %res
}

define i16 @test_atomicrmw_umax_i16_global(i16 addrspace(1)* %ptr, i16 %value) {
; CHECK-LABEL: @test_atomicrmw_umax_i16_global(
; CHECK-NEXT:    [[RES:%.*]] = atomicrmw umax i16 addrspace(1)* [[PTR:%.*]], i16 [[VALUE:%.*]] seq_cst
; CHECK-NEXT:    ret i16 [[RES]]
;
  %res = atomicrmw umax i16 addrspace(1)* %ptr, i16 %value seq_cst
  ret i16 %res
}

define i16 @test_atomicrmw_umin_i16_global(i16 addrspace(1)* %ptr, i16 %value) {
; CHECK-LABEL: @test_atomicrmw_umin_i16_global(
; CHECK-NEXT:    [[RES:%.*]] = atomicrmw umin i16 addrspace(1)* [[PTR:%.*]], i16 [[VALUE:%.*]] seq_cst
; CHECK-NEXT:    ret i16 [[RES]]
;
  %res = atomicrmw umin i16 addrspace(1)* %ptr, i16 %value seq_cst
  ret i16 %res
}

define i16 @test_cmpxchg_i16_global(i16 addrspace(1)* %out, i16 %in, i16 %old) {
; CHECK-LABEL: @test_cmpxchg_i16_global(
; CHECK-NEXT:    [[GEP:%.*]] = getelementptr i16, i16 addrspace(1)* [[OUT:%.*]], i64 4
; CHECK-NEXT:    [[TMP1:%.*]] = ptrtoint i16 addrspace(1)* [[GEP]] to i64
; CHECK-NEXT:    [[TMP2:%.*]] = and i64 [[TMP1]], -4
; CHECK-NEXT:    [[ALIGNEDADDR:%.*]] = inttoptr i64 [[TMP2]] to i32 addrspace(1)*
; CHECK-NEXT:    [[PTRLSB:%.*]] = and i64 [[TMP1]], 3
; CHECK-NEXT:    [[TMP3:%.*]] = shl i64 [[PTRLSB]], 3
; CHECK-NEXT:    [[SHIFTAMT:%.*]] = trunc i64 [[TMP3]] to i32
; CHECK-NEXT:    [[MASK:%.*]] = shl i32 65535, [[SHIFTAMT]]
; CHECK-NEXT:    [[INV_MASK:%.*]] = xor i32 [[MASK]], -1
; CHECK-NEXT:    [[TMP4:%.*]] = zext i16 [[IN:%.*]] to i32
; CHECK-NEXT:    [[TMP5:%.*]] = shl i32 [[TMP4]], [[SHIFTAMT]]
; CHECK-NEXT:    [[TMP6:%.*]] = zext i16 [[OLD:%.*]] to i32
; CHECK-NEXT:    [[TMP7:%.*]] = shl i32 [[TMP6]], [[SHIFTAMT]]
; CHECK-NEXT:    [[TMP8:%.*]] = load i32, i32 addrspace(1)* [[ALIGNEDADDR]]
; CHECK-NEXT:    [[TMP9:%.*]] = and i32 [[TMP8]], [[INV_MASK]]
; CHECK-NEXT:    br label [[PARTWORD_CMPXCHG_LOOP:%.*]]
; CHECK:       partword.cmpxchg.loop:
; CHECK-NEXT:    [[TMP10:%.*]] = phi i32 [ [[TMP9]], [[TMP0:%.*]] ], [ [[TMP16:%.*]], [[PARTWORD_CMPXCHG_FAILURE:%.*]] ]
; CHECK-NEXT:    [[TMP11:%.*]] = or i32 [[TMP10]], [[TMP5]]
; CHECK-NEXT:    [[TMP12:%.*]] = or i32 [[TMP10]], [[TMP7]]
; CHECK-NEXT:    [[TMP13:%.*]] = cmpxchg i32 addrspace(1)* [[ALIGNEDADDR]], i32 [[TMP12]], i32 [[TMP11]] seq_cst seq_cst
; CHECK-NEXT:    [[TMP14:%.*]] = extractvalue { i32, i1 } [[TMP13]], 0
; CHECK-NEXT:    [[TMP15:%.*]] = extractvalue { i32, i1 } [[TMP13]], 1
; CHECK-NEXT:    br i1 [[TMP15]], label [[PARTWORD_CMPXCHG_END:%.*]], label [[PARTWORD_CMPXCHG_FAILURE]]
; CHECK:       partword.cmpxchg.failure:
; CHECK-NEXT:    [[TMP16]] = and i32 [[TMP14]], [[INV_MASK]]
; CHECK-NEXT:    [[TMP17:%.*]] = icmp ne i32 [[TMP10]], [[TMP16]]
; CHECK-NEXT:    br i1 [[TMP17]], label [[PARTWORD_CMPXCHG_LOOP]], label [[PARTWORD_CMPXCHG_END]]
; CHECK:       partword.cmpxchg.end:
; CHECK-NEXT:    [[TMP18:%.*]] = lshr i32 [[TMP14]], [[SHIFTAMT]]
; CHECK-NEXT:    [[TMP19:%.*]] = trunc i32 [[TMP18]] to i16
; CHECK-NEXT:    [[TMP20:%.*]] = insertvalue { i16, i1 } undef, i16 [[TMP19]], 0
; CHECK-NEXT:    [[TMP21:%.*]] = insertvalue { i16, i1 } [[TMP20]], i1 [[TMP15]], 1
; CHECK-NEXT:    [[EXTRACT:%.*]] = extractvalue { i16, i1 } [[TMP21]], 0
; CHECK-NEXT:    ret i16 [[EXTRACT]]
;
  %gep = getelementptr i16, i16 addrspace(1)* %out, i64 4
  %res = cmpxchg i16 addrspace(1)* %gep, i16 %old, i16 %in seq_cst seq_cst
  %extract = extractvalue {i16, i1} %res, 0
  ret i16 %extract
}
