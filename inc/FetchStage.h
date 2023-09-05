//class to represent the Fetch stage

class FetchStage: public Stage
{
   private:
      bool f_stall;
      bool d_stall;
      bool d_bubble;

      //set the input value for each fieldof the D pipeline register
      void setDInput(PipeReg * dreg, uint64_t stat, uint64_t icode, 
                     uint64_t ifun, uint64_t rA, uint64_t rB,
                     uint64_t valC, uint64_t valP);
   public:
      //These are the only methods that are called outside of the class
      bool doClockLow(PipeReg ** pregs);  
      void doClockHigh(PipeReg ** pregs);
      uint64_t selectPC(PipeReg * freg, PipeReg * mreg, PipeReg * wreg);
      bool needRegIds(uint64_t icode);
      bool needValC(uint64_t icode);
      uint64_t predictPC(uint64_t icode, uint64_t valC, uint64_t valP);
      uint64_t PCincrement(uint64_t address, bool regids, bool val_C);
      uint64_t getRegIds(uint64_t address);
      uint64_t buildValC(uint64_t address, bool needRegIds);
      bool instr_valid(uint64_t icode);
      bool F_stall(PipeReg * dreg, PipeReg * ereg, PipeReg * mreg);
      bool D_stall(PipeReg * ereg);
      bool D_Bubble(PipeReg * dreg, PipeReg * ereg, PipeReg * mreg, bool cnd);
      void calculateControlSignals(PipeReg * dreg, PipeReg * ereg, PipeReg * mreg);






};
