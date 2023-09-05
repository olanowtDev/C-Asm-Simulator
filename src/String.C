#include <cstdint>
#include <string> //for std::string functions
#include <string.h> //for c-string functions (strlen, strcpy, ..)
#include "Debug.h"
#include "String.h"

/*
 * String
 *
 * Construct a String object out of a std::string 
 * This constructor would also be called on a c-str
 *
 * Modifies: str and length data members
 */
String::String(std::string str)
{
   //Dynamically allocate an array of chars just large enough to 
   //hold the chars of the std::string.
   //Don't store the NULL or allocate space for it.
   //Copy the characters in the std::string (excluding a NULL)
   //into your str array.
   //Set length to the size of the array.
    this->length = str.length();
    this->str = (char*)malloc(str.length());
    for (int i = 0; i < this->length; i++)
    {
        this->str[i] = str[i];
    }
}

/*
 * get_cstr
 *
 * Build a return a c-str from the array of characters.
 */
char * String::get_cstr()
{
   //You need to dynamically allocate space (array of char) for the
   //c-string that you are building
   //That space needs to include space for a NULL
   //Don't forget to add the NULL.
   char * toReturn = (char*)malloc(this->length + 1);
   for (int i = 0; i < this->length; i++)
   {
        toReturn[i] = this->str[i];
   }
   toReturn[length]=NULL;
   return toReturn;
    
}

/*
 * get_stdstr
 *
 * Build a return a std::string from the array of characters.
 */
std::string String::get_stdstr()
{
    char * newString = (char*)malloc(this->length + 1);
    for (int i = 0; i < this->length; i++)
    {
        newString[i] = this->str[i];
    }   
    newString[length] = NULL;
    std::string toReturn = newString;
   
   return toReturn; 
}

/*
 * get_length
 *
 * return the value of the length data member
 */
int32_t String::get_length()
{
    return this->length; 
}

/*
 * badIndex
 *
 * Returns true if this index into the str array is
 * invalid (negative or greater than array size)
 */
bool String::badIndex(int32_t idx) 
{
    if (idx < 0 || idx >= this->length)
    {
        return true;
    }
    return false;
}

/*
 * isSpaces
 *
 * Returns true if the characters in the array from
 * startIdx to endIdx are all spaces.
 *
 * Three cases are possible:
 * 1) error set to true and returns false if indices are invalid
 * 2) error set to false and returns false if the characters in
 *    the array at the specified indices are not all spaces
 * 3) error set to false and returns true if the characters in
 *    the array at the specified indices are all spaces
 */

bool String::isSpaces(int32_t startIdx, int32_t endIdx, bool & error)
{
    if (badIndex(startIdx) || badIndex(endIdx) || endIdx < startIdx)
    {
        error = true;
        return false;
    }

    error = false;

    for (int i = startIdx; i <= endIdx; i++)
    {
        if (this->str[i] != ' ')
        {
            return false;
        }
    }
    return true;
}

/*
 * convert2Hex
 *
 * Builds and returns a number from the values in the array from startIdx
 * to endIdx treating those characters as if they are hex digits.
 * Returns 0 and sets error to true if the indices are invalid or
 * if the characters are not hex.
 *
 * Valid characters for conversion are:
 *  '0', '1' through '9'
 *  'a', 'b' through 'f'
 *  'A', 'B' through 'F'
 *
 *  Three cases are possible:
 *  1) sets error to false and returns converted number if the characters
 *     in the specified indices are valid hex characters
 *  2) sets error to true and returns 0 if the indices are invalid
 *  3) sets error to true and returns 0 if the indices are valid but the
 *     characters are not hex
 */
