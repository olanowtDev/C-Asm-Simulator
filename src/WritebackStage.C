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
#include "WritebackStage.h"
#include "Status.h"
#include "Debug.h"
#include "Instructions.h"


bool WritebackStage::doClockLow(PipeReg ** pregs)
{
    PipeReg * wreg = pregs[WREG];
    uint64_t icode = wreg->get(W_ICODE);
    uint64_t stat = wreg->get(W_STAT);
    if (stat != SAOK)
    {
        return true;
    }
    else
    {
        return false;
    }

}

void WritebackStage::doClockHigh(PipeReg ** pregs)
{
    PipeReg * wreg = pregs[WREG];
    bool w_error;
    rf->writeRegister(wreg->get(W_VALE), wreg->get(W_DSTE), w_error);
    rf->writeRegister(wreg->get(W_VALM), wreg->get(W_DSTM), w_error);
}
