
; Constants which are used in the following assembly code.
; @@TODO: Adjust as per comments below.
SIZEOF_OUTGOING_ARGUMENT_HOMES          equ 8h*4h
NUMBER_XMM_SAVES                        equ 6h
SIZEOF_STACK_ALLOC                      equ 10h*NUMBER_XMM_SAVES + SIZEOF_OUTGOING_ARGUMENT_HOMES
OFFSETOF_XMM_SAVE                       equ SIZEOF_OUTGOING_ARGUMENT_HOMES

; @@TODO: Change to match the function you're implementing
extern FunctionEnter2_CPP_Helper_x64:proc

; @@TODO: Change to match the function you're implementing
;typedef void FunctionEnter2_Wrapper(
;         rcx =  FunctionID funcId, 
;         rdx =  UINT_PTR clientData, 
;         r8  =  COR_PRF_FRAME_INFO func, 
;         r9  =  COR_PRF_FUNCTION_ARGUMENT_RANGE *retvalRange);
_TEXT segment para 'CODE'

        align   16

        ; @@TODO: Change to match the function you're implementing
        public  FunctionEnter2_Wrapper_x64

; @@TODO: Change to match the function you're implementing
FunctionEnter2_Wrapper_x64    proc    frame

        ; save integer return register
        push    rax
        .allocstack 8

SaveVolIntRegs:
        ; @@TODO: In CLR version 2.0, everything from here to DoneSaveVolIntRegs
        ; is not strictly needed.
        ; Of course that may change in future releases, so use this code if you 
        ; want to avoid breaking.
        push    rcx
        .allocstack 8

        push    rdx
        .allocstack 8

        push    r8
        .allocstack 8

        push    r9
        .allocstack 8

        push    r10
        .allocstack 8

        push    r11
        .allocstack 8
DoneSaveVolIntRegs:

        
        ; reserve space for floating point registers to be saved
        ; @@TODO: In CLR version 2.0, strictly speaking you only need to save the
        ; return register, so you can use 30h in the next two lines instead of 80h.
        ; Of course, this may all change in future releases, so use this code as is
        ; if you want to avoid breaking.
        sub     rsp, SIZEOF_STACK_ALLOC
        .allocstack SIZEOF_STACK_ALLOC

        ; save floating point return register
        movdqa  [rsp + OFFSETOF_XMM_SAVE + 0h], xmm0
        .savexmm128 xmm0, OFFSETOF_XMM_SAVE + 0h
    
SaveVolFPRegs:
        ; save volatile floating point registers
        ; @@TODO: In CLR version 2.0, strictly speaking you only need to save the
        ; return register, so you can omit everything from here to DoneSaveVolFPRegs.
        ; Of course that may change in future releases, so use this code as is
        ; if you want to avoid breaking.
        movdqa  [rsp + OFFSETOF_XMM_SAVE + 10h], xmm1
        .savexmm128 xmm1, OFFSETOF_XMM_SAVE + 10h
    
        movdqa  [rsp + OFFSETOF_XMM_SAVE + 20h], xmm2
        .savexmm128 xmm2, OFFSETOF_XMM_SAVE + 20h
    
        movdqa  [rsp + OFFSETOF_XMM_SAVE + 30h], xmm3
        .savexmm128 xmm3, OFFSETOF_XMM_SAVE + 30h
    
        movdqa  [rsp + OFFSETOF_XMM_SAVE + 40h], xmm4
        .savexmm128 xmm4, OFFSETOF_XMM_SAVE + 40h
    
        movdqa  [rsp + OFFSETOF_XMM_SAVE + 50h], xmm5
        .savexmm128 xmm5, OFFSETOF_XMM_SAVE + 50h
DoneSaveVolFPRegs:
    
        .endprolog


        ; call FuntionLeave2 c++ helper
        ; @@TODO: Change to match the function you're implementing
        call                    FunctionEnter2_CPP_Helper_x64

        ; restore floating-point return register
        movdqa                  xmm0, [rsp + OFFSETOF_XMM_SAVE + 0h]

RestoreVolFPRegs:
        ; restore volatile floating point registers
        ; @@TODO: In CLR version 2.0, strictly speaking you only need to save the
        ; return register, so you can omit everything from here to DoneRestoreVolFPRegs.
        ; Of course that may change in future releases, so use this code as is
        ; you want to avoid breaking.
        movdqa                  xmm1, [rsp + OFFSETOF_XMM_SAVE + 10h]
        movdqa                  xmm2, [rsp + OFFSETOF_XMM_SAVE + 20h]
        movdqa                  xmm3, [rsp + OFFSETOF_XMM_SAVE + 30h]
        movdqa                  xmm4, [rsp + OFFSETOF_XMM_SAVE + 40h]
        movdqa                  xmm5, [rsp + OFFSETOF_XMM_SAVE + 50h]
DoneRestoreVolFPRegs:

        ; restore the stack pointer 
        ; @@TODO: In CLR version 2.0, strictly speaking you only need to save the
        ; return register, so you can use 30h in this line instead of 80h.
        ; Of course that may change in future releases, so use this code as is
        ; you want to avoid breaking.
        add                     rsp, SIZEOF_STACK_ALLOC

        ; restore volatile integer registers
RestoreVolIntRegs:
        ; @@TODO: In CLR version 2.0, everything from here to DoneRestoreVolIntRegs
        ; is not strictly needed.
        ; Of course that may change in future releases, so use this code if you want 
        ; to avoid breaking.
        pop                     r11
        pop                     r10
        pop                     r9
        pop                     r8
        pop                     rdx
        pop                     rcx
DoneRestoreVolIntRegs:

        ; restore integer return register
        pop                     rax

        ; return
        ret

; @@TODO: Change to match the function you're implementing
FunctionEnter2_Wrapper_x64    endp

_TEXT ends

end
