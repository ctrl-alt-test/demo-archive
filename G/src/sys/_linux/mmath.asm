;;--------------------------------------------------------------------------;;
;; iq . 2003 . code for the Paradise 64 kb intro by RGBA                    ;;
;;--------------------------------------------------------------------------;;

%ifdef NDEBUG
%define ISFASTCALL 1
%else
%define ISFASTCALL 0
%endif


;[GLOBAL __ftol]

%if ISFASTCALL
;[GLOBAL @mlog2@4]
[GLOBAL @mfmod@8]
;[GLOBAL @mexp@4]
;[GLOBAL @mifloor@4]		; fastcall
[GLOBAL @mpow@8]
%else
;[GLOBAL _mlog2]
[GLOBAL _mfmod]
;[GLOBAL _mexp]
;[GLOBAL _mifloor]		; cedcl
[GLOBAL _mpow]
;[GLOBAL _masin]
;[GLOBAL _mfloor]
;[GLOBAL _mceil]
%endif


;[SEGMENT .bss USE32]

[SEGMENT .data USE32]


;magic dd 0x59c00000
opc1    dw  0x043f      ; floor
;opc2   dw  0x083f      ; ceil

[SEGMENT .text USE32]


;__ftol:
;    ;fistp     qword [esp-8]
;    fadd      dword [magic]
;    fstp      qword [esp-8]
;    mov       eax, dword [esp-8]
;    mov       edx, dword [esp-4]
;    ret


%if ISFASTCALL

;@mlog2@4:
;    fld     dword [esp+4]
;    fld1
;    fxch    st1
;    fyl2x
;    ret		4

@mfmod@8:
    fld     dword [esp+8]
    fld     dword [esp+4]
    fprem
    fxch    st1
    fstp    st0
    ret		8

;@mexp@4:
;    fld     dword [esp+4]
;    fldl2e
;    fmulp   st1, st0
;    fld1
;    fld     st1
;    fprem
;    f2xm1
;    faddp   st1, st0
;    fscale
;    fxch    st1
;    fstp    st0
;    ret     4

@mpow@8:
    fld     dword [esp+8]
    fld     dword [esp+4]
    fyl2x
    fld1
    fld     st1
    fprem
    f2xm1
    faddp   st1, st0
    fscale
    fxch
    fstp    st0
    ret     8

;@mifloor@4:
;    fstcw   word  [esp-16]
;    fld     dword [esp+4]
;    fldcw   word  [opc1]
;    fistp   dword [esp-20]
;    fldcw   word  [esp-16]
;    mov     eax,  [esp-20]
;    ret    4

;--------------------------------------------
%else

;_mlog2:
;    fld     dword [esp+4]
;    fld1
;    fxch    st1
;    fyl2x
;    ret

_mfmod:
    fld     dword [esp+8]
    fld     dword [esp+4]
    fprem
    fxch    st1
    fstp    st0
    ret

;_mexp:
;    fld     dword [esp+4]
;    fldl2e
;    fmulp   st1, st0
;    fld1
;    fld     st1
;    fprem
;    f2xm1
;    faddp   st1, st0
;    fscale
;    fxch    st1
;    fstp    st0
;    ret

_mpow:
    fld     dword [esp+8]
    fld     dword [esp+4]
    fyl2x
    fld1
    fld     st1
    fprem
    f2xm1
    faddp   st1, st0
    fscale
    fxch
    fstp    st0
    ret

;_mifloor:
;    fstcw   word  [esp-16]
;    fld     dword [esp+4]
;    fldcw   word  [opc1]
;    fistp   dword [esp-20]
;    fldcw   word  [esp-16]
;    mov     eax,  [esp-20]
;    ret


%endif

;_masin:
;    fld     dword [esp+4]
;    fld1
;    fadd    st0, st1
;    fld1
;    fsub    st0, st2
;    fmulp   st1, st0
;    fsqrt
;    fpatan
;    ret
;_mfloor:
;    fstcw   word  [esp-16]
;    fld     dword [esp+4]
;    fldcw   word  [opc1]
;    frndint
;    fldcw   word  [esp-16]
;    ret


