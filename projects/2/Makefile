ifeq ($(OS),Windows_NT)
  EXE_SUFFIX=.exe
else
  UNAME_S := $(shell uname -s)
  ifeq ($(UNAME_S),Linux)
    EXE_SUFFIX=.linux
	CASAN = -fsanitize=address
    LASAN = -fsanitize=address
  endif
  ifeq ($(UNAME_S),Darwin)
    EXE_SUFFIX=.mac
	CASAN = -fsanitize=address
    LASAN = -fsanitize=address
  endif
endif

TARGETS = five_stage$(EXE_SUFFIX) trace_reader$(EXE_SUFFIX) trace_generator$(EXE_SUFFIX)

SHORT_TRACES_DIR = plot_traces
GNUPLOT = /afs/cs.pitt.edu/courses/1541/gnuplot-5.2.8/bin/gnuplot

CONFS = $(wildcard confs/*.conf)
TRACES = $(wildcard traces/*.tr)
OUTPUTS := $(foreach conf,$(CONFS),$(foreach trace, $(TRACES), outputs/$(trace:traces/%.tr=%).$(conf:confs/%.conf=%).out))
OUTPUTS_SOLUTION := $(foreach conf,$(CONFS),$(foreach trace, $(TRACES), outputs_solution/$(trace:traces/%.tr=%).$(conf:confs/%.conf=%).out))
DIFFS := $(foreach conf,$(CONFS),$(foreach trace, $(TRACES), diffs/$(trace:traces/%.tr=%).$(conf:confs/%.conf=%).diff))

SHORT_TRACES = $(wildcard $(SHORT_TRACES_DIR)/*.tr)
PLOT_CONFS = $(wildcard plot_confs/*.conf)
PLOT_OUTPUTS := $(foreach conf,$(PLOT_CONFS),$(foreach trace, $(SHORT_TRACES), plot_outputs/$(trace:$(SHORT_TRACES_DIR)/%.tr=%).$(conf:plot_confs/%.conf=%).out))
PLOT_OUTPUTS_SOLUTION := $(foreach conf,$(PLOT_CONFS),$(foreach trace, $(SHORT_TRACES), plot_outputs_solution/$(trace:$(SHORT_TRACES_DIR)/%.tr=%).$(conf:plot_confs/%.conf=%).out))

COPT = -g -Wall $(CASAN) -Wno-format-security -std=c++11 `pkg-config --cflags glib-2.0`
LOPT = -g $(LASAN) `pkg-config --libs glib-2.0`
CC = g++

all: build run
build: $(TARGETS)
run: $(OUTPUTS) $(OUTPUTS_SOLUTION) $(DIFFS)
plots: $(PLOT_OUTPUTS) $(PLOT_OUTPUTS_SOLUTION)

five_stage.o: config.h CPU.h MemObj.h MemRequest.h
trace_reader.o: CPU.h trace.h
trace_generator.o: CPU.h trace.h
config.o: config.h
CPU.o: config.h trace.h CPU.h Cache.h CacheCore.h CacheLine.h Counter.h DRAM.h WriteBuffer.h MemObj.h MemRequest.h log2i.h
Cache.o: config.h Cache.h CacheCore.h CacheLine.h Counter.h MemObj.h MemRequest.h log2i.h
CacheCore.o: CacheCore.h CacheLine.h log2i.h
MemObj.o: Cache.h CacheCore.h CacheLine.h Counter.h DRAM.h WriteBuffer.h MemObj.h MemRequest.h log2i.h

five_stage$(EXE_SUFFIX): five_stage.o config.o CPU.o trace.o CacheCore.o Cache.o MemObj.o log2i.o
	$(CC) $^ $(LOPT) -o $@

trace_reader$(EXE_SUFFIX): trace_reader.o trace.o
	$(CC) $^ $(LOPT) -o $@

trace_generator$(EXE_SUFFIX): trace_generator.o trace.o
	$(CC) $^ $(LOPT) -o $@

%.o: %.cpp
	$(CC) -c $(COPT) $<

define run_rules
outputs/$(1:traces/%.tr=%).$(2:confs/%.conf=%).out: five_stage$(EXE_SUFFIX) $(1) $(2)
	@echo "Running ./five_stage$(EXE_SUFFIX) -t $(1) -c $(2) -d > $$@"
	-@./five_stage$(EXE_SUFFIX) -t $(1) -c $(2) -d > $$@

outputs_solution/$(1:traces/%.tr=%).$(2:confs/%.conf=%).out: five_stage_solution$(EXE_SUFFIX) $(1) $(2)
	@echo "Running ./five_stage_solution$(EXE_SUFFIX) -t $(1) -c $(2) -d > $$@"
	-@./five_stage_solution$(EXE_SUFFIX) -t $(1) -c $(2) -d > $$@
endef

$(foreach trace,$(TRACES),$(foreach conf, $(CONFS), $(eval $(call run_rules,$(trace),$(conf)))))

define diff_rules
diffs/$(1:traces/%.tr=%).$(2:confs/%.conf=%).diff: outputs/$(1:traces/%.tr=%).$(2:confs/%.conf=%).out
	@echo "Running diff -dwy -W 170 $$< outputs_solution/$(1:traces/%.tr=%).$(2:confs/%.conf=%).out > $$@"
	-@diff -dwy -W 120 $$< outputs_solution/$(1:traces/%.tr=%).$(2:confs/%.conf=%).out > $$@
endef

$(foreach trace,$(TRACES),$(foreach conf, $(CONFS), $(eval $(call diff_rules,$(trace),$(conf)))))

define plot_rules
plot_outputs/$(1:$(SHORT_TRACES_DIR)/%.tr=%).$(2:plot_confs/%.conf=%).out: five_stage$(EXE_SUFFIX) $(1) $(2)
	@echo "Running ./five_stage$(EXE_SUFFIX) -t $(1) -c $(2) > $$@"
	-@./five_stage$(EXE_SUFFIX) -t $(1) -c $(2) > $$@

plot_outputs_solution/$(1:$(SHORT_TRACES_DIR)/%.tr=%).$(2:plot_confs/%.conf=%).out: five_stage_solution$(EXE_SUFFIX) $(1) $(2)
	@echo "Running ./five_stage_solution$(EXE_SUFFIX) -t $(1) -c $(2) > $$@"
	-@./five_stage_solution$(EXE_SUFFIX) -t $(1) -c $(2) > $$@
endef

$(foreach trace,$(SHORT_TRACES),$(foreach conf, $(PLOT_CONFS), $(eval $(call plot_rules,$(trace),$(conf)))))

%.pdf: %.dat
	$(GNUPLOT) -e "inputFile='$<'" -e "outputFile='$@'" generate_plot.plt

IPC.dat: $(PLOT_OUTPUTS)
	python generate_plot.py -i plot_outputs -o $@

IPC_solution.dat: $(PLOT_OUTPUTS_SOLUTION)
	python generate_plot.py -i plot_outputs_solution -o $@


clean:
	rm -f $(TARGETS) *.o $(OUTPUTS) $(PLOT_OUTPUTS) $(DIFFS) *.pdf *.dat

distclean: clean
	rm -f $(OUTPUTS_SOLUTION) $(PLOT_OUTPUTS_SOLUTION)
