#include <string>
#include <cstdint>
#include "RegisterFile.h"
#include "PipeRegField.h"
#include "PipeReg.h"
#include "F.h"
#include "D.h"
#include "M.h"
#include "W.h"
#include "Memory.h"
#include "ConditionCodes.h"
#include "Stage.h"
#include "MemoryStage.h"
#include "Status.h"
#include "Debug.h"
#include "Instructions.h"

bool MemoryStage::doClockLow(PipeReg ** pregs)
{
    bool mem_error = false;
    PipeReg * mreg = pregs[MREG];
    PipeReg * wreg = pregs[WREG];
    uint64_t icode = mreg->get(M_ICODE);
    uint64_t valE = mreg->get(M_VALE);
    uint64_t valA = mreg->get(M_VALA);
    uint64_t dstE = mreg->get(M_DSTE);
    uint64_t dstM = mreg->get(M_DSTM);
    m_valM = mem_addr(mreg);
    m_stat = get_m_stat(mreg, mem_error);



    if (mem_read(icode))
    {
        bool mem_error;
        m_valM = mem->getLong(m_valM, mem_error);
        if (mem_error)
        {
            m_stat = SADR;
        }
    }
    else if (mem_write(icode))
    {
        bool mem_error;
        mem->putLong(valA, m_valM, mem_error);
        m_valM = 0;
        if (mem_error)
        {
            m_stat = SADR;
        }
    }
    setWInput(wreg, m_stat, icode, valE, m_valM, dstE, dstM);
    return false;

}

void MemoryStage::doClockHigh(PipeReg ** pregs)
{
    PipeReg * mreg = pregs[MREG]; 
    PipeReg * wreg = pregs[WREG];
    mreg->normal();
    wreg->normal();
}

void MemoryStage::setWInput(PipeReg * wreg, uint64_t stat, uint64_t icode,
                             uint64_t valE, uint64_t valM, uint64_t dstE,
                             uint64_t dstM)
{
    wreg->set(W_STAT, stat);
    wreg->set(W_ICODE, icode);
    wreg->set(W_VALE, valE);
    wreg->set(W_VALM, valM);
    wreg->set(W_DSTE, dstE);
    wreg->set(W_DSTM, dstM);
}

uint64_t MemoryStage::mem_addr(PipeReg * mreg)
{
    uint64_t icode = mreg->get(M_ICODE);
    if (icode == IRMMOVQ || icode == IPUSHQ || icode == ICALL || icode == IMRMOVQ)
    {
        return mreg -> get(M_VALE);
    }
    else if (icode == IPOPQ || icode == IRET) 
    {
        return mreg -> get(M_VALA);
    }
    else 
    {
        return 0;
    }
}

bool MemoryStage::mem_read(uint64_t icode)
{
    if (icode == IMRMOVQ || icode == IPOPQ || icode == IRET)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool MemoryStage::mem_write(uint64_t icode)
{
    if (icode == IRMMOVQ || icode == IPUSHQ || icode == ICALL)
    {
        return true;
    }
    else 
    {
        return false;
    }
}

uint64_t MemoryStage::get_m_stat(PipeReg * mreg, bool mem_error)
{
    if (mem_error)
    {
        return SADR;
    }
    else
    {
        return mreg->get(M_STAT);
    }
}
