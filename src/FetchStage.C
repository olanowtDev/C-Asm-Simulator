#include <iostream>
#include <cstdint>
#include <stdio.h>
#include "Memory.h"
#include "ConditionCodes.h"
#include "Instructions.h"
#include "RegisterFile.h"
#include "PipeRegField.h"
#include "PipeReg.h"
#include "Tools.h"
#include "F.h"
#include "D.h"
#include "E.h"
#include "M.h"
#include "W.h"
#include "Stage.h"
#include "FetchStage.h"
#include "Status.h"
#include "Debug.h"

/*
 * doClockLow
 *
 * Performs the Fetch stage combinational logic that is performed when
 * the clock edge is low.
 *
 * @param: pregs - array of the pipeline register (F, D, E, M, W instances)
 */
bool FetchStage::doClockLow(PipeReg ** pregs)
{
   PipeReg * freg = pregs[FREG];  //pointer to object representing F pipeline register
   PipeReg * dreg = pregs[DREG];  //pointer to object representing D pipeline register
   PipeReg * ereg = pregs[EREG];
   PipeReg * wreg = pregs[WREG];
   PipeReg * mreg = pregs[MREG];
   
   bool mem_error = false;
   uint64_t icode = INOP, ifun = FNONE, rA = RNONE, rB = RNONE;
   uint64_t valC = 0, valP = 0, stat = 0, predPC = 0;
   bool needvalC = false;
   bool needregId = false;

   uint64_t f_pc = selectPC(freg, mreg, wreg);
   uint64_t instructs = mem->getByte(f_pc, mem_error);
   uint64_t regs = mem->getByte(f_pc+1, mem_error);   

   calculateControlSignals(dreg, ereg, mreg);

   if (mem_error)
   {
      ifun = FNONE;
      icode = INOP;
   }
   else
   {
      ifun = Tools::getBits(instructs, 0, 3);
      icode = Tools::getBits(instructs, 4, 7);
   }

   bool valid_instruction = instr_valid(icode);

   if (mem_error)
   {
      stat = SADR;
   }
   else if (!valid_instruction)
   {
      stat = SINS;
   }
   else if (icode == IHALT)
   {
      stat = SHLT;
   }
   else
   {
      stat = SAOK;
   }

   needvalC = needValC(icode);
   needregId = needRegIds(icode);

   if (needregId)
   {
      rA = Tools::getBits(regs, 4, 7);
      rB = Tools::getBits(regs, 0, 3);
   }

   if (needvalC)
   {
      if (needregId) 
      {
         valC = buildValC(f_pc, true); 
      }
      else
      {
         valC = buildValC(f_pc, false);
      }
   }
 
   valP = PCincrement(f_pc, needRegIds(icode), needValC(icode));
  
   predPC = predictPC(icode, valC, valP);

   freg->set(F_PREDPC, predPC);


   setDInput(dreg, stat, icode, ifun, rA, rB, valC, valP);
   return false;
}

/* doClockHigh
 *
 * applies the appropriate control signal to the F
 * and D register intances
 * 
 * @param: pregs - array of the pipeline register (F, D, E, M, W instances)
*/
void FetchStage::doClockHigh(PipeReg ** pregs)
{
   PipeReg * freg = pregs[FREG];  
   PipeReg * dreg = pregs[DREG];

   if (!f_stall)
   {
      freg->normal();
   }
   if (d_bubble)
   {
      ((D *)dreg)->bubble();
   }
   else if (!d_stall)
   {
      dreg->normal();
   }
   
}

/* setDInput
 * provides the input to potentially be stored in the D register
 * during doClockHigh
 *
 * @param: dreg - pointer to the D register instance
 * @param: stat - value to be stored in the stat pipeline register within D
 * @param: icode - value to be stored in the icode pipeline register within D
 * @param: ifun - value to be stored in the ifun pipeline register within D
 * @param: rA - value to be stored in the rA pipeline register within D
 * @param: rB - value to be stored in the rB pipeline register within D
 * @param: valC - value to be stored in the valC pipeline register within D
 * @param: valP - value to be stored in the valP pipeline register within D
*/
void FetchStage::setDInput(PipeReg * dreg, uint64_t stat, uint64_t icode, 
                           uint64_t ifun, uint64_t rA, uint64_t rB,
                           uint64_t valC, uint64_t valP)
{
   dreg->set(D_STAT, stat);
   dreg->set(D_ICODE, icode);
   dreg->set(D_IFUN, ifun);
   dreg->set(D_RA, rA);
   dreg->set(D_RB, rB);
   dreg->set(D_VALC, valC);
   dreg->set(D_VALP, valP);
}

uint64_t FetchStage::selectPC(PipeReg * freg, PipeReg * mreg, PipeReg * wreg)
{

   if (mreg->get(M_ICODE) == IJXX && !(mreg->get(M_CND)))
   {
      return mreg->get(M_VALA);
   }
   else if (wreg->get(W_ICODE) == IRET)
   {
      return wreg->get(W_VALM);
   }
   else
   {
      return freg->get(F_PREDPC);      
   }
}

bool FetchStage::needRegIds(uint64_t icode)
{
   if (icode == IRRMOVQ || icode == IOPQ || icode == IPUSHQ || icode == IPOPQ || icode == IIRMOVQ || icode == IRMMOVQ || icode == IMRMOVQ ) 
   {
      return true;
   }

   return false;
}


bool FetchStage::needValC(uint64_t icode)
{
   if (icode == IIRMOVQ || icode == IRMMOVQ || icode == IMRMOVQ || icode == IJXX || icode == ICALL) 
   {
      return true;
   }
   return false;
}

uint64_t FetchStage::predictPC(uint64_t icode, uint64_t valC, uint64_t valP)
{
   if (icode == IJXX || icode == ICALL) 
   {
      return valC;
   }
   return valP;
}

uint64_t FetchStage::PCincrement(uint64_t address, bool regids, bool val_C)
{
   if (regids) 
   {
      address++;
   }

   if (val_C) 
   {
      address += 8;
   }

   address++;

   return address;
}

uint64_t FetchStage::getRegIds(uint64_t address)
{
   bool mem_error;
   address++;
   uint64_t regIds = mem->getByte(address, mem_error);
   return regIds;

}

uint64_t FetchStage::buildValC(uint64_t address, bool needRegIds)
{
   bool mem_error;

   if (needRegIds)
   {
      address = address + 2;
   }
   else
   {
      address = address + 1;
   }

   uint8_t bytes[8];

   for (int i = 0; i < 8; i++)
   {
      bytes[i] = mem->getByte(address + i, mem_error);
   }

   uint64_t toReturn = Tools::buildLong(bytes);
   return toReturn;
}

bool FetchStage::instr_valid(uint64_t icode)
{
   switch (icode)
   {
      case INOP:
      case IHALT:
      case IRRMOVQ:
      case IIRMOVQ:
      case IRMMOVQ:
      case IMRMOVQ:
      case IOPQ:
      case IJXX:
      case ICALL:
      case IRET:
      case IPUSHQ:
      case IPOPQ:
         return true;
      default:
         return false;
   }
}

bool FetchStage::F_stall(PipeReg * dreg, PipeReg * ereg, PipeReg * mreg)
{
   uint64_t icode = ereg->get(E_ICODE);
   uint64_t dstM = ereg->get(E_DSTM);

   if ((icode == IMRMOVQ || icode == IPOPQ) && (dstM == d_srcA || dstM == d_srcB) ||
    (IRET == dreg->get(D_ICODE) || IRET == icode || IRET == mreg->get(M_ICODE)))
   {
      return true;
   }
   else
   {
      return false;
   }
}

bool FetchStage::D_stall(PipeReg * ereg)
{
   uint64_t icode = ereg->get(E_ICODE);
   uint64_t dstM = ereg->get(E_DSTM);

   if ((icode == IMRMOVQ || icode == IPOPQ) && (dstM == d_srcA || dstM == d_srcB))
   {
      return true;
   }
   else
   {
      return false;
   }

}

bool FetchStage::D_Bubble(PipeReg * dreg, PipeReg * ereg, PipeReg * mreg, bool cnd)
{
   uint64_t icode = ereg->get(E_ICODE);
   if(icode == IJXX && !(cnd) || 
     !((icode == IMRMOVQ || icode == IPOPQ) && (ereg->get(E_DSTM) == d_srcA || ereg->get(E_DSTM) == d_srcB)) && 
     (IRET == dreg->get(D_ICODE) || IRET == icode || IRET == mreg->get(M_ICODE)))
   {
      return true;
   }
   else 
   {
      return false;
   }
}

void FetchStage::calculateControlSignals(PipeReg * dreg, PipeReg * ereg, PipeReg * mreg)
{
   f_stall = F_stall(dreg, ereg, mreg);
   d_stall = D_stall(ereg);
   d_bubble = D_Bubble(dreg, ereg, mreg, e_Cnd);
}


     
