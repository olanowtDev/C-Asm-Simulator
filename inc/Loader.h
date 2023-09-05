
class Loader
{
   private:
      int32_t lastAddress;  //last address stored to in memory
      Memory * mem;         //pointer to memory instance
      std::ifstream inf;    //file handle
      String * inputFile;   //pointer to String object containing file name

      bool openFile();      //check file name for correctness and open it
      bool printErrMsg(int32_t, int32_t, String *);  //print error message

      //TODO
      //add declarations for other methods used by your loader
      
   public:
      //methods called outside of class
      Loader(int argc, char * argv[], Memory * mem);
      bool load();
      bool badFile(String * str);
      bool fileCanBeOpened();
      void loadLine(String * line);
      int32_t convertAddress(String * str, int32_t start, int32_t len);
      int32_t convertString(String * str, int32_t start, int32_t len);
      bool badComment(String * str);
      bool missingGuard(String * str);
      bool commentLine(String * str);
      bool dataLine(String * str);
      bool badData(String * str);
      bool missingColon(String * str);
      uint32_t dataLength(String * str);
      bool dataNonHex(String * str);
      bool dataBytesModTwo(String * str);
      bool spaceFollowColon(String * str);
      bool dataAddressField(String * str);
      bool consecutiveBytes(String * str);
      bool invalidMemAddress(String * str);
      bool outsideMemArray(String * str);
      
};
