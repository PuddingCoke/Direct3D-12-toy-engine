#if 0
;
; Note: shader requires additional functionality:
;       Resource descriptor heap indexing
;
;
; Input signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; no parameters
;
; Output signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; no parameters
; shader hash: 115760d967c5217580adce6f259417ad
;
; Pipeline Runtime Information: 
;
;
;
; Buffer Definitions:
;
; cbuffer TextureIdx
; {
;
;   struct TextureIdx
;   {
;
;       uint rgbaTexIdx;                              ; Offset:    0
;       uint lumaTexIdx;                              ; Offset:    4
;       uint chromaTexIdx;                            ; Offset:    8
;   
;   } TextureIdx;                                     ; Offset:    0 Size:    12
;
; }
;
;
; Resource Bindings:
;
; Name                                 Type  Format         Dim      ID      HLSL Bind  Count
; ------------------------------ ---------- ------- ----------- ------- -------------- ------
; TextureIdx                        cbuffer      NA          NA     CB0            cb1     1
;
target datalayout = "e-m:e-p:32:32-i1:32-i8:32-i16:32-i32:32-i64:64-f16:32-f32:32-f64:64-n8:16:32:64"
target triple = "dxil-ms-dx"

%dx.types.Handle = type { i8* }
%dx.types.ResBind = type { i32, i32, i32, i8 }
%dx.types.ResourceProperties = type { i32, i32 }
%dx.types.CBufRet.i32 = type { i32, i32, i32, i32 }
%dx.types.ResRet.f32 = type { float, float, float, float, i32 }
%TextureIdx = type { i32, i32, i32 }

define void @main() {
  %1 = call %dx.types.Handle @dx.op.createHandleFromBinding(i32 217, %dx.types.ResBind { i32 1, i32 1, i32 0, i8 2 }, i32 1, i1 false)  ; CreateHandleFromBinding(bind,index,nonUniformIndex)
  %2 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %1, %dx.types.ResourceProperties { i32 13, i32 12 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %3 = call i32 @dx.op.threadId.i32(i32 93, i32 0)  ; ThreadId(component)
  %4 = call i32 @dx.op.threadId.i32(i32 93, i32 1)  ; ThreadId(component)
  %5 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %6 = extractvalue %dx.types.CBufRet.i32 %5, 0
  %7 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %6, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %8 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %7, %dx.types.ResourceProperties { i32 2, i32 1033 })  ; AnnotateHandle(res,props)  resource: Texture2D<4xF32>
  %9 = extractvalue %dx.types.CBufRet.i32 %5, 1
  %10 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %9, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %11 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %10, %dx.types.ResourceProperties { i32 4098, i32 265 })  ; AnnotateHandle(res,props)  resource: RWTexture2D<F32>
  %12 = extractvalue %dx.types.CBufRet.i32 %5, 2
  %13 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %12, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %14 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %13, %dx.types.ResourceProperties { i32 4098, i32 521 })  ; AnnotateHandle(res,props)  resource: RWTexture2D<2xF32>
  %15 = shl i32 %3, 1
  %16 = shl i32 %4, 1
  %17 = call %dx.types.ResRet.f32 @dx.op.textureLoad.f32(i32 66, %dx.types.Handle %8, i32 0, i32 %15, i32 %16, i32 undef, i32 undef, i32 undef, i32 undef)  ; TextureLoad(srv,mipLevelOrSampleCount,coord0,coord1,coord2,offset0,offset1,offset2)
  %18 = extractvalue %dx.types.ResRet.f32 %17, 0
  %19 = extractvalue %dx.types.ResRet.f32 %17, 1
  %20 = extractvalue %dx.types.ResRet.f32 %17, 2
  %21 = fmul fast float %18, 0x3FD322D0E0000000
  %22 = fmul fast float %19, 0x3FE2C8B440000000
  %23 = fadd fast float %21, %22
  %24 = fmul fast float %20, 0x3FBD2F1AA0000000
  %25 = fadd fast float %23, %24
  %26 = or i32 %15, 1
  %27 = call %dx.types.ResRet.f32 @dx.op.textureLoad.f32(i32 66, %dx.types.Handle %8, i32 0, i32 %26, i32 %16, i32 undef, i32 undef, i32 undef, i32 undef)  ; TextureLoad(srv,mipLevelOrSampleCount,coord0,coord1,coord2,offset0,offset1,offset2)
  %28 = extractvalue %dx.types.ResRet.f32 %27, 0
  %29 = extractvalue %dx.types.ResRet.f32 %27, 1
  %30 = extractvalue %dx.types.ResRet.f32 %27, 2
  %31 = fmul fast float %28, 0x3FD322D0E0000000
  %32 = fmul fast float %29, 0x3FE2C8B440000000
  %33 = fadd fast float %31, %32
  %34 = fmul fast float %30, 0x3FBD2F1AA0000000
  %35 = fadd fast float %33, %34
  %36 = or i32 %16, 1
  %37 = call %dx.types.ResRet.f32 @dx.op.textureLoad.f32(i32 66, %dx.types.Handle %8, i32 0, i32 %15, i32 %36, i32 undef, i32 undef, i32 undef, i32 undef)  ; TextureLoad(srv,mipLevelOrSampleCount,coord0,coord1,coord2,offset0,offset1,offset2)
  %38 = extractvalue %dx.types.ResRet.f32 %37, 0
  %39 = extractvalue %dx.types.ResRet.f32 %37, 1
  %40 = extractvalue %dx.types.ResRet.f32 %37, 2
  %41 = fmul fast float %38, 0x3FD322D0E0000000
  %42 = fmul fast float %39, 0x3FE2C8B440000000
  %43 = fadd fast float %41, %42
  %44 = fmul fast float %40, 0x3FBD2F1AA0000000
  %45 = fadd fast float %43, %44
  %46 = call %dx.types.ResRet.f32 @dx.op.textureLoad.f32(i32 66, %dx.types.Handle %8, i32 0, i32 %26, i32 %36, i32 undef, i32 undef, i32 undef, i32 undef)  ; TextureLoad(srv,mipLevelOrSampleCount,coord0,coord1,coord2,offset0,offset1,offset2)
  %47 = extractvalue %dx.types.ResRet.f32 %46, 0
  %48 = extractvalue %dx.types.ResRet.f32 %46, 1
  %49 = extractvalue %dx.types.ResRet.f32 %46, 2
  %50 = fmul fast float %47, 0x3FD322D0E0000000
  %51 = fmul fast float %48, 0x3FE2C8B440000000
  %52 = fadd fast float %50, %51
  %53 = fmul fast float %49, 0x3FBD2F1AA0000000
  %54 = fadd fast float %52, %53
  call void @dx.op.textureStore.f32(i32 67, %dx.types.Handle %11, i32 %15, i32 %16, i32 undef, float %25, float %25, float %25, float %25, i8 15)  ; TextureStore(srv,coord0,coord1,coord2,value0,value1,value2,value3,mask)
  call void @dx.op.textureStore.f32(i32 67, %dx.types.Handle %11, i32 %26, i32 %16, i32 undef, float %35, float %35, float %35, float %35, i8 15)  ; TextureStore(srv,coord0,coord1,coord2,value0,value1,value2,value3,mask)
  call void @dx.op.textureStore.f32(i32 67, %dx.types.Handle %11, i32 %15, i32 %36, i32 undef, float %45, float %45, float %45, float %45, i8 15)  ; TextureStore(srv,coord0,coord1,coord2,value0,value1,value2,value3,mask)
  call void @dx.op.textureStore.f32(i32 67, %dx.types.Handle %11, i32 %26, i32 %36, i32 undef, float %54, float %54, float %54, float %54, i8 15)  ; TextureStore(srv,coord0,coord1,coord2,value0,value1,value2,value3,mask)
  %55 = fadd fast float %30, %20
  %56 = fadd fast float %55, %40
  %57 = fsub fast float %56, %25
  %58 = fadd fast float %57, %49
  %59 = fsub fast float %58, %35
  %60 = fsub fast float %59, %45
  %61 = fsub fast float %60, %54
  %62 = fadd fast float %28, %18
  %63 = fadd fast float %62, %38
  %64 = fsub fast float %63, %25
  %65 = fadd fast float %64, %47
  %66 = fsub fast float %65, %35
  %67 = fsub fast float %66, %45
  %68 = fsub fast float %67, %54
  %69 = fmul fast float %61, 0x3FC20C49C0000000
  %70 = fadd fast float %69, 5.000000e-01
  %71 = fmul fast float %68, 0x3FC6D0E560000000
  %72 = fadd fast float %71, 5.000000e-01
  call void @dx.op.textureStore.f32(i32 67, %dx.types.Handle %14, i32 %3, i32 %4, i32 undef, float %70, float %72, float %70, float %70, i8 15)  ; TextureStore(srv,coord0,coord1,coord2,value0,value1,value2,value3,mask)
  ret void
}

