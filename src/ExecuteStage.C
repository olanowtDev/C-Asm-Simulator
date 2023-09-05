#include <string>
#include <cstdint>
#include "Debug.h"
#include "RegisterFile.h"
#include "PipeRegField.h"
#include "PipeReg.h"
#include "E.h"
#include "F.h"
#include "D.h"
#include "M.h"
#include "W.h"
#include "Memory.h"
#include "ConditionCodes.h"
#include "Stage.h"
#include "ExecuteStage.h"
#include "Status.h"
#include "Debug.h"
#include "Instructions.h"
#include "Tools.h"

bool ExecuteStage::doClockLow(PipeReg ** pregs)
{
    PipeReg * ereg = pregs[EREG];
    PipeReg * mreg = pregs[MREG];
    PipeReg * wreg = pregs[WREG];
    uint64_t stat = ereg->get(E_STAT);
    uint64_t icode = ereg->get(E_ICODE);
    uint64_t ifun = ereg->get(E_IFUN);
    e_Cnd = ExecuteStage::cond(icode, ifun);
    e_valE = ExecuteStage::ALU(ereg, wreg, icode);
    uint64_t valA = ereg->get(E_VALA);
    e_dstE = ExecuteStage::get_e_dstE(ereg, e_Cnd);
    uint64_t dstM = ereg->get(E_DSTM);
    M_bubble = calculateControlSignals(wreg);


    setMInput(mreg, stat, icode, e_Cnd, e_valE, valA, e_dstE, dstM);

    return false;
}

void ExecuteStage::doClockHigh(PipeReg ** pregs)
{
    PipeReg * ereg = pregs[EREG]; 
    PipeReg * mreg = pregs[MREG];
    ereg->normal();
    if (M_bubble)
    {
        ((M *)mreg)->bubble();
    }
    else
    {
        mreg->normal();
    }
}

void ExecuteStage::setMInput(PipeReg * mreg, uint64_t stat, uint64_t icode, uint64_t Cnd,
                             uint64_t valE, uint64_t valA, uint64_t dstE,
                             uint64_t dstM)
{
    mreg->set(M_STAT, stat);
    mreg->set(M_ICODE, icode);
    mreg->set(M_CND, Cnd);
    mreg->set(M_VALE, valE);
    mreg->set(M_VALA, valA);
    mreg->set(M_DSTE, dstE);
    mreg->set(M_DSTM, dstM);
}


uint64_t ExecuteStage::aluA(PipeReg * ereg)
{
    uint64_t icode = ereg -> get(E_ICODE);

    if (icode == IRRMOVQ || icode == IOPQ)
    {
        return ereg -> get(E_VALA);
    }

    else if (icode == IIRMOVQ || icode == IRMMOVQ || icode == IMRMOVQ)
    {
        return ereg -> get(E_VALC);
    }

    else if (icode == ICALL || icode == IPUSHQ)
    {
        return -8;
    }

    else if (icode == IRET || icode == IPOPQ)
    {
        return 8;
    }

    else 
    {
        return 0;
    }
}


uint64_t ExecuteStage::aluB(PipeReg * ereg)
{
    uint64_t icode = ereg -> get(E_ICODE);

    if (icode == IRMMOVQ || icode == IMRMOVQ || icode == IOPQ || icode == ICALL || icode == IPUSHQ || icode == IRET || icode == IPOPQ)
    {
        return ereg -> get(E_VALB);
    }
    
    else if (icode == IRRMOVQ || icode == IIRMOVQ)
    {
        return 0;
    }

    else 
    {
        return 0;
    }
}


uint64_t ExecuteStage::alufun(PipeReg * ereg)
{
    uint64_t icode = ereg -> get(E_ICODE);

    if (icode == IOPQ)
    {
        return ereg -> get(E_IFUN);
    }
    else
    {
        return ADDQ;
    }
}


bool ExecuteStage::set_cc(PipeReg * ereg, PipeReg * wreg)
{
    uint64_t icode = ereg -> get(E_ICODE);
    uint64_t W_stat = wreg->get(W_STAT);

    if ((icode == IOPQ) && (m_stat == SAOK)  && (wreg->get(W_STAT) == SAOK))
    {
        return true;
    }
    else 
    {
        return false;
    }
}


