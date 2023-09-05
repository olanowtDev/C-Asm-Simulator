class DecodeStage: public Stage
{
   private:
      bool e_bubble;
      void setEInput(PipeReg * ereg, uint64_t stat, uint64_t icode, uint64_t ifun,
                              uint64_t valC, uint64_t valA, uint64_t valB, uint64_t dstE,
                              uint64_t dstM, uint64_t srcA, uint64_t srcB);

   public:
      bool doClockLow(PipeReg ** pregs);
      void doClockHigh(PipeReg ** pregs);
      uint64_t getSrcA(PipeReg * dreg);
      uint64_t getSrcB(PipeReg * dreg);
      uint64_t getDstE(PipeReg * dreg, uint64_t icode);
      uint64_t getDstM(PipeReg * dreg);
      uint64_t getValA(uint64_t d_srca, PipeReg * dreg, PipeReg * mreg, PipeReg * wreg);
      uint64_t getValB(uint64_t d_srcb, PipeReg * dreg, PipeReg * mreg, PipeReg * wreg);
      bool calculateControlSignals(PipeReg * ereg);

};