; Function Attrs: nounwind readnone
declare i32 @dx.op.threadId.i32(i32, i32) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromHeap(i32, i32, i1, i1) #0

; Function Attrs: nounwind readonly
declare %dx.types.ResRet.f32 @dx.op.textureLoad.f32(i32, %dx.types.Handle, i32, i32, i32, i32, i32, i32, i32) #1

; Function Attrs: nounwind
declare void @dx.op.textureStore.f32(i32, %dx.types.Handle, i32, i32, i32, float, float, float, float, i8) #2

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32, %dx.types.Handle, i32) #1

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.annotateHandle(i32, %dx.types.Handle, %dx.types.ResourceProperties) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromBinding(i32, %dx.types.ResBind, i32, i1) #0

attributes #0 = { nounwind readnone }
attributes #1 = { nounwind readonly }
attributes #2 = { nounwind }

!llvm.ident = !{!0}
!dx.version = !{!1}
!dx.valver = !{!1}
!dx.shaderModel = !{!2}
!dx.resources = !{!3}
!dx.entryPoints = !{!6}

!0 = !{!"clang version 3.7 (tags/RELEASE_370/final)"}
!1 = !{i32 1, i32 6}
!2 = !{!"cs", i32 6, i32 6}
!3 = !{null, null, !4, null}
!4 = !{!5}
!5 = !{i32 0, %TextureIdx* undef, !"", i32 0, i32 1, i32 1, i32 12, null}
!6 = !{void ()* @main, !"main", null, !3, !7}
!7 = !{i32 0, i64 1073741824, i32 4, !8}
!8 = !{i32 2, i32 2, i32 1}

#endif