uint32_t String::convert2Hex(int32_t startIdx, int32_t endIdx, bool & error)
{
    if (badIndex(startIdx) || badIndex(endIdx) || endIdx < startIdx)
    {
        error = true;
        return 0;
    }
    error = false;
    int x = 0;

    for (int i = startIdx; i <= endIdx; i++)
    {
        switch(this->str[i])
        {
            case '0':
                x = x << 4;
                break;
            case '1':
                x += 1;
                x = x << 4;
                break;
            case '2':
                x += 2;
                x = x << 4;
                break;
            case '3':
                x += 3;
                x = x << 4;
                break;
            case '4':
                x += 4;
                x = x << 4;
                break;
            case '5':
                x += 5;
                x = x << 4;
                break;
            case '6':
                x += 6;
                x = x << 4;
                break;
            case '7':
                x += 7;
                x = x << 4;
                break;
            case '8':
                x += 8;
                x = x << 4;
                break;
            case '9':
                x += 9;
                x = x << 4;
                break;
            case 'a': case 'A':
                x += 10;
                x = x << 4;
                break;
            case 'b': case 'B':
                x += 11;
                x = x << 4;
                break;
            case 'c': case 'C':
                x += 12;
                x = x << 4;
                break;
            case 'd': case 'D':
                x += 13;
                x = x << 4;
                break;
            case 'e': case 'E':
                x += 14;
                x = x << 4;
                break;
            case 'f': case 'F':
                x += 15;
                x = x << 4;
                break;
            default:
                error = true;
                return 0;
        }
    }
    x = x >> 4;
    //printf("%x\n", x);
    return x;
}

/* 
 * isChar
 * Returns true if str[idx] is equal to what 
 *
 * Three cases are possible:
 * 1) set error to true and return false if idx is invalid
 * 2) set error to false and return true if str[idx] is what
 * 3) set error to false and return false if str[idx] is not what
 */
bool String::isChar(char what, int32_t idx, bool & error)
{
    if (badIndex(idx))
    {
        error = true;
        return false;
    }
    error = false;
    if (this->str[idx] == what)
    {
        return true;
    }
    return false;
}      

/*
 * isSubString
 *
 * Returns true if the c-str subStr is in the str array starting
 * at index idx 
 *
 * Three cases are possible:
 * 1) one or both indices are invalid: set error to true and return false
 * 2) indices are valid and subStr is in str array: set error to false 
 *    and return true
 * 3) indices are valid and subStr is not in str array: set error to 
 *    false and return false
 */
bool String::isSubString(char * subStr, int32_t idx, bool & error)
{
   //TODO 
    if (badIndex(idx) || badIndex(idx + strlen(subStr) - 1))
    {
        error = true;
        return false;
    }
    error = false;
   /*
    for (uint32_t i = idx; i < (sizeof(*subStr)/sizeof(uint32_t)); i++)
    {
        if (!(this->str[i].isChar(*subStr,i,error)))
        {
            return false;
        }
    }
    return true;
    */
    for (uint32_t i = idx, j = 0; j < strlen(subStr); i++, j++)
    {
        if (subStr[j] != this->str[i])
        {
            return false;
        }
    }
    return true;
}


/*
 * isSubString
 *
 * Returns true if the std::string subStr is in the str array starting
 * at index idx 
 *
 * Three cases are possible:
 * 1) one or both indices are invalid: set error to true and return false
 * 2) indices are valid and subStr is in str array: set error to false 
 *    and return true
 * 3) indices are valid and subStr is not in str array: set error to 
 *    false and return false
 */
bool String::isSubString(std::string subStr, int32_t idx, bool & error)
{  
    //TODO
    if (badIndex(idx) || badIndex(idx + subStr.length() - 1))
    {
        error = true;
        return false;
    }
    error = false;
    
    for (uint32_t i = idx, j = 0; j < subStr.length(); i++, j++)
    {
        if (subStr[j] != this->str[i])
        {
            return false;
        }
    }
    return true;
                                                               
    
    /*
    for (uint32_t i = 0; i < (sizeof(subStr)/(sizeof(uint32_t))); i++)
    {
        if (subStr[i] != this->str[i])
        {
            return false;
        }
    }
    */
}

 
