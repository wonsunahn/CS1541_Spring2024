TARGETS = five_stage trace_reader trace_generator

CONFS = $(wildcard confs/*.conf)
TRACES = $(wildcard traces/*.tr)
OUTPUTS := $(foreach conf,$(CONFS),$(foreach trace, $(TRACES), outputs/$(trace:traces/%.tr=%).$(conf:confs/%.conf=%).out))

COPT = -g -Wall -Wno-format-security -std=c++11 -I/usr/include/glib-2.0/ -I/usr/lib/x86_64-linux-gnu/glib-2.0/include/
LOPT = -lglib-2.0
CC = g++

all: $(TARGETS) run
run: $(OUTPUTS) 

define run_rules
outputs/$(1:traces/%.tr=%).$(2:confs/%.conf=%).out: five_stage $(1) $(2)
	@echo "Running ./five_stage -t $(1) -c $(2) -d > $$@"
	-@./five_stage -t $(1) -c $(2) -d > $$@
endef

$(foreach trace,$(TRACES),$(foreach conf, $(CONFS), $(eval $(call run_rules,$(trace),$(conf)))))

clean:
	rm -f $(OUTPUTS)
