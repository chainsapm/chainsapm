#include "stdafx.h"
#include <cor.h>
#include <corprof.h>

#if X64

#else

// @@TODO: Change the declaration to match the function you're implementing
void __declspec(naked) __stdcall FunctionLeave2_x86(FunctionID id, UINT_PTR clientData, COR_PRF_FRAME_INFO frame, COR_PRF_FUNCTION_ARGUMENT_RANGE* retvalRange)
{
    __asm
    {
#ifdef DEBUG
        // Set up EBP Frame (easy debugging)
        // Turned off in release mode to save cycles
        push ebp
        mov ebp, esp
#endif

        // Make space for locals.
        sub esp, __LOCAL_SIZE

        // Save all registers.
        //
        // Technically you need only save what you use, but if you're working in C
        // rather than assembly that's hard to predict. Saving them all is safer.
        // If you really need to save the minimal set, code the whole darn thing in 
        // assembly so you can control the register usage.
        //
        // Pushing ESP again is redundant (and in DEBUG pushing EBP is too), but using
        // pushad is cleaner and saves some clock time over pushing individual registers.
        // You can push the individual registers if you want; doing so will
        // save a couple of DWORDs of stack space, perhaps at the expense of
        // a clock cycle.
        pushad
        
        // Check if there's anything on the FP stack.
        //
        // Again technically you need only save what you use. You might think that
        // FP regs are not commonly used in the kind of code you'd write in these,
        // but there are common cases that might interfere. For example, in the 8.0 MS CRT, 
        // memcpy clears the FP stack.
        //
        // @@TODO: In CLR versions 1.x and 2.0, everything from here to NoSaveFPReg
        // is only strictly necessary for FunctionLeave and FunctionLeave2.
		// Of course that may change in future releases, so use this code for all of your
		// enter/leave function hooks if you want to avoid breaking.
        fstsw   ax
        and     ax, 3800h		// Check the top-of-fp-stack bits
        cmp     ax, 0			// If non-zero, we have something to save
        jnz     SaveFPReg
        push    0				// otherwise, mark that there is no float value
        jmp     NoSaveFPReg
SaveFPReg:
        sub     esp, 8			// Make room for the FP value
        fstp    qword ptr [esp] // Copy the FP value to the buffer as a double
        push    1				// mark that a float value is present
NoSaveFPReg:
    }

    // @@TODO: Do your stuff here

    __asm
    {
        // Now see if we have to restore any floating point registers
        // @@TODO: In CLR versions 1.x and 2.0, everything from here to 
        // RestoreFPRegsDone is only strictly necessary for FunctionLeave and FunctionLeave2
		// Of course that may change in future releases, so use this code for all of your
		// enter/leave function hooks if you want to avoid breaking.
		cmp     [esp], 0			// Check the flag
        jz      NoRestoreFPRegs		// If zero, no FP regs
RestoreFPRegs:
        fld     qword ptr [esp + 4]	// Restore FP regs
        add    esp, 12				// Move ESP past the storage space
        jmp   RestoreFPRegsDone
NoRestoreFPRegs:
        add     esp, 4				// Move ESP past the flag
RestoreFPRegsDone:

        // Restore other registers
        popad

        // Pop off locals
        add esp, __LOCAL_SIZE

#ifdef DEBUG
        // Restore EBP
        // Turned off in release mode to save cycles
        pop ebp
#endif

        // stdcall: Callee cleans up args from stack on return
        // @@TODO: Change this line to match the parameters to your function!
        ret SIZE id + SIZE clientData + SIZE frame + SIZE retvalRange
    }
}
#endif