class ExecuteStage: public Stage
{
   uint64_t valE;
   uint64_t dstE;

   private:
      bool M_bubble;

   public:
      bool doClockLow(PipeReg ** pregs);
      void doClockHigh(PipeReg ** pregs);
      void setMInput(PipeReg * mreg, uint64_t stat, uint64_t icode, uint64_t Cnd,
                             uint64_t valE, uint64_t valA, uint64_t dstE,
                             uint64_t dstM);

      uint64_t aluA(PipeReg * ereg);
      uint64_t aluB(PipeReg * ereg);
      uint64_t alufun(PipeReg * ereg);
      bool set_cc(PipeReg * ereg, PipeReg * wreg);
      uint64_t get_e_dstE(PipeReg * ereg, bool Cnd);
      uint64_t ALU(PipeReg * ereg, PipeReg * wreg, uint64_t icode);
      void CC(PipeReg * ereg, uint64_t icode, uint64_t function, int64_t valE, int64_t aluA, int64_t aluB);
      uint64_t cond(uint64_t icode, uint64_t ifun);
      bool calculateControlSignals(PipeReg * wreg);









};
