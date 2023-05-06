target=cadd0023_beta
src_dir=./source/
inc_dir=./include/
deg_dir=./debug/

lef_deg_dir=./lef/lib/liblef.a
def_deg_dir=./def/lib/libdef.a
lef_inc_dir=./lef/include/
def_inc_dir=./def/include/

src=$(wildcard $(src_dir)*cpp)
obj=$(patsubst %.cpp,$(deg_dir)%.o,$(notdir $(src)))
CFLAGS += -I$(inc_dir)
CFLAGSLEF += -I$(lef_inc_dir)
CFLAGSDEF += -I$(def_inc_dir)

$(target) : $(obj)
	g++  $^ $(lef_deg_dir) $(def_deg_dir) -O3 -no-pie -o $@
$(deg_dir)%.o : $(src_dir)%.cpp
	g++  $(CFLAGS) $(CFLAGSLEF) $(CFLAGSDEF) -c $< -o $@
.PHONY:clean
clean:
	-rm -f $(target) $(obj) $(dep)
