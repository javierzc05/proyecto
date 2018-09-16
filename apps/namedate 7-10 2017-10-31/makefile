# makefile for NameDate 7
# James Greene

CC 		=	gcc.exe
# CFLAGS	=	-O0 -Wall -fmessage-length=0 -std=gnu99 -D "_DEBUG"
CFLAGS  =	-O3 -Wall -fmessage-length=0 -std=gnu99 -D "NDEBUG"
INCLUDES=
LFLAGS	=
RC		=	windres.exe
# RFLAGS	=	-l 0x409 -D "_DEBUG"
RFLAGS	=	-l 0x409 -D "NDEBUG"
LIBS	=
OBJDIR	=	Release
RM		=	del

default:	all

SRCS	=	NameDate.c ndutils.c cmdlntool.c getexif.c
RSC		=	namedate.rc
RSCO	=	namedate.ro
OBJS	=	$(SRCS:.c=.o) 
EXE		=	NameDate.exe

#
# Everything below should be automatic if everything above is correct.
# 

.PHONY:	depend clean

all:	$(OBJDIR)/$(EXE)
	@echo	NameDate has been compiled
		
clean:
	@echo	Cleaning NameDate
	$(RM) $(OBJDIR)\*.o
	$(RM) $(OBJDIR)\$(RSCO)
	$(RM) $(OBJDIR)\$(EXE)

$(OBJDIR)/$(EXE):	$(addprefix $(OBJDIR)/, $(OBJS) $(RSCO))
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $(addprefix $(OBJDIR)/, $(OBJS) $(RSCO)) $(LFLAGS) $(LIBS)
	
$(OBJDIR)/%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@

$(OBJDIR)/$(RSCO): $(RSC)
	$(RC) $(RFLAGS) -o $@ -i $< 

	
depend: $(SRCS)

# End of makefile
