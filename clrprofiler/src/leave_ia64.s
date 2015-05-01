// @@TODO: Change to match the function you're implementing
.global FunctionLeave2_CPP_Helper
.type FunctionLeave2_CPP_Helper,@function

// @@TODO: Change to match the function you're implementing
// typedef void FunctionLeave2_Wrapper(
//          r32 =  FunctionID funcId, 
//          r33 =  UINT_PTR clientData, 
//          r34 =  COR_PRF_FRAME_INFO func, 
//          r35 =  COR_PRF_FUNCTION_ARGUMENT_RANGE *retvalRange);
.text; .global FunctionLeave2_Wrapper; .proc FunctionLeave2_Wrapper; FunctionLeave2_Wrapper::
        .prologue
        .regstk                4, 7, 4, 0
        .save ar.pfs, loc0
        alloc   loc0 = ar.pfs, 4, 7, 4, 0       // in, loc, out, rotatlocg
        .fframe 0x80
        add     sp = -0x80, sp
        .save rp, loc1
        mov     loc1 = rp   
        .body

        // propagate arguments to callee
	    // @@TODO: Change this to match the number of args for your function
        mov     out0 = in0
        mov     out1 = in1
        mov     out2 = in2
        mov     out3 = in3

        // save volatile integer return registers
        mov     loc2 = ret0
        mov     loc3 = ret1
        mov     loc4 = ret2
        mov     loc5 = ret3

        // save volatile floating point registers
        mov     loc6 = sp ;;
        stf.spill.nta   [loc6] = farg0, 16 ;;
        stf.spill.nta   [loc6] = farg1, 16 ;;
        stf.spill.nta   [loc6] = farg2, 16 ;;
        stf.spill.nta   [loc6] = farg3, 16 ;;
        stf.spill.nta   [loc6] = farg4, 16 ;;
        stf.spill.nta   [loc6] = farg5, 16 ;;
        stf.spill.nta   [loc6] = farg6, 16 ;;
        stf.spill.nta   [loc6] = farg7, 16

        // call your c++ FunctionLeave2 helper
        // @@TODO: Change to match the function you're implementing
        br.call.sptk.few rp = FunctionLeave2_CPP_Helper

        // restore volatile floating point registers
        mov     loc6 = sp ;;
        ldf.fill.nta    farg0 = [loc6], 16 ;;
        ldf.fill.nta    farg1 = [loc6], 16 ;;
        ldf.fill.nta    farg2 = [loc6], 16 ;;
        ldf.fill.nta    farg3 = [loc6], 16 ;;
        ldf.fill.nta    farg4 = [loc6], 16 ;;
        ldf.fill.nta    farg5 = [loc6], 16 ;;
        ldf.fill.nta    farg6 = [loc6], 16 ;;
        ldf.fill.nta    farg7 = [loc6], 16

        // restore volatile integer return registers
        mov     ret0 = loc2
        mov     ret1 = loc3
        mov     ret2 = loc4
        mov     ret3 = loc5

        .restore sp
        add     sp     = 0x80, sp
        mov     ar.pfs = loc0
        mov     rp     = loc1
        
        br.ret.sptk.few rp
.endp FunctionLeave2_Wrapper;
