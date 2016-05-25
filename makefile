ORACLE_INC_DIR=/usr/local/include/oracle
OCCI_DIR=./occi
LOG_DIR=./log
CXX=g++ -std=c++11
INC_PATH=-I$(ORACLE_INC_DIR) -I./

CXXFLAGS=-Wall -gdwarf-3 $(INC_PATH) -DBOOST_LOG_DYN_LINK
LDFLAGS=-Wl,-rpath=./libs -L./libs
LDLIBS=-locci -lclntsh -pthread -lboost_log

OCCI_OBJ=$(OCCI_DIR)/OcciConnectPool.o \
		 $(OCCI_DIR)/OcciClient.o      \
		 $(OCCI_DIR)/OcciOperator.o    \
		 $(LOG_DIR)/Log.o

main:main.o $(OCCI_OBJ)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(LDLIBS) $^ -o $@ 
%.o:%.cpp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(LDLIBS) -c $^ -o $@
#
#-include $(OCCI_DEPEND)
#%.d:%.cpp
#	set -e; $(RM) -f $@*;\
#	$(CXX) -MM $(CXXFLAGS) $< > $@.$$$$;\
#	sed 's/\([A-Za-z0-9]\)\.o/\1\.d/' < $@.$$$$ > $@;\
#	sed r $@.$$$$ >> $@;\
#	$(RM) -f $@.$$$$;\
#	sed -i '$$a \\t$(CXX) $(CXXFLAGS) $(LDFLAGS) $(LDLIBS) -c $^ -o $@'  $@

.PHONY:clean
clean:
	rm -rf *.o
	rm -rf $(OCCI_OBJ)
	rm -rf main

