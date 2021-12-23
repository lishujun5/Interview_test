target = Problem_A_1 Problem_A_2
SRC_PATH = src
SRCS = $(wildcard $(SRC_PATH)/*.cc) main.cc
object = $(SRCS:.cc=.o)
cc = g++
compile_flags = -g -o2 -std=c++11 -I./
link_flags = -lpthread

.PYHONY : all
all : $(target)
Problem_A_1 : $(object)
	$(cc)  $^ -o $@ $(link_flags);
Problem_A_2 : $(object)
	$(cc)  $^ -o $@ $(link_flags);\
	rm -f $(SRCS:.cc=.d)

%.o : %.cc
	$(cc) -c $(compile_flags) $< -o $@

%.d: %.cc
	@set -e; rm -f $@; \
	$(CC) -MM -I./ $< > $@

sinclude $(object:.o=.d)

.PYHONY:clean
clean:
	-rm -f $(object) $(SRCS:.cc=.d)