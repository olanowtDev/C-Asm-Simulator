#makefile to compile the .C files in this directory
#and write the .o output to the ../obj directory
#This makefile also creates the yess executable.
CC = g++
INC = ../inc
BIN = ../bin
OBJD = ../obj
CFLAGS = -g -c -Wall -Wno-unused-variable -std=c++11 -I $(INC)

OBJ = $(OBJD)/Tools.o\
	  $(OBJD)/Memory.o\
	  $(OBJD)/ConditionCodes.o\
      $(OBJD)/RegisterFile.o \
	  $(OBJD)/String.o\
	  $(OBJD)/Loader.o\
	  $(OBJD)/yess.o\
	  $(OBJD)/DecodeStage.o\
	  $(OBJD)/ExecuteStage.o\
	  $(OBJD)/MemoryStage.o\
	  $(OBJD)/FetchStage.o\
	  $(OBJD)/WritebackStage.o\
	  $(OBJD)/CPU.o\
	  $(OBJD)/F.o\
	  $(OBJD)/D.o\
	  $(OBJD)/E.o\
	  $(OBJD)/M.o\
	  $(OBJD)/W.o\
	  $(OBJD)/PipeReg.o\
	  $(OBJD)/PipeRegField.o\
	  $(OBJD)/Stage.o

OBJS = yess.o Loader.o String.o RegisterFile.o ConditionCodes.o Memory.o Tools.o

#common includes for F/D/E/M/W.C files. Currently unused.
PIPERH = $(INC)/Status.h\
		 $(INC)/PipeReg.h\
		 $(INC)/PipeRegField.h\
		 $(INC)/Instructions.h\
		 $(INC)/RegisterFile.h

#this is just a brute force for getting all the headers to compile
HEADERALL = $(INC)/ConditionCodes.h\
			$(INC)/Tools.h\
			$(INC)/Memory.h\
			$(INC)/RegisterFile.h\
			$(INC)/PipeRegField.h\
			$(INC)/PipeReg.h\
			$(INC)/F.h\
			$(INC)/D.h\
			$(INC)/E.h\
			$(INC)/M.h\
			$(INC)/W.h\
			$(INC)/Stage.h\
			$(INC)/MemoryStage.h\
			$(INC)/ExecuteStage.h\
			$(INC)/DecodeStage.h\
			$(INC)/WritebackStage.h\
			$(INC)/CPU.h\
			$(INC)/Debug.h\
			$(INC)/Status.h\
			$(INC)/Instructions.h\
			$(INC)/String.h\
			$(INC)/Loader.h 

		 
#rule to match .C with object in another directory
$(OBJD)/%.o: %.C 
	@mkdir -p $(OBJD)
	$(CC) $(CFLAGS) $< -o $@

#1) Need a .o target for the new .C files (String.C, yess.C, Loader.C)
#   Add those targets to the OBJ macro.
#
#2) Need a ../bin/yess target that links together the .o files
#   to create a ../bin/yess executable (make the first target in
#   the makefile).  It actually doesn't need all of the .o files
#   yet, but it doesn't hurt to link them into the executable
#   Use the OBJ and BIN macros

#The rules below should match what you had in your old makefile
run:
	make $(BIN)/yess
	$(BIN)/run.sh

$(BIN)/yess: $(OBJ)
	g++ $(OBJ) -o yess
	mv yess $(BIN)

all: $(OBJ)

$(OBJD)/yess.o: $(HEADERALL)

$(OBJD)/Loader.o: $(HEADERALL)

$(OBJD)/String.o: $(HEADERALL)

$(OBJD)/Memory.o: $(HEADERALL)

$(OBJD)/Tools.o: $(HEADERALL)

$(OBJD)/RegisterFile.o: $(HEADERALL)

$(OBJD)/ConditionCodes.o: $(HEADERALL)

$(OBJD)/CPU.o: $(HEADERALL)

$(OBJD)/FetchStage.o: $(HEADERALL)

$(OBJD)/DecodeStage.o: $(HEADERALL)

$(OBJD)/ExecuteStage.o: $(HEADERALL)

$(OBJD)/MemoryStage.o: $(HEADERALL)

$(OBJD)/WritebackStage.o: $(HEADERALL)

$(OBJD)/F.o: $(HEADERALL)

$(OBJD)/D.o: $(HEADERALL)

$(OBJD)/E.o: $(HEADERALL)

$(OBJD)/M.o: $(HEADERALL)

$(OBJD)/W.o: $(HEADERALL)

$(OBJD)/PipeReg.o: $(HEADERALL)

$(OBJD)/PipeRegField.o: $(HEADERALL)

$(OBJD)/Stage.o: $(HEADERALL)

clean:
	rm -f $(OBJ)
	rm -f ../bin/yess



