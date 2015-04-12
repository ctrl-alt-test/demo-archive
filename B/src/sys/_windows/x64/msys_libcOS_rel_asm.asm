;--------------------------------------------------------------------------;
; iq . 2003/2008 . code for 64 kb intros by RGBA                           ;
;--------------------------------------------------------------------------;


public msys_cosf 
public msys_sinf 

.code

msys_cosf proc public
   movss dword ptr [rsp-8], xmm0
   fld dword ptr [rsp-8]
   fcos
   fstp dword ptr [rsp-8]
   movss xmm0, dword ptr [rsp-8]
   ret
msys_cosf endp

msys_sinf proc public
   movss dword ptr [rsp-8], xmm0
   fld dword ptr [rsp-8]
   fsin
   fstp dword ptr [rsp-8]
   movss xmm0, dword ptr [rsp-8]
   ret
msys_sinf endp

msys_sqrtf proc public
   sqrtss xmm0, xmm0

   ;movss dword ptr [rsp-8], xmm0
   ;fld dword ptr [rsp-8]
   ;fsqrt
   ;fstp dword ptr [rsp-8]
   ;movss xmm0, dword ptr [rsp-8]
   ret
msys_sqrtf endp

END
