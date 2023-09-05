class MemoryStage: public Stage
{
    public:
        bool doClockLow(PipeReg ** pregs);
        void doClockHigh(PipeReg ** pregs);
        void setWInput(PipeReg * wreg, uint64_t stat, uint64_t icode,
                             uint64_t valE, uint64_t valM, uint64_t dstE,
                             uint64_t dstM);
        uint64_t mem_addr(PipeReg * mreg);
        bool mem_read(uint64_t icode);
        bool mem_write(uint64_t icode);
        uint64_t get_m_stat(PipeReg * mreg, bool mem_error);




};