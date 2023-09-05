#include <iostream>
#include <fstream>
#include <string.h>
#include <ctype.h>
#include "Debug.h" 
#include "Memory.h"
#include "String.h"
#include "Loader.h"

//These are the constants used by the
//printErrMsg method.
#define USAGE 0          /* missing command line argument */ 
#define BADFILE 1        /* file has wrong suffix */
#define OPENERR 2        /* file doesn't open */
#define BADDATA 3        /* bad data record */
#define BADCOM 4         /* bad comment record */
#define NUMERRS 5

//useful defines to avoid magic numbers
#define ADDRBEGIN 2      /* beginning and ending indices for address */
#define ADDREND 4
#define DATABEGIN 7      /* beginning index for data bytes */ 
#define COMMENT 28       /* location of | */
#define MAXBYTES 10      /* max data bytes in a data record */

/* 
 * Loader
 * Initializes the private data members
 */
Loader::Loader(int argc, char * argv[], Memory * mem)
{
   //this method is COMPLETE
   this->lastAddress = -1;   //keep track of last mem byte written to for error checking
   this->mem = mem;          //memory instance
   this->inputFile = NULL;   
   if (argc > 1) inputFile = new String(argv[1]);  //input file name
}

/*
 * printErrMsg
 * Prints an error message and returns false (load failed)
 * If the line number is not -1, it also prints the line where error occurred
 *
 * which - indicates error number
 * lineNumber - number of line in input file on which error occurred (if applicable)
 * line - line on which error occurred (if applicable)
 */
bool Loader::printErrMsg(int32_t which, int32_t lineNumber, String * line)
{
   //this method is COMPLETE
   static char * errMsg[NUMERRS] = 
   {
      (char *) "Usage: yess <file.yo>\n",                       //USAGE
      (char *) "Input file name must end with .yo extension\n", //BADFILE
      (char *) "File open failed\n",                            //OPENERR
      (char *) "Badly formed data record\n",                    //BADDATA
      (char *) "Badly formed comment record\n",                 //BADCOM
   };   
   if (which >= NUMERRS)
   {
      std::cout << "Unknown error: " << which << "\n";
   } else
   {
      std::cout << errMsg[which]; 
      if (lineNumber != -1 && line != NULL)
      {
         std::cout << "Error on line " << std::dec << lineNumber
              << ": " << line->get_stdstr() << std::endl;
      }
   } 
   return false; //load fails
}

/*
 * openFile
 * The name of the file is in the data member openFile (could be NULL if
 * no command line argument provided)
 * Checks to see if the file name is well-formed and can be opened
 * If there is an error, it prints an error message and returns false.
 * Otherwise, the file is opened and the function returns false
 *
 * modifies inf data member (file handle) if file is opened
 */
bool Loader::openFile()
{

   //If the user didn't supply a command line argument (inputFile is NULL)
   //then print the USAGE error message and return false
   //printf("\nentered openfile\n");
   if (this->inputFile == NULL)
   {
      printErrMsg(USAGE, this->lastAddress, this->inputFile);
      return false;
   }
   
   //If the filename is badly formed (doesn't end in a .yo)
   //then print the BADFILE error message and return false
   if (badFile(inputFile))
   {
      printErrMsg(BADFILE, this->lastAddress, this->inputFile);
      return false;
   }
   
   //open the file using an std::ifstream open
   //if the file can't be opened then print the OPENERR message 
   //and return false
   if (!fileCanBeOpened())
   {
      printErrMsg(OPENERR, this->lastAddress, this->inputFile);
      return false;
   }



   return true;  //file name is good and file open succeeded
}

/*
 * load 
 * Opens the .yo file
 * Reads the lines in the file line by line and
 * loads the data bytes in data records into the Memory
 * If a line has an error in it, then NONE of the line will be
 * loaded into memory and the load function will return false
 *
 * Returns true if load succeeded (no errors in the input) 
 * and false otherwise
*/   
bool Loader::load()
{
   if (!openFile()) return false;

   std::string line;
   int lineNumber = 1;  //needed if an error is found


   while (getline(inf, line))
   {
      //create a String to contain the std::string
      //Now, all accesses to the input line MUST be via your
      //String class methods
      String inputLine(line);
      //if the line is a data record with errors
      //then print the BADDATA error message and return false
      if (badData(&inputLine))
      {
         printErrMsg(BADDATA, lineNumber, &inputLine);
         return false;
      }
      else if (badComment(&inputLine))
      {
         printErrMsg(BADCOM, lineNumber, &inputLine);
         return false;
      }
      else
      {
         loadLine(&inputLine);
      }
      //if the line is a comment record with errors
      //then print the BADCOM error message and return false
      //
      //Otherwise, load any data on the line into
      //memory
      

      //Don't do all of this work in this method!
      //Break the work up into multiple single purpose methods

      //increment the line number for next iteration
      lineNumber++;
   }

   return true;  //load succeeded
}

//add helper methods here and to Loader.h

/**
 * Helper method to make sure that the file can be opened
 * takes a string object as input and then runs the open, is_open, and close ifstream methods in sequence 
 * to test if the file can be opened
 * returns true if the file can be opened, false otherwise.
*/
bool Loader::fileCanBeOpened()
{
   this->inf.open(this->inputFile->get_stdstr(), std::ifstream::in);
   if (this->inf.is_open())
   {
      return true;
   }
   else
   {
      return false;
   }

   this->inf.close();

}