uint64_t ExecuteStage::get_e_dstE(PipeReg * ereg, bool Cnd)
{
    uint64_t icode = ereg -> get(E_ICODE);

    if (icode == IRRMOVQ && !(Cnd))
    {
        return RNONE;
    }
    else
    {
        return ereg -> get(E_DSTE);
    }
}


uint64_t ExecuteStage::ALU(PipeReg * ereg, PipeReg * wreg,  uint64_t icode)
{
    int64_t a = aluA(ereg);
    int64_t b = aluB(ereg);
    uint64_t function = alufun(ereg);
    int64_t computed_valE = 0;

    if (function == ADDQ)
    {
        computed_valE = a + b;
    }

    if (function == SUBQ)
    {
        computed_valE = b - a;
    }

    if (function == XORQ)
    {
        computed_valE = a ^ b;
    }

    if (function == ANDQ)
    {
        computed_valE = a & b;
    }

    if (set_cc(ereg, wreg))
    {
        ExecuteStage::CC(ereg, icode, function, computed_valE, a, b);
    }
    
    return computed_valE;
}

void ExecuteStage::CC(PipeReg * ereg, uint64_t icode, uint64_t function, int64_t valE, int64_t aluA, int64_t aluB)
{
    bool cc_error;

    if (icode == IOPQ)
    {
        if (function == ADDQ)
        {
            cc ->setConditionCode(Tools::sign(aluA) == Tools::sign(aluB) && Tools::sign(valE) != (Tools::sign(aluA)), OF, cc_error);
        }

        if (function == SUBQ)
        {
            cc -> setConditionCode((Tools::sign(aluA) != Tools::sign(aluB) && Tools::sign(valE) == (Tools::sign(aluA))), OF, cc_error);
        }

        if (function == XORQ)
        {
            cc -> setConditionCode(0, OF, cc_error);
        }

        if (function == ANDQ)
        {
            cc -> setConditionCode(0, OF, cc_error);
        }

        if (valE == 0)
        {
            cc -> setConditionCode(1, ZF, cc_error);
        }
        else
        {
            cc -> setConditionCode(0, ZF, cc_error);
        }

        if (valE < 0)
        {
            cc -> setConditionCode(1, SF, cc_error);
        }
        else
        {
            cc -> setConditionCode(0, SF, cc_error);
        }
    }
}    

uint64_t ExecuteStage::cond(uint64_t icode, uint64_t ifun)
{
    uint64_t e_Cnd;
    bool error;
    int32_t of = cc->getConditionCode(OF, error);
    int32_t sf = cc->getConditionCode(SF, error);
    int32_t zf = cc->getConditionCode(ZF, error);
    if (icode != IJXX && icode != ICMOVXX)
    {
        return 0;
    }
    if ((icode == IJXX || icode == IRRMOVQ) && ifun == UNCOND)
    {
        e_Cnd = 1;
    }
    else if ((icode == IJXX || icode == IRRMOVQ) &&ifun == LESSEQ)
    {
        e_Cnd = (sf ^ of) | zf;
    }
    else if ((icode == IJXX || icode == IRRMOVQ) &&ifun == LESS)
    {
        e_Cnd = sf ^ of;
    }
    else if ((icode == IJXX || icode == IRRMOVQ) &&ifun == EQUAL)
    {
        e_Cnd = zf;
    }
    else if ((icode == IJXX || icode == IRRMOVQ) &&ifun == NOTEQUAL)
    {
        e_Cnd = !zf;
    }
    else if ((icode == IJXX || icode == IRRMOVQ) &&ifun == GREATER)
    {
        e_Cnd = !(sf ^ of) & !zf;
    }
    else if ((icode == IJXX || icode == IRRMOVQ) &&ifun == GREATEREQ)
    {
        e_Cnd = !(sf ^ of);
    }
    else 
    {
        return 0;
    }

    return e_Cnd;
}


bool ExecuteStage::calculateControlSignals(PipeReg * wreg)
{
    return (m_stat != SAOK || wreg->get(W_STAT) != SAOK);
}







