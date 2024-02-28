TARGETS = branch

CC = g++
COPT = -pthread
LOPT = -pthread
TYPES = sort nosort
SIZES = 100000 
PERF_DATA := $(foreach type,$(TYPES),$(foreach size, $(SIZES), outputs/$(type).$(size).perf))

all: $(TARGETS) $(PERF_DATA)

build: $(TARGETS)

%.o: %.c
	$(CC) $(COPT) -c $<

branch: branch.o
	$(CC) $(LOPT) $^ -o $@

$(foreach target,$(TARGETS), $(eval $(call link_rules,$(target))))

define perf_rules
outputs/$(1).$(2).perf: $(TARGETS)
	perf record -s -e cycles,instructions,branches,branch-misses ./branch $(2) $(1)
	perf report -T -i perf.data > $$@
	rm -f perf.data
endef

$(foreach type,$(TYPES),$(foreach size, $(SIZES), $(eval $(call perf_rules,$(type),$(size)))))

clean:
	rm -f $(TARGETS) $(PERF_DATA) $(CACHE_DATA) *.pdf *.dat *.o
