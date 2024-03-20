TARGETS = five_stage trace_reader trace_generator

ifeq ($(OS),Windows_NT)
	FIVE_STAGE_SOLUTION = ./five_stage_solution.exe
else
UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        FIVE_STAGE_SOLUTION = ./five_stage_solution.linux
        CASAN = -fsanitize=address
        LASAN = -fsanitize=address
    endif
    ifeq ($(UNAME_S),Darwin)
        FIVE_STAGE_SOLUTION = ./five_stage_solution.mac
        CASAN = -fsanitize=address
        LASAN = -fsanitize=address
    endif
endif

PLOT_TRACES_DIR = plot_traces
GNUPLOT = gnuplot

SOURCES = $(wildcard *.cpp)
OBJECTS = $(SOURCES:%.cpp=%.o)
CONFS = $(wildcard confs/*.conf)
TRACES = $(wildcard traces/*.tr)
OUTPUTS := $(foreach conf,$(CONFS),$(foreach trace, $(TRACES), outputs/$(trace:traces/%.tr=%).$(conf:confs/%.conf=%).out))
OUTPUTS_SOLUTION := $(foreach conf,$(CONFS),$(foreach trace, $(TRACES), outputs_solution/$(trace:traces/%.tr=%).$(conf:confs/%.conf=%).out))
DIFFS := $(foreach conf,$(CONFS),$(foreach trace, $(TRACES), diffs/$(trace:traces/%.tr=%).$(conf:confs/%.conf=%).diff))

PLOT_TRACES = $(wildcard $(PLOT_TRACES_DIR)/*.tr)
PLOT_CONFS = $(wildcard plot_confs/*.conf)
PLOT_OUTPUTS := $(foreach conf,$(PLOT_CONFS),$(foreach trace, $(PLOT_TRACES), plots/$(trace:$(PLOT_TRACES_DIR)/%.tr=%).$(conf:plot_confs/%.conf=%).out))
PLOT_OUTPUTS_SOLUTION := $(foreach conf,$(PLOT_CONFS),$(foreach trace, $(PLOT_TRACES), plots_solution/$(trace:$(PLOT_TRACES_DIR)/%.tr=%).$(conf:plot_confs/%.conf=%).out))

COPT = -g -Wall $(CASAN) `pkg-config --cflags glib-2.0`
LOPT = -g $(LASAN) `pkg-config --libs glib-2.0`
CC = g++

all: build run
build: $(TARGETS)
run: $(OUTPUTS) $(OUTPUTS_SOLUTION) $(DIFFS)
plots: IPC.pdf IPC_solution.pdf

five_stage.o: CPU.h config.h
config.o: config.h
CPU.o: CPU.h config.h trace.h
trace_reader.o: CPU.h trace.h

five_stage: five_stage.o config.o CPU.o trace.o
	$(CC) $^ $(LOPT) -o $@

trace_reader: trace_reader.o config.o CPU.o trace.o
	$(CC) $^ $(LOPT) -o $@

trace_generator: trace_generator.o config.o CPU.o trace.o
	$(CC) $^ $(LOPT) -o $@

%.o: %.cpp
	$(CC) -c $(COPT) $<

define run_rules
outputs/$(1:traces/%.tr=%).$(2:confs/%.conf=%).out: five_stage $(1) $(2)
	@echo "Running ./five_stage -t $(1) -c $(2) -d > $$@"
	-@./five_stage -t $(1) -c $(2) -d > $$@

outputs_solution/$(1:traces/%.tr=%).$(2:confs/%.conf=%).out: $(FIVE_STAGE_SOLUTION) $(1) $(2)
	@echo "Running $(FIVE_STAGE_SOLUTION) -t $(1) -c $(2) -d > $$@"
	-@$(FIVE_STAGE_SOLUTION) -t $(1) -c $(2) -d > $$@
endef

$(foreach trace,$(TRACES),$(foreach conf, $(CONFS), $(eval $(call run_rules,$(trace),$(conf)))))

define diff_rules
diffs/$(1:traces/%.tr=%).$(2:confs/%.conf=%).diff: outputs/$(1:traces/%.tr=%).$(2:confs/%.conf=%).out
	@echo "Running diff -dwy -W 170 $$< outputs_solution/$(1:traces/%.tr=%).$(2:confs/%.conf=%).out > $$@"
	-@diff -dwy -W 170 $$< outputs_solution/$(1:traces/%.tr=%).$(2:confs/%.conf=%).out > $$@
endef

$(foreach trace,$(TRACES),$(foreach conf, $(CONFS), $(eval $(call diff_rules,$(trace),$(conf)))))

define plot_rules
plots/$(1:$(PLOT_TRACES_DIR)/%.tr=%).$(2:plot_confs/%.conf=%).out: five_stage $(1) $(2)
	@echo "Running ./five_stage -t $(1) -c $(2) > $$@"
	-@./five_stage -t $(1) -c $(2) > $$@

plots_solution/$(1:$(PLOT_TRACES_DIR)/%.tr=%).$(2:plot_confs/%.conf=%).out: $(FIVE_STAGE_SOLUTION) $(1) $(2)
	@echo "Running $(FIVE_STAGE_SOLUTION) -t $(1) -c $(2) > $$@"
	-@$(FIVE_STAGE_SOLUTION) -t $(1) -c $(2) > $$@
endef

$(foreach trace,$(PLOT_TRACES),$(foreach conf, $(PLOT_CONFS), $(eval $(call plot_rules,$(trace),$(conf)))))

%.pdf: %.dat
	$(GNUPLOT) -e "inputFile='$<'" -e "outputFile='$@'" generate_plot.plt

IPC.dat: $(PLOT_OUTPUTS)
	python generate_plot.py -i plots -o $@

IPC_solution.dat: $(PLOT_OUTPUTS_SOLUTION)
	python generate_plot.py -i plots_solution -o $@


clean:
	rm -f $(TARGETS) $(OBJECTS) $(OUTPUTS) $(PLOT_OUTPUTS) $(DIFFS) IPC.pdf IPC.dat

distclean: clean
	rm -f $(OUTPUTS_SOLUTION) $(PLOT_OUTPUTS_SOLUTION)
