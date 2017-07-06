EXEC   = sage 

OBJS   = 	./code/main.o \
			./code/core_read_parameter_file.o \
			./code/core_init.o \
			./code/core_io_tree.o \
			./code/core_cool_func.o \
			./code/core_build_model.o \
			./code/core_save.o \
			./code/core_mymalloc.o \
			./code/core_allvars.o \
			./code/model_infall.o \
			./code/model_cooling_heating.o \
			./code/model_starformation_and_feedback.o \
			./code/model_disk_instability.o \
			./code/model_reincorporation.o \
			./code/model_mergers.o \
			./code/model_misc.o

INCL   =	./code/core_allvars.h  \
			./code/core_proto.h  \
			./code/core_simulation.h  \
			./Makefile

CC       =   mpicc            # sets the C-compiler
OPTIMIZE =   -g -O0 -Wall     # optimization and warning flags
GSL_INCL = -I/opt/local/include   # make sure your system know what GSL_DIR is
GSL_LIBS = -L/opt/local/lib

LIBS   =   -g -lm  $(GSL_LIBS) -lgsl -lgslcblas 

CFLAGS =   $(OPTIONS) $(OPT) $(OPTIMIZE) $(GSL_INCL)

default: all

$(EXEC): $(OBJS) 
	$(CC) $(OPTIMIZE) $(OBJS) $(LIBS)   -o  $(EXEC)

$(OBJS): $(INCL) 

clean:
	rm -f $(OBJS)

tidy:
	rm -f $(OBJS) ./$(EXEC)

all:  tidy $(EXEC) clean