const unsigned char g_RGBToNV12CSBytes[] = {
  0x44, 0x58, 0x42, 0x43, 0xe7, 0xd3, 0x01, 0x13, 0x22, 0x2e, 0x8c, 0x8d,
  0x04, 0x14, 0xd1, 0xc7, 0x03, 0x1d, 0xc5, 0xec, 0x01, 0x00, 0x00, 0x00,
  0x54, 0x0f, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x00,
  0x4c, 0x00, 0x00, 0x00, 0x5c, 0x00, 0x00, 0x00, 0x6c, 0x00, 0x00, 0x00,
  0xd4, 0x00, 0x00, 0x00, 0x40, 0x07, 0x00, 0x00, 0x5c, 0x07, 0x00, 0x00,
  0x53, 0x46, 0x49, 0x30, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02,
  0x00, 0x00, 0x00, 0x00, 0x49, 0x53, 0x47, 0x31, 0x08, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x4f, 0x53, 0x47, 0x31,
  0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00,
  0x50, 0x53, 0x56, 0x30, 0x60, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff,
  0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x02, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
  0x01, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
  0x0d, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x53, 0x54, 0x41, 0x54,
  0x64, 0x06, 0x00, 0x00, 0x66, 0x00, 0x05, 0x00, 0x99, 0x01, 0x00, 0x00,
  0x44, 0x58, 0x49, 0x4c, 0x06, 0x01, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00,
  0x4c, 0x06, 0x00, 0x00, 0x42, 0x43, 0xc0, 0xde, 0x21, 0x0c, 0x00, 0x00,
  0x90, 0x01, 0x00, 0x00, 0x0b, 0x82, 0x20, 0x00, 0x02, 0x00, 0x00, 0x00,
  0x13, 0x00, 0x00, 0x00, 0x07, 0x81, 0x23, 0x91, 0x41, 0xc8, 0x04, 0x49,
  0x06, 0x10, 0x32, 0x39, 0x92, 0x01, 0x84, 0x0c, 0x25, 0x05, 0x08, 0x19,
  0x1e, 0x04, 0x8b, 0x62, 0x80, 0x14, 0x45, 0x02, 0x42, 0x92, 0x0b, 0x42,
  0xa4, 0x10, 0x32, 0x14, 0x38, 0x08, 0x18, 0x4b, 0x0a, 0x32, 0x52, 0x88,
  0x48, 0x90, 0x14, 0x20, 0x43, 0x46, 0x88, 0xa5, 0x00, 0x19, 0x32, 0x42,
  0xe4, 0x48, 0x0e, 0x90, 0x91, 0x22, 0xc4, 0x50, 0x41, 0x51, 0x81, 0x8c,
  0xe1, 0x83, 0xe5, 0x8a, 0x04, 0x29, 0x46, 0x06, 0x51, 0x18, 0x00, 0x00,
  0x08, 0x00, 0x00, 0x00, 0x1b, 0x8c, 0xe0, 0xff, 0xff, 0xff, 0xff, 0x07,
  0x40, 0x02, 0xa8, 0x0d, 0x86, 0xf0, 0xff, 0xff, 0xff, 0xff, 0x03, 0x20,
  0x01, 0xd5, 0x06, 0x62, 0xf8, 0xff, 0xff, 0xff, 0xff, 0x01, 0x90, 0x00,
  0x49, 0x18, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x13, 0x82, 0x60, 0x42,
  0x20, 0x4c, 0x08, 0x06, 0x00, 0x00, 0x00, 0x00, 0x89, 0x20, 0x00, 0x00,
  0x34, 0x00, 0x00, 0x00, 0x32, 0x22, 0x48, 0x09, 0x20, 0x64, 0x85, 0x04,
  0x93, 0x22, 0xa4, 0x84, 0x04, 0x93, 0x22, 0xe3, 0x84, 0xa1, 0x90, 0x14,
  0x12, 0x4c, 0x8a, 0x8c, 0x0b, 0x84, 0xa4, 0x4c, 0x10, 0x7c, 0x23, 0x00,
  0x25, 0x00, 0x14, 0xe6, 0x08, 0xc0, 0xa0, 0x0c, 0x63, 0x0c, 0x22, 0x73,
  0x04, 0x08, 0x99, 0x7b, 0x86, 0xcb, 0x9f, 0xb0, 0x87, 0x90, 0xfc, 0x10,
  0x68, 0x86, 0x85, 0x40, 0xc1, 0x99, 0x23, 0x08, 0x4a, 0x81, 0xc6, 0x48,
  0x89, 0xd4, 0x0c, 0xc0, 0x4d, 0xc3, 0xe5, 0x4f, 0xd8, 0x43, 0x48, 0xfe,
  0x4a, 0x48, 0x2b, 0x31, 0xf9, 0xc5, 0x6d, 0xa3, 0x82, 0x31, 0xc6, 0x46,
  0x51, 0xda, 0x40, 0x63, 0x8c, 0x31, 0xc6, 0x20, 0x57, 0x16, 0x30, 0xd0,
  0x18, 0x83, 0x31, 0xc6, 0x0c, 0xc1, 0xa3, 0x86, 0xcb, 0x9f, 0xb0, 0x87,
  0x90, 0x7c, 0x6e, 0xa3, 0x8a, 0x95, 0x98, 0x7c, 0xe4, 0xb6, 0x11, 0x31,
  0xc6, 0x18, 0x85, 0x90, 0x03, 0x0d, 0x9a, 0xb7, 0x0d, 0x97, 0x3f, 0x61,
  0x0f, 0x21, 0xf9, 0x2b, 0x21, 0x39, 0x54, 0x24, 0x10, 0x69, 0xe4, 0x3c,
  0x44, 0x34, 0x21, 0x84, 0x84, 0x84, 0x31, 0x0a, 0x81, 0x06, 0xaa, 0x64,
  0x0f, 0x1a, 0x2e, 0x7f, 0xc2, 0x1e, 0x42, 0xf2, 0x57, 0x42, 0xda, 0x90,
  0x66, 0x40, 0xc4, 0x18, 0xc3, 0x94, 0x02, 0x0d, 0x3c, 0x12, 0xe5, 0x81,
  0x80, 0x53, 0x5a, 0xe2, 0x9a, 0xa8, 0x88, 0x10, 0x87, 0x0b, 0x0d, 0x63,
  0x0c, 0xe2, 0x73, 0x04, 0xa0, 0x00, 0x00, 0x00, 0x13, 0x14, 0x72, 0xc0,
  0x87, 0x74, 0x60, 0x87, 0x36, 0x68, 0x87, 0x79, 0x68, 0x03, 0x72, 0xc0,
  0x87, 0x0d, 0xaf, 0x50, 0x0e, 0x6d, 0xd0, 0x0e, 0x7a, 0x50, 0x0e, 0x6d,
  0x00, 0x0f, 0x7a, 0x30, 0x07, 0x72, 0xa0, 0x07, 0x73, 0x20, 0x07, 0x6d,
  0x90, 0x0e, 0x71, 0xa0, 0x07, 0x73, 0x20, 0x07, 0x6d, 0x90, 0x0e, 0x78,
  0xa0, 0x07, 0x73, 0x20, 0x07, 0x6d, 0x90, 0x0e, 0x71, 0x60, 0x07, 0x7a,
  0x30, 0x07, 0x72, 0xd0, 0x06, 0xe9, 0x30, 0x07, 0x72, 0xa0, 0x07, 0x73,
  0x20, 0x07, 0x6d, 0x90, 0x0e, 0x76, 0x40, 0x07, 0x7a, 0x60, 0x07, 0x74,
  0xd0, 0x06, 0xe6, 0x10, 0x07, 0x76, 0xa0, 0x07, 0x73, 0x20, 0x07, 0x6d,
  0x60, 0x0e, 0x73, 0x20, 0x07, 0x7a, 0x30, 0x07, 0x72, 0xd0, 0x06, 0xe6,
  0x60, 0x07, 0x74, 0xa0, 0x07, 0x76, 0x40, 0x07, 0x6d, 0xe0, 0x0e, 0x78,
  0xa0, 0x07, 0x71, 0x60, 0x07, 0x7a, 0x30, 0x07, 0x72, 0xa0, 0x07, 0x76,
  0x40, 0x07, 0x43, 0x9e, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x86, 0x3c, 0x04, 0x10, 0x00, 0x01, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x0c, 0x79, 0x14, 0x20, 0x00, 0x02, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0xf2, 0x38, 0x40, 0x00, 0x08, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0xe4, 0x81, 0x80, 0x00, 0x18,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0xc8, 0x33, 0x01, 0x01,
  0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x90, 0xc7, 0x02,
  0x02, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x21, 0x4f,
  0x06, 0x04, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x59,
  0x20, 0x00, 0x00, 0x00, 0x0e, 0x00, 0x00, 0x00, 0x32, 0x1e, 0x98, 0x14,
  0x19, 0x11, 0x4c, 0x90, 0x8c, 0x09, 0x26, 0x47, 0xc6, 0x04, 0x43, 0x1a,
  0x25, 0x50, 0x0c, 0x23, 0x00, 0x85, 0x51, 0x06, 0xe5, 0x50, 0x0a, 0x25,
  0x51, 0x08, 0x05, 0x51, 0x04, 0xc5, 0x54, 0x9c, 0x01, 0x94, 0x6a, 0x80,
  0x7c, 0x01, 0x02, 0x02, 0x02, 0x22, 0x50, 0x9f, 0x01, 0x20, 0x3e, 0x03,
  0x00, 0x00, 0x00, 0x00, 0x79, 0x18, 0x00, 0x00, 0x71, 0x00, 0x00, 0x00,
  0x1a, 0x03, 0x4c, 0x90, 0x46, 0x02, 0x13, 0x44, 0x35, 0x18, 0x63, 0x0b,
  0x73, 0x3b, 0x03, 0xb1, 0x2b, 0x93, 0x9b, 0x4b, 0x7b, 0x73, 0x03, 0x99,
  0x71, 0xb9, 0x01, 0x41, 0xa1, 0x0b, 0x3b, 0x9b, 0x7b, 0x91, 0x2a, 0x62,
  0x2a, 0x0a, 0x9a, 0x2a, 0xfa, 0x9a, 0xb9, 0x81, 0x79, 0x31, 0x4b, 0x73,
  0x0b, 0x63, 0x4b, 0xd9, 0x10, 0x04, 0x13, 0x84, 0x01, 0x99, 0x20, 0x0c,
  0xc9, 0x06, 0x61, 0x20, 0x26, 0x08, 0x83, 0xb2, 0x41, 0x30, 0x0c, 0x0a,
  0x63, 0x73, 0x1b, 0x06, 0x84, 0x20, 0x26, 0x08, 0xdd, 0x45, 0x85, 0xaa,
  0x0c, 0x8f, 0xae, 0x4e, 0xae, 0x2c, 0x89, 0x0c, 0x6f, 0x82, 0x30, 0x2c,
  0x1b, 0x10, 0x43, 0x59, 0x8c, 0x61, 0x60, 0x80, 0x0d, 0x41, 0xb3, 0x81,
  0x00, 0x00, 0x07, 0x98, 0x20, 0x70, 0x18, 0x15, 0xb9, 0x33, 0xb1, 0x30,
  0xaa, 0x32, 0xbc, 0x24, 0x32, 0xbc, 0x09, 0xc2, 0xc0, 0x4c, 0x10, 0x86,
  0x66, 0x82, 0x30, 0x38, 0x13, 0x84, 0xe1, 0x99, 0x20, 0x24, 0xd5, 0x06,
  0x84, 0x88, 0x24, 0x63, 0xa2, 0x2a, 0x8b, 0x0a, 0x5b, 0x5d, 0x5b, 0x18,
  0x55, 0x19, 0x5e, 0x12, 0x19, 0xde, 0x04, 0x61, 0x80, 0x36, 0x20, 0x04,
  0x26, 0x65, 0x13, 0x55, 0x59, 0x64, 0xc6, 0xd0, 0xe4, 0xde, 0xda, 0xc2,
  0xa8, 0xca, 0xf0, 0x92, 0xc8, 0xf0, 0x26, 0x08, 0x43, 0xb4, 0x01, 0x21,
  0x36, 0x89, 0x9b, 0xa8, 0xca, 0xda, 0x40, 0x30, 0x97, 0xd6, 0x6d, 0x18,
  0x0c, 0xc8, 0x9b, 0x20, 0x08, 0xc0, 0x06, 0x60, 0xc3, 0x60, 0x84, 0x41,
  0x18, 0x6c, 0x08, 0xc4, 0x60, 0xc3, 0x30, 0x80, 0xc1, 0x18, 0x90, 0x68,
  0x0b, 0x4b, 0x73, 0x9b, 0x20, 0x78, 0xd6, 0x04, 0x61, 0x90, 0x36, 0x0c,
  0x67, 0x70, 0x06, 0xc3, 0x06, 0xc2, 0x30, 0x83, 0x0c, 0x0d, 0x36, 0x14,
  0x60, 0x50, 0x06, 0xc0, 0x93, 0x06, 0x34, 0xcc, 0xd8, 0xde, 0xc2, 0xe8,
  0xe6, 0x26, 0x08, 0xc3, 0xc4, 0x22, 0xcd, 0x6d, 0x8e, 0x6e, 0x6e, 0x82,
  0x30, 0x50, 0x44, 0xe8, 0xca, 0xf0, 0xbe, 0xd8, 0xde, 0xc2, 0xc8, 0x98,
  0xd0, 0x95, 0xe1, 0x7d, 0xcd, 0xd1, 0xbd, 0xc9, 0x95, 0xb1, 0xa8, 0x4b,
  0x73, 0xa3, 0x9b, 0xdb, 0xa0, 0xac, 0x01, 0x1b, 0xb4, 0x81, 0x1b, 0xbc,
  0x41, 0x06, 0x07, 0x54, 0x1c, 0x64, 0x55, 0xd8, 0xd8, 0xec, 0xda, 0x5c,
  0xd2, 0xc8, 0xca, 0xdc, 0xe8, 0xa6, 0x04, 0x41, 0x15, 0x32, 0x3c, 0x17,
  0xbb, 0x32, 0xb9, 0xb9, 0xb4, 0x37, 0xb7, 0x29, 0x01, 0xd1, 0x84, 0x0c,
  0xcf, 0xc5, 0x2e, 0x8c, 0xcd, 0xae, 0x4c, 0x6e, 0x4a, 0x60, 0xd4, 0x21,
  0xc3, 0x73, 0x99, 0x43, 0x0b, 0x23, 0x2b, 0x93, 0x6b, 0x7a, 0x23, 0x2b,
  0x63, 0x9b, 0x12, 0x20, 0x65, 0xc8, 0xf0, 0x5c, 0xe4, 0xca, 0xe6, 0xde,
  0xea, 0xe4, 0xc6, 0xca, 0xe6, 0xa6, 0x04, 0x4e, 0x25, 0x32, 0x3c, 0x17,
  0xba, 0x3c, 0xb8, 0xb2, 0x20, 0x37, 0xb7, 0x37, 0xba, 0x30, 0xba, 0xb4,
  0x37, 0xb7, 0xb9, 0x29, 0x82, 0x37, 0x06, 0x75, 0xc8, 0xf0, 0x5c, 0xca,
  0xdc, 0xe8, 0xe4, 0xf2, 0xa0, 0xde, 0xd2, 0xdc, 0xe8, 0xe6, 0xa6, 0x04,
  0x69, 0xd0, 0x85, 0x0c, 0xcf, 0x65, 0xec, 0xad, 0xce, 0x8d, 0xae, 0x4c,
  0x6e, 0x6e, 0x4a, 0x10, 0x07, 0x00, 0x00, 0x00, 0x79, 0x18, 0x00, 0x00,
  0x4c, 0x00, 0x00, 0x00, 0x33, 0x08, 0x80, 0x1c, 0xc4, 0xe1, 0x1c, 0x66,
  0x14, 0x01, 0x3d, 0x88, 0x43, 0x38, 0x84, 0xc3, 0x8c, 0x42, 0x80, 0x07,
  0x79, 0x78, 0x07, 0x73, 0x98, 0x71, 0x0c, 0xe6, 0x00, 0x0f, 0xed, 0x10,
  0x0e, 0xf4, 0x80, 0x0e, 0x33, 0x0c, 0x42, 0x1e, 0xc2, 0xc1, 0x1d, 0xce,
  0xa1, 0x1c, 0x66, 0x30, 0x05, 0x3d, 0x88, 0x43, 0x38, 0x84, 0x83, 0x1b,
  0xcc, 0x03, 0x3d, 0xc8, 0x43, 0x3d, 0x8c, 0x03, 0x3d, 0xcc, 0x78, 0x8c,
  0x74, 0x70, 0x07, 0x7b, 0x08, 0x07, 0x79, 0x48, 0x87, 0x70, 0x70, 0x07,
  0x7a, 0x70, 0x03, 0x76, 0x78, 0x87, 0x70, 0x20, 0x87, 0x19, 0xcc, 0x11,
  0x0e, 0xec, 0x90, 0x0e, 0xe1, 0x30, 0x0f, 0x6e, 0x30, 0x0f, 0xe3, 0xf0,
  0x0e, 0xf0, 0x50, 0x0e, 0x33, 0x10, 0xc4, 0x1d, 0xde, 0x21, 0x1c, 0xd8,
  0x21, 0x1d, 0xc2, 0x61, 0x1e, 0x66, 0x30, 0x89, 0x3b, 0xbc, 0x83, 0x3b,
  0xd0, 0x43, 0x39, 0xb4, 0x03, 0x3c, 0xbc, 0x83, 0x3c, 0x84, 0x03, 0x3b,
  0xcc, 0xf0, 0x14, 0x76, 0x60, 0x07, 0x7b, 0x68, 0x07, 0x37, 0x68, 0x87,
  0x72, 0x68, 0x07, 0x37, 0x80, 0x87, 0x70, 0x90, 0x87, 0x70, 0x60, 0x07,
  0x76, 0x28, 0x07, 0x76, 0xf8, 0x05, 0x76, 0x78, 0x87, 0x77, 0x80, 0x87,
  0x5f, 0x08, 0x87, 0x71, 0x18, 0x87, 0x72, 0x98, 0x87, 0x79, 0x98, 0x81,
  0x2c, 0xee, 0xf0, 0x0e, 0xee, 0xe0, 0x0e, 0xf5, 0xc0, 0x0e, 0xec, 0x30,
  0x03, 0x62, 0xc8, 0xa1, 0x1c, 0xe4, 0xa1, 0x1c, 0xcc, 0xa1, 0x1c, 0xe4,
  0xa1, 0x1c, 0xdc, 0x61, 0x1c, 0xca, 0x21, 0x1c, 0xc4, 0x81, 0x1d, 0xca,
  0x61, 0x06, 0xd6, 0x90, 0x43, 0x39, 0xc8, 0x43, 0x39, 0x98, 0x43, 0x39,
  0xc8, 0x43, 0x39, 0xb8, 0xc3, 0x38, 0x94, 0x43, 0x38, 0x88, 0x03, 0x3b,
  0x94, 0xc3, 0x2f, 0xbc, 0x83, 0x3c, 0xfc, 0x82, 0x3b, 0xd4, 0x03, 0x3b,
  0xb0, 0xc3, 0x0c, 0xc4, 0x21, 0x07, 0x7c, 0x70, 0x03, 0x7a, 0x28, 0x87,
  0x76, 0x80, 0x87, 0x19, 0xd1, 0x43, 0x0e, 0xf8, 0xe0, 0x06, 0xe4, 0x20,
  0x0e, 0xe7, 0xe0, 0x06, 0xf6, 0x10, 0x0e, 0xf2, 0xc0, 0x0e, 0xe1, 0x90,
  0x0f, 0xef, 0x50, 0x0f, 0xf4, 0x00, 0x00, 0x00, 0x71, 0x20, 0x00, 0x00,
  0x24, 0x00, 0x00, 0x00, 0x66, 0x40, 0x0d, 0x97, 0xef, 0x3c, 0x3e, 0xd0,
  0x34, 0xce, 0x04, 0x4c, 0x44, 0x08, 0x34, 0xc3, 0x42, 0x58, 0xc1, 0x36,
  0x5c, 0xbe, 0xf3, 0xf8, 0x42, 0x40, 0x15, 0x05, 0x11, 0x95, 0x0e, 0x30,
  0x94, 0x84, 0x01, 0x08, 0x98, 0x8f, 0xdc, 0xb6, 0x1d, 0x74, 0xc3, 0xe5,
  0x3b, 0x8f, 0x2f, 0x44, 0x04, 0x30, 0x11, 0x21, 0xd0, 0x0c, 0x0b, 0xf1,
  0x45, 0x0e, 0xb3, 0x21, 0xcd, 0x80, 0x34, 0x86, 0x09, 0x68, 0xc3, 0xe5,
  0x3b, 0x8f, 0x2f, 0x44, 0x04, 0x30, 0x11, 0x21, 0xd0, 0x0c, 0x0b, 0xf1,
  0x45, 0x0e, 0x13, 0x12, 0xc0, 0x63, 0x03, 0xd5, 0x70, 0xf9, 0xce, 0xe3,
  0x4f, 0xc4, 0x35, 0x51, 0x11, 0x51, 0x3a, 0xc0, 0xe0, 0x17, 0xb7, 0x6d,
  0x04, 0xd6, 0x70, 0xf9, 0xce, 0xe3, 0x4f, 0xc4, 0x35, 0x51, 0x11, 0xc1,
  0x4e, 0x4e, 0x44, 0xf8, 0xc5, 0x6d, 0x5b, 0x80, 0x34, 0x5c, 0xbe, 0xf3,
  0xf8, 0xd3, 0x11, 0x11, 0xc0, 0x20, 0x0e, 0x3e, 0x72, 0xdb, 0x06, 0x40,
  0x30, 0x00, 0xd2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x48, 0x41, 0x53, 0x48,
  0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x57, 0x60, 0xd9,
  0x67, 0xc5, 0x21, 0x75, 0x80, 0xad, 0xce, 0x6f, 0x25, 0x94, 0x17, 0xad,
  0x44, 0x58, 0x49, 0x4c, 0xf0, 0x07, 0x00, 0x00, 0x66, 0x00, 0x05, 0x00,
  0xfc, 0x01, 0x00, 0x00, 0x44, 0x58, 0x49, 0x4c, 0x06, 0x01, 0x00, 0x00,
  0x10, 0x00, 0x00, 0x00, 0xd8, 0x07, 0x00, 0x00, 0x42, 0x43, 0xc0, 0xde,
  0x21, 0x0c, 0x00, 0x00, 0xf3, 0x01, 0x00, 0x00, 0x0b, 0x82, 0x20, 0x00,
  0x02, 0x00, 0x00, 0x00, 0x13, 0x00, 0x00, 0x00, 0x07, 0x81, 0x23, 0x91,
  0x41, 0xc8, 0x04, 0x49, 0x06, 0x10, 0x32, 0x39, 0x92, 0x01, 0x84, 0x0c,
  0x25, 0x05, 0x08, 0x19, 0x1e, 0x04, 0x8b, 0x62, 0x80, 0x14, 0x45, 0x02,
  0x42, 0x92, 0x0b, 0x42, 0xa4, 0x10, 0x32, 0x14, 0x38, 0x08, 0x18, 0x4b,
  0x0a, 0x32, 0x52, 0x88, 0x48, 0x90, 0x14, 0x20, 0x43, 0x46, 0x88, 0xa5,
  0x00, 0x19, 0x32, 0x42, 0xe4, 0x48, 0x0e, 0x90, 0x91, 0x22, 0xc4, 0x50,
  0x41, 0x51, 0x81, 0x8c, 0xe1, 0x83, 0xe5, 0x8a, 0x04, 0x29, 0x46, 0x06,
  0x51, 0x18, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x1b, 0x8c, 0xe0, 0xff,
  0xff, 0xff, 0xff, 0x07, 0x40, 0x02, 0xa8, 0x0d, 0x86, 0xf0, 0xff, 0xff,
  0xff, 0xff, 0x03, 0x20, 0x01, 0xd5, 0x06, 0x62, 0xf8, 0xff, 0xff, 0xff,
  0xff, 0x01, 0x90, 0x00, 0x49, 0x18, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00,
  0x13, 0x82, 0x60, 0x42, 0x20, 0x4c, 0x08, 0x06, 0x00, 0x00, 0x00, 0x00,
  0x89, 0x20, 0x00, 0x00, 0x34, 0x00, 0x00, 0x00, 0x32, 0x22, 0x48, 0x09,
  0x20, 0x64, 0x85, 0x04, 0x93, 0x22, 0xa4, 0x84, 0x04, 0x93, 0x22, 0xe3,
  0x84, 0xa1, 0x90, 0x14, 0x12, 0x4c, 0x8a, 0x8c, 0x0b, 0x84, 0xa4, 0x4c,
  0x10, 0x7c, 0x23, 0x00, 0x25, 0x00, 0x14, 0xe6, 0x08, 0xc0, 0xa0, 0x0c,
  0x63, 0x0c, 0x22, 0x73, 0x04, 0x08, 0x99, 0x7b, 0x86, 0xcb, 0x9f, 0xb0,
  0x87, 0x90, 0xfc, 0x10, 0x68, 0x86, 0x85, 0x40, 0xc1, 0x99, 0x23, 0x08,
  0x4a, 0x81, 0xc6, 0x48, 0x89, 0xd4, 0x0c, 0xc0, 0x4d, 0xc3, 0xe5, 0x4f,
  0xd8, 0x43, 0x48, 0xfe, 0x4a, 0x48, 0x2b, 0x31, 0xf9, 0xc5, 0x6d, 0xa3,
  0x82, 0x31, 0xc6, 0x46, 0x51, 0xda, 0x40, 0x63, 0x8c, 0x31, 0xc6, 0x20,
  0x57, 0x16, 0x30, 0xd0, 0x18, 0x83, 0x31, 0xc6, 0x0c, 0xc1, 0xa3, 0x86,
  0xcb, 0x9f, 0xb0, 0x87, 0x90, 0x7c, 0x6e, 0xa3, 0x8a, 0x95, 0x98, 0x7c,
  0xe4, 0xb6, 0x11, 0x31, 0xc6, 0x18, 0x85, 0x90, 0x03, 0x0d, 0x9a, 0xb7,
  0x0d, 0x97, 0x3f, 0x61, 0x0f, 0x21, 0xf9, 0x2b, 0x21, 0x39, 0x54, 0x24,
  0x10, 0x69, 0xe4, 0x3c, 0x44, 0x34, 0x21, 0x84, 0x84, 0x84, 0x31, 0x0a,
  0x81, 0x06, 0xaa, 0x64, 0x0f, 0x1a, 0x2e, 0x7f, 0xc2, 0x1e, 0x42, 0xf2,
  0x57, 0x42, 0xda, 0x90, 0x66, 0x40, 0xc4, 0x18, 0xc3, 0x94, 0x02, 0x0d,
  0x3c, 0x12, 0xe5, 0x81, 0x80, 0x53, 0x5a, 0xe2, 0x9a, 0xa8, 0x88, 0x10,
  0x87, 0x0b, 0x0d, 0x63, 0x0c, 0xe2, 0x73, 0x04, 0xa0, 0x00, 0x00, 0x00,
  0x13, 0x14, 0x72, 0xc0, 0x87, 0x74, 0x60, 0x87, 0x36, 0x68, 0x87, 0x79,
  0x68, 0x03, 0x72, 0xc0, 0x87, 0x0d, 0xaf, 0x50, 0x0e, 0x6d, 0xd0, 0x0e,
  0x7a, 0x50, 0x0e, 0x6d, 0x00, 0x0f, 0x7a, 0x30, 0x07, 0x72, 0xa0, 0x07,
  0x73, 0x20, 0x07, 0x6d, 0x90, 0x0e, 0x71, 0xa0, 0x07, 0x73, 0x20, 0x07,
  0x6d, 0x90, 0x0e, 0x78, 0xa0, 0x07, 0x73, 0x20, 0x07, 0x6d, 0x90, 0x0e,
  0x71, 0x60, 0x07, 0x7a, 0x30, 0x07, 0x72, 0xd0, 0x06, 0xe9, 0x30, 0x07,
  0x72, 0xa0, 0x07, 0x73, 0x20, 0x07, 0x6d, 0x90, 0x0e, 0x76, 0x40, 0x07,
  0x7a, 0x60, 0x07, 0x74, 0xd0, 0x06, 0xe6, 0x10, 0x07, 0x76, 0xa0, 0x07,
  0x73, 0x20, 0x07, 0x6d, 0x60, 0x0e, 0x73, 0x20, 0x07, 0x7a, 0x30, 0x07,
  0x72, 0xd0, 0x06, 0xe6, 0x60, 0x07, 0x74, 0xa0, 0x07, 0x76, 0x40, 0x07,
  0x6d, 0xe0, 0x0e, 0x78, 0xa0, 0x07, 0x71, 0x60, 0x07, 0x7a, 0x30, 0x07,
  0x72, 0xa0, 0x07, 0x76, 0x40, 0x07, 0x43, 0x9e, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x86, 0x3c, 0x04, 0x10, 0x00,
  0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x79, 0x14, 0x20,
  0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0xf2, 0x38,
  0x40, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0xe4,
  0x81, 0x80, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60,
  0xc8, 0x33, 0x01, 0x01, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0xc0, 0x90, 0xc7, 0x02, 0x02, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x80, 0x21, 0x4f, 0x06, 0x04, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x59, 0x20, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00,
  0x32, 0x1e, 0x98, 0x14, 0x19, 0x11, 0x4c, 0x90, 0x8c, 0x09, 0x26, 0x47,
  0xc6, 0x04, 0x43, 0x1a, 0x25, 0x50, 0x0c, 0x23, 0x00, 0x85, 0x51, 0x08,
  0x45, 0x40, 0xbe, 0x00, 0x01, 0x01, 0x01, 0x11, 0xa8, 0xcf, 0x00, 0x00,
  0x79, 0x18, 0x00, 0x00, 0x38, 0x00, 0x00, 0x00, 0x1a, 0x03, 0x4c, 0x90,
  0x46, 0x02, 0x13, 0x44, 0x35, 0x18, 0x63, 0x0b, 0x73, 0x3b, 0x03, 0xb1,
  0x2b, 0x93, 0x9b, 0x4b, 0x7b, 0x73, 0x03, 0x99, 0x71, 0xb9, 0x01, 0x41,
  0xa1, 0x0b, 0x3b, 0x9b, 0x7b, 0x91, 0x2a, 0x62, 0x2a, 0x0a, 0x9a, 0x2a,
  0xfa, 0x9a, 0xb9, 0x81, 0x79, 0x31, 0x4b, 0x73, 0x0b, 0x63, 0x4b, 0xd9,
  0x10, 0x04, 0x13, 0x84, 0x01, 0x99, 0x20, 0x0c, 0xc9, 0x06, 0x61, 0x20,
  0x28, 0x8c, 0xcd, 0x6d, 0x18, 0x0c, 0x82, 0x98, 0x20, 0x0c, 0xca, 0x04,
  0xa1, 0x7b, 0x08, 0x4c, 0x10, 0x86, 0x65, 0x03, 0x82, 0x24, 0x0a, 0x32,
  0x0c, 0x0b, 0xb0, 0x21, 0x60, 0x36, 0x10, 0x00, 0xd0, 0x00, 0x13, 0x04,
  0x01, 0x20, 0xd1, 0x16, 0x96, 0xe6, 0x36, 0x41, 0xf0, 0x9c, 0x09, 0xc2,
  0xc0, 0x4c, 0x10, 0x86, 0x66, 0xc3, 0x30, 0x4d, 0xc3, 0x06, 0x02, 0x89,
  0x24, 0x6a, 0x43, 0xf1, 0x40, 0x80, 0x53, 0x55, 0x61, 0x63, 0xb3, 0x6b,
  0x73, 0x49, 0x23, 0x2b, 0x73, 0xa3, 0x9b, 0x12, 0x04, 0x55, 0xc8, 0xf0,
  0x5c, 0xec, 0xca, 0xe4, 0xe6, 0xd2, 0xde, 0xdc, 0xa6, 0x04, 0x44, 0x13,
  0x32, 0x3c, 0x17, 0xbb, 0x30, 0x36, 0xbb, 0x32, 0xb9, 0x29, 0x01, 0x51,
  0x87, 0x0c, 0xcf, 0x65, 0x0e, 0x2d, 0x8c, 0xac, 0x4c, 0xae, 0xe9, 0x8d,
  0xac, 0x8c, 0x6d, 0x4a, 0x60, 0x94, 0x21, 0xc3, 0x73, 0x91, 0x2b, 0x9b,
  0x7b, 0xab, 0x93, 0x1b, 0x2b, 0x9b, 0x9b, 0x12, 0x34, 0x75, 0xc8, 0xf0,
  0x5c, 0xca, 0xdc, 0xe8, 0xe4, 0xf2, 0xa0, 0xde, 0xd2, 0xdc, 0xe8, 0xe6,
  0xa6, 0x04, 0x15, 0x00, 0x79, 0x18, 0x00, 0x00, 0x4c, 0x00, 0x00, 0x00,
  0x33, 0x08, 0x80, 0x1c, 0xc4, 0xe1, 0x1c, 0x66, 0x14, 0x01, 0x3d, 0x88,
  0x43, 0x38, 0x84, 0xc3, 0x8c, 0x42, 0x80, 0x07, 0x79, 0x78, 0x07, 0x73,
  0x98, 0x71, 0x0c, 0xe6, 0x00, 0x0f, 0xed, 0x10, 0x0e, 0xf4, 0x80, 0x0e,
  0x33, 0x0c, 0x42, 0x1e, 0xc2, 0xc1, 0x1d, 0xce, 0xa1, 0x1c, 0x66, 0x30,
  0x05, 0x3d, 0x88, 0x43, 0x38, 0x84, 0x83, 0x1b, 0xcc, 0x03, 0x3d, 0xc8,
  0x43, 0x3d, 0x8c, 0x03, 0x3d, 0xcc, 0x78, 0x8c, 0x74, 0x70, 0x07, 0x7b,
  0x08, 0x07, 0x79, 0x48, 0x87, 0x70, 0x70, 0x07, 0x7a, 0x70, 0x03, 0x76,
  0x78, 0x87, 0x70, 0x20, 0x87, 0x19, 0xcc, 0x11, 0x0e, 0xec, 0x90, 0x0e,
  0xe1, 0x30, 0x0f, 0x6e, 0x30, 0x0f, 0xe3, 0xf0, 0x0e, 0xf0, 0x50, 0x0e,
  0x33, 0x10, 0xc4, 0x1d, 0xde, 0x21, 0x1c, 0xd8, 0x21, 0x1d, 0xc2, 0x61,
  0x1e, 0x66, 0x30, 0x89, 0x3b, 0xbc, 0x83, 0x3b, 0xd0, 0x43, 0x39, 0xb4,
  0x03, 0x3c, 0xbc, 0x83, 0x3c, 0x84, 0x03, 0x3b, 0xcc, 0xf0, 0x14, 0x76,
  0x60, 0x07, 0x7b, 0x68, 0x07, 0x37, 0x68, 0x87, 0x72, 0x68, 0x07, 0x37,
  0x80, 0x87, 0x70, 0x90, 0x87, 0x70, 0x60, 0x07, 0x76, 0x28, 0x07, 0x76,
  0xf8, 0x05, 0x76, 0x78, 0x87, 0x77, 0x80, 0x87, 0x5f, 0x08, 0x87, 0x71,
  0x18, 0x87, 0x72, 0x98, 0x87, 0x79, 0x98, 0x81, 0x2c, 0xee, 0xf0, 0x0e,
  0xee, 0xe0, 0x0e, 0xf5, 0xc0, 0x0e, 0xec, 0x30, 0x03, 0x62, 0xc8, 0xa1,
  0x1c, 0xe4, 0xa1, 0x1c, 0xcc, 0xa1, 0x1c, 0xe4, 0xa1, 0x1c, 0xdc, 0x61,
  0x1c, 0xca, 0x21, 0x1c, 0xc4, 0x81, 0x1d, 0xca, 0x61, 0x06, 0xd6, 0x90,
  0x43, 0x39, 0xc8, 0x43, 0x39, 0x98, 0x43, 0x39, 0xc8, 0x43, 0x39, 0xb8,
  0xc3, 0x38, 0x94, 0x43, 0x38, 0x88, 0x03, 0x3b, 0x94, 0xc3, 0x2f, 0xbc,
  0x83, 0x3c, 0xfc, 0x82, 0x3b, 0xd4, 0x03, 0x3b, 0xb0, 0xc3, 0x0c, 0xc4,
  0x21, 0x07, 0x7c, 0x70, 0x03, 0x7a, 0x28, 0x87, 0x76, 0x80, 0x87, 0x19,
  0xd1, 0x43, 0x0e, 0xf8, 0xe0, 0x06, 0xe4, 0x20, 0x0e, 0xe7, 0xe0, 0x06,
  0xf6, 0x10, 0x0e, 0xf2, 0xc0, 0x0e, 0xe1, 0x90, 0x0f, 0xef, 0x50, 0x0f,
  0xf4, 0x00, 0x00, 0x00, 0x71, 0x20, 0x00, 0x00, 0x24, 0x00, 0x00, 0x00,
  0x66, 0x40, 0x0d, 0x97, 0xef, 0x3c, 0x3e, 0xd0, 0x34, 0xce, 0x04, 0x4c,
  0x44, 0x08, 0x34, 0xc3, 0x42, 0x58, 0xc1, 0x36, 0x5c, 0xbe, 0xf3, 0xf8,
  0x42, 0x40, 0x15, 0x05, 0x11, 0x95, 0x0e, 0x30, 0x94, 0x84, 0x01, 0x08,
  0x98, 0x8f, 0xdc, 0xb6, 0x1d, 0x74, 0xc3, 0xe5, 0x3b, 0x8f, 0x2f, 0x44,
  0x04, 0x30, 0x11, 0x21, 0xd0, 0x0c, 0x0b, 0xf1, 0x45, 0x0e, 0xb3, 0x21,
  0xcd, 0x80, 0x34, 0x86, 0x09, 0x68, 0xc3, 0xe5, 0x3b, 0x8f, 0x2f, 0x44,
  0x04, 0x30, 0x11, 0x21, 0xd0, 0x0c, 0x0b, 0xf1, 0x45, 0x0e, 0x13, 0x12,
  0xc0, 0x63, 0x03, 0xd5, 0x70, 0xf9, 0xce, 0xe3, 0x4f, 0xc4, 0x35, 0x51,
  0x11, 0x51, 0x3a, 0xc0, 0xe0, 0x17, 0xb7, 0x6d, 0x04, 0xd6, 0x70, 0xf9,
  0xce, 0xe3, 0x4f, 0xc4, 0x35, 0x51, 0x11, 0xc1, 0x4e, 0x4e, 0x44, 0xf8,
  0xc5, 0x6d, 0x5b, 0x80, 0x34, 0x5c, 0xbe, 0xf3, 0xf8, 0xd3, 0x11, 0x11,
  0xc0, 0x20, 0x0e, 0x3e, 0x72, 0xdb, 0x06, 0x40, 0x30, 0x00, 0xd2, 0x00,
  0x61, 0x20, 0x00, 0x00, 0x9f, 0x00, 0x00, 0x00, 0x13, 0x04, 0x41, 0x2c,
  0x10, 0x00, 0x00, 0x00, 0x1a, 0x00, 0x00, 0x00, 0x34, 0x66, 0x00, 0xca,
  0x30, 0xa0, 0x60, 0x07, 0x8a, 0x30, 0xa0, 0x68, 0x07, 0x4a, 0x37, 0xa0,
  0x08, 0x81, 0x4a, 0x76, 0xa0, 0x34, 0xca, 0xae, 0x24, 0x21, 0x4a, 0x92,
  0xa0, 0x24, 0x11, 0xc8, 0x94, 0x47, 0x11, 0x50, 0x1a, 0x01, 0x20, 0x36,
  0x46, 0x70, 0xd2, 0xa5, 0x9c, 0x7e, 0x63, 0x04, 0x62, 0x1b, 0xb3, 0xf1,
  0x37, 0x46, 0x50, 0x9b, 0xbe, 0xec, 0x7b, 0x63, 0x04, 0x20, 0x08, 0x82,
  0xf0, 0x37, 0x46, 0xe0, 0xca, 0x38, 0x18, 0x7e, 0x63, 0x04, 0x6b, 0x1e,
  0xb6, 0xe3, 0x47, 0x75, 0x0e, 0x01, 0x5b, 0xe6, 0x10, 0x1a, 0x6d, 0x0e,
  0xc1, 0xda, 0xe6, 0x10, 0x2c, 0x8e, 0xf0, 0x1c, 0x04, 0x82, 0x28, 0x1e,
  0x00, 0x00, 0x00, 0x00, 0x23, 0x06, 0x08, 0x00, 0x82, 0x60, 0x90, 0x91,
  0x01, 0x15, 0x8c, 0x01, 0x33, 0x62, 0x70, 0x00, 0x20, 0x08, 0x06, 0x96,
  0x19, 0x68, 0x81, 0x31, 0x62, 0x60, 0x00, 0x20, 0x08, 0x06, 0x04, 0x1b,
  0x60, 0x63, 0x30, 0x62, 0x60, 0x00, 0x20, 0x08, 0x06, 0x44, 0x1b, 0x64,
  0x66, 0x30, 0x62, 0x70, 0x00, 0x20, 0x08, 0x06, 0x93, 0x1a, 0x58, 0x43,
  0x19, 0x8c, 0x26, 0x04, 0xc0, 0x88, 0x01, 0x02, 0x80, 0x20, 0x18, 0x28,
  0x6f, 0xd0, 0x05, 0x92, 0x34, 0x62, 0x70, 0x00, 0x20, 0x08, 0x06, 0x16,
  0x1b, 0x80, 0x41, 0xb0, 0x8c, 0x26, 0x10, 0xc1, 0x88, 0x01, 0x02, 0x80,
  0x20, 0x18, 0x28, 0x72, 0x00, 0x06, 0x41, 0x55, 0x8d, 0x18, 0x1c, 0x00,
  0x08, 0x82, 0x81, 0xf5, 0x06, 0x63, 0x10, 0x34, 0xa3, 0x09, 0x87, 0x30,
  0x62, 0x80, 0x00, 0x20, 0x08, 0x06, 0x4a, 0x1d, 0x8c, 0x41, 0x80, 0x61,
  0x23, 0x06, 0x07, 0x00, 0x82, 0x60, 0x60, 0xc9, 0x81, 0x19, 0x04, 0x4f,
  0x31, 0x71, 0x70, 0xc5, 0xc8, 0xc1, 0x8d, 0x18, 0x34, 0x00, 0x08, 0x82,
  0x81, 0x83, 0x07, 0x68, 0x90, 0xc4, 0x81, 0x10, 0xac, 0xc1, 0x1a, 0xac,
  0xc1, 0x1a, 0x8c, 0x26, 0x04, 0xc0, 0x68, 0x82, 0x10, 0x8c, 0x26, 0x0c,
  0x82, 0x0d, 0x9f, 0x7c, 0x6c, 0xf8, 0xe4, 0x63, 0x42, 0x00, 0x1f, 0x23,
  0xc0, 0x40, 0x3e, 0x26, 0x04, 0xf0, 0xa9, 0x85, 0x0f, 0x76, 0xc4, 0xa0,
  0x01, 0x40, 0x10, 0x0c, 0x1c, 0x51, 0x90, 0x83, 0x69, 0x0f, 0x82, 0xa5,
  0x0e, 0xea, 0xa0, 0x0e, 0xea, 0x60, 0x34, 0x21, 0x00, 0x46, 0x13, 0x84,
  0x60, 0x34, 0x61, 0x10, 0x6c, 0x48, 0x03, 0xf9, 0xd8, 0x90, 0x06, 0xf2,
  0x31, 0x21, 0x80, 0x8f, 0x11, 0x6a, 0x20, 0x1f, 0x13, 0x02, 0xf8, 0x14,
  0x65, 0x0a, 0x3b, 0x62, 0xd0, 0x00, 0x20, 0x08, 0x06, 0x0e, 0x2b, 0xf0,
  0x41, 0x57, 0x0a, 0x56, 0xf0, 0x07, 0x7f, 0xf0, 0x07, 0x7f, 0x30, 0x9a,
  0x10, 0x00, 0xa3, 0x09, 0x42, 0x30, 0x9a, 0x30, 0x08, 0x36, 0xcc, 0x81,
  0x7c, 0x6c, 0x98, 0x03, 0xf9, 0x98, 0x10, 0xc0, 0xc7, 0x08, 0x3a, 0x90,
  0x8f, 0x09, 0x01, 0x7c, 0x46, 0x0c, 0x1a, 0x00, 0x04, 0xc1, 0xc0, 0xa9,
  0x85, 0x52, 0x30, 0x03, 0x57, 0xa0, 0x14, 0x54, 0x40, 0x05, 0x54, 0x40,
  0x85, 0xd1, 0x84, 0x00, 0x18, 0x4d, 0x10, 0x82, 0xd1, 0x84, 0x41, 0xb0,
  0x81, 0x0f, 0xe4, 0x63, 0x03, 0x1f, 0xc8, 0xc7, 0x84, 0x00, 0x3e, 0x46,
  0xf4, 0x81, 0x7c, 0x4c, 0x08, 0xe0, 0x33, 0x62, 0xe0, 0x00, 0x20, 0x08,
  0x06, 0x50, 0x2f, 0xc0, 0x02, 0x1b, 0xa0, 0xc1, 0x19, 0xc4, 0x82, 0xe7,
  0x79, 0x1e, 0x29, 0x8c, 0x18, 0x38, 0x00, 0x08, 0x82, 0x01, 0xd4, 0x0b,
  0xb0, 0xc0, 0x06, 0xdd, 0x19, 0xc4, 0x02, 0x45, 0x51, 0x14, 0x29, 0x8c,
  0x18, 0x38, 0x00, 0x08, 0x82, 0x01, 0xd4, 0x0b, 0xb0, 0xc0, 0x06, 0x68,
  0x30, 0xc5, 0x82, 0xa2, 0x28, 0x0a, 0x29, 0x8c, 0x18, 0x38, 0x00, 0x08,
  0x82, 0x01, 0xd4, 0x0b, 0xb0, 0xc0, 0x06, 0xdd, 0x14, 0x0b, 0x41, 0x10,
  0x04, 0xa4, 0x60, 0xd9, 0x18, 0xc0, 0xc7, 0x02, 0x08, 0x3e, 0x16, 0x80,
  0x41, 0x7c, 0x2c, 0x48, 0xe0, 0x63, 0x01, 0x16, 0x1f, 0x0b, 0x9e, 0xf8,
  0x58, 0x70, 0xc4, 0xc7, 0xc4, 0x80, 0x0d, 0xe0, 0x63, 0x41, 0x06, 0x1f,
  0x0b, 0xce, 0x20, 0x3e, 0x16, 0x48, 0xf0, 0xb1, 0xe0, 0x8b, 0x8f, 0x05,
  0x56, 0x7c, 0x2c, 0x70, 0xe2, 0x63, 0xc8, 0x2a, 0xc8, 0xc7, 0x82, 0x56,
  0x80, 0x8f, 0x0d, 0xac, 0x20, 0x1f, 0x0b, 0x5e, 0x01, 0x3e, 0x23, 0x06,
  0x0e, 0x00, 0x82, 0x60, 0x00, 0xbd, 0x83, 0x38, 0xec, 0x81, 0x29, 0x94,
  0xc2, 0x38, 0x0c, 0xc1, 0x30, 0xd8, 0x02, 0x02, 0x00, 0x00, 0x00, 0x00
};