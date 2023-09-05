#include <string>
#include <cstdint>
#include "ConditionCodes.h"
#include "Debug.h"
#include "RegisterFile.h"
#include "PipeRegField.h"
#include "PipeReg.h"
#include "D.h"
#include "E.h"
#include "M.h"
#include "W.h"
#include "Memory.h"
#include "Stage.h"
#include "DecodeStage.h"
#include "Status.h"
#include "Debug.h"
#include "Instructions.h"
#include "ExecuteStage.h"
#include "MemoryStage.h"


bool DecodeStage::doClockLow(PipeReg ** pregs)
{
    PipeReg * dreg = pregs[DREG];
    PipeReg * ereg = pregs[EREG];
    PipeReg * mreg = pregs[MREG];
    PipeReg * wreg = pregs[WREG];
    uint64_t stat = dreg->get(D_STAT);
    uint64_t icode = dreg->get(D_ICODE);
    uint64_t ifun = dreg->get(D_IFUN);
    d_srcA = getSrcA(dreg);
    d_srcB = getSrcB(dreg);
    uint64_t valC = dreg->get(D_VALC);
    uint64_t valA = getValA(d_srcA, dreg, mreg, wreg);
    uint64_t valB = getValB(d_srcB, dreg, mreg, wreg);
    uint64_t dstE = getDstE(dreg, icode);
    uint64_t dstM = getDstM(dreg);
    e_bubble = calculateControlSignals(ereg);
    setEInput(ereg, stat, icode, ifun, valC, valA, valB, dstE, dstM, d_srcA, d_srcB);
    return false;

}

void DecodeStage::doClockHigh(PipeReg ** pregs)
{
    PipeReg * dreg = pregs[DREG]; 
    PipeReg * ereg = pregs[EREG];
    if (e_bubble)
    {
        ((E *) ereg)->bubble();
    }
    else
    {
        ereg->normal();
    }
}

void DecodeStage::setEInput(PipeReg * ereg, uint64_t stat, uint64_t icode, uint64_t ifun,
                             uint64_t valC, uint64_t valA, uint64_t valB, uint64_t dstE,
                             uint64_t dstM, uint64_t srcA, uint64_t srcB)
{
    ereg->set(E_STAT, stat);
    ereg->set(E_ICODE, icode);
    ereg->set(E_IFUN, ifun);
    ereg->set(E_VALC, valC);
    ereg->set(E_VALA, valA);
    ereg->set(E_VALB, valB);
    ereg->set(E_DSTE, dstE);
    ereg->set(E_DSTM, dstM);
    ereg->set(E_SRCA, srcA);
    ereg->set(E_SRCB, srcB);
    
}

uint64_t DecodeStage::getSrcA(PipeReg * dreg)
{
    uint64_t icode = dreg->get(D_ICODE);
    if (icode == IRRMOVQ || icode == IRMMOVQ || icode == IOPQ || icode == IPUSHQ)
    {
        return dreg->get(D_RA);
    }
    else if (icode == IPOPQ || icode == IRET)
    {
        return RSP;
    }
    else
    {
        return RNONE;
    }
}

uint64_t DecodeStage::getSrcB(PipeReg * dreg)
{
    uint64_t icode = dreg->get(D_ICODE);

    if (icode == IOPQ || icode == IRMMOVQ || icode == IMRMOVQ)
    {
        return dreg->get(D_RB);
    }
    else if (icode == IPUSHQ || icode == IPOPQ || icode == ICALL || icode == IRET)
    {
        return RSP;
    }
    else
    {
        return RNONE;
    }
}

uint64_t DecodeStage::getDstE(PipeReg * dreg, uint64_t icode)
{
    if (icode == IRRMOVQ || icode == IOPQ || icode == IIRMOVQ)
    {     
        return dreg->get(D_RB);
    }
    else if (icode == IPUSHQ || icode == IPOPQ || icode == ICALL || icode == IRET)
    {
        return RSP;
    }
    else
    {
        return RNONE;
    }
}

uint64_t DecodeStage::getDstM(PipeReg * dreg)
{
    uint64_t icode = dreg->get(D_ICODE);
    if (icode == IMRMOVQ || icode == IPOPQ)
    {
        return dreg->get(D_RA);
    }
    else
    {
        return RNONE;
    }
}

uint64_t DecodeStage::getValA(uint64_t srcA, PipeReg * dreg, PipeReg * mreg, PipeReg * wreg)
{
    bool error;
    uint64_t icode = dreg->get(D_ICODE);
    if (icode == ICALL || icode == IJXX)
    {
        return dreg->get(D_VALP);
    }
    else if (srcA == RNONE)
    {
        return 0;
    }    
    else if (srcA == e_dstE)
    {
        return e_valE;
    }
    else if (srcA == mreg -> get(M_DSTM))
    {
        return m_valM;
    }
    else if (srcA == mreg -> get(M_DSTE))
    {
        return mreg -> get(M_VALE);
    }
    else if (srcA == wreg -> get(W_DSTM))
    {
        return wreg -> get(W_VALM);
    }
    else if (srcA == wreg -> get(W_DSTE))
    {
        return wreg -> get(W_VALE);
    }
    else 
    {
        return rf->readRegister(DecodeStage::getSrcA(dreg), error);
    }
}

uint64_t DecodeStage::getValB(uint64_t srcB, PipeReg * dreg, PipeReg * mreg, PipeReg * wreg)
{
    bool error;
    if (srcB == RNONE)
    {
        return 0;
    }
    else if (srcB == e_dstE)
    {
        return e_valE;
    }
    else if (srcB == mreg->get(M_DSTM))
    {
        return m_valM;
    }
    else if (srcB == mreg->get(M_DSTE))
    {
        return mreg->get(M_VALE);
    }
    else if (srcB == wreg->get(W_DSTM))
    {
        return wreg->get(W_VALM);
    }
    else if (srcB == wreg->get(W_DSTE))
    {
        return wreg->get(W_VALE);
    }
    else
    {
        return rf->readRegister(DecodeStage::getSrcB(dreg), error);
    }
}

bool DecodeStage::calculateControlSignals(PipeReg * ereg)
{
    uint64_t icode = ereg->get(E_ICODE);
    uint64_t dstM = ereg->get(E_DSTM);

    if (icode == IJXX && !(e_Cnd) || ((icode == IMRMOVQ || icode == IPOPQ) && (dstM == d_srcA || dstM == d_srcB)))
    {
        return true;
    }
    else
    {
        return false;
    }
}