/**
 * Helper method to make sure the input file retains certain parameteres, namely, ending with a  ".yo" extension.
*/
bool Loader::badFile(String * str)
{
   std::string check = ".yo";
   std::string fileName = this->inputFile->get_stdstr();

   if ((fileName.find(check) == std::string::npos) || fileName.length() < BADCOM)
   {
      return true;
   }
   else
   {
      return false;
   }   
}


int32_t Loader::convertString(String * str, int32_t start, int32_t end)
{
   bool error = false;
   return str->convert2Hex(start, end, error);
}


void Loader::loadLine(String * str)
{
   int32_t address = convertString(str, ADDRBEGIN, ADDREND); 
	int32_t curByte = DATABEGIN;

	bool error = false;

   while (!str->isSpaces(curByte,curByte,error))
   {
      int32_t data = convertString(str,curByte,curByte+1);
      this->mem->putByte(data,address,error);
      curByte += ADDRBEGIN;
      this->lastAddress = address;
      address++;
   }
}

bool Loader::badComment(String * str)
{
   bool error = false;
   if (!dataLine(str))
   {
      if (!commentLine(str))
      {
         return true;
      }
      if (missingGuard(str))
      {
         return true;
      }
   }
   
   return false;
}

bool Loader::missingGuard(String * str)
{
   bool error = false;
   if (!str->isChar('|', COMMENT, error))
   {
      return true;
   }
   else
   {
      return false;
   }
}

bool Loader::commentLine(String * str)
{
   bool error = false;
   if (str->isSpaces(USAGE, COMMENT-1, error))
   {
      return true;
   }
   else
   {
      return false;
   }
}

bool Loader::dataLine(String * str)
{
   bool error = false;
   if (str->isChar('0', 0, error) && str->isChar('x', 1, error))
   {
      return true;
   }
   else
   {
      return false;
   }
}




bool Loader::badData(String * str)
{
   bool error = false;
   if (dataLine(str))
   {
      if (missingGuard(str))
      {
         return true;
      }

      if (missingColon(str))
      {
         return true;
      }

      if (dataNonHex(str))
      {
         return true;
      }

      if (!dataBytesModTwo(str))
      {
         return true;
      }

      if (spaceFollowColon(str))
      {
         return true;
      }

      if (consecutiveBytes(str))
      {
         return true;
      }

      if (invalidMemAddress(str))
      {
         return true;
      }

      if (outsideMemArray(str))
      {
         return true;
      }
      


   }

   return false;
}

bool Loader::missingColon(String * str)
{
   bool error = false;
   if (!str->isChar(':',NUMERRS,error))
   {
      return true;
   }
   else
   {
      return false;
   }
}

uint32_t Loader::dataLength(String * str)
{
   bool error = false;

   uint32_t len = 0;

   for (int i = DATABEGIN; i < COMMENT-1; i++)
   {
      if (!str->isSpaces(i,i,error))
      {
         len++;
      }
   }

   //printf("\n len is: %d", len);

   return len;
}

bool Loader::dataNonHex(String * str)
{
   bool error = false;
   uint32_t dataLen = dataLength(str);

   for (uint32_t i = 0; i < dataLen; i++)
   {
      uint32_t conversion = str->convert2Hex(DATABEGIN+i,DATABEGIN+i, error);

      //printf("\nconversion returns: %x", conversion);

      if (conversion == 0 && error == true)
      {
         return true;
      }
   }

   return false;
}

bool Loader::dataBytesModTwo(String * str)
{
   uint32_t dataBytes = dataLength(str);

   return dataBytes % 2 == 0;
}

bool Loader::spaceFollowColon(String * str)
{
   bool error = false;
   if (!str->isSpaces(DATABEGIN-1,DATABEGIN-1, error))
   {
      return true;
   }
   else
   {
      return false;
   }
}

bool Loader::dataAddressField(String * str)
{
   bool error = false;
   if (str->isSpaces(ADDRBEGIN,ADDREND,error))
   {
      return true;
   }
   else
   {
      return false;
   }
}

bool Loader::consecutiveBytes(String * str)
{
   bool error = false;
   uint32_t dataBytes = dataLength(str);

   if (!str->isSpaces(DATABEGIN + dataBytes, COMMENT - 1, error))
   {
      return true;
   }
   else
   {
      return false;
   }

}

bool Loader::invalidMemAddress(String * str)
{
   bool error = false;
   int32_t currAdd = str->convert2Hex(ADDRBEGIN,ADDREND,error);
   int32_t numBytes = dataLength(str);

   if (currAdd < this->lastAddress || (currAdd + numBytes) < this->lastAddress)
   {
      return true;
   }
   else
   {
      return false;
   }
}

bool Loader::outsideMemArray(String * str)
{
   bool error = false;
   int32_t currAdd = str->convert2Hex(ADDRBEGIN,ADDREND,error);
   int32_t numBytes = dataLength(str);
   numBytes /= 2;

   if (currAdd > MEMSIZE || (currAdd + numBytes) > MEMSIZE)
   {
      return true;
   }
   else
   {
      return false;
   }
}








