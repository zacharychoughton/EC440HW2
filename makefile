override CFLAGS :=  -std=gnu99 -O0 -g $(CFLAGS) -I.
CC = gcc
#-Werror -Wall

threads.o: threads.c ec440threads.h

.PHONY: clean 

test_c_files=$(shell find tests -type f -name '*.c')
test_o_files=$(test_c_files:.c=.o)
test_files=$(test_c_files:.c=)

tests/%: tests/%.o threads.o
	$(CC) $(LDFLAGS) $+ $(LOADLIBES) $(LDLIBS) -o $@

checkprogs: $(test_files)

static_analysis:
	@echo "===== Running a static analyzer ====="
	# Analyze with clang-tidy. Ignore warnings about language extensions.
	# We use embedded assembly in a header file. An alternative is to build
	# from separate .s files and link those functions in.
	clang-tidy --checks=-clang-diagnostic-language-extension-token threads.c $(test_files) -- $(CFLAGS)
	@echo ""
	@echo "===== Running another static analyzer ====="
	cppcheck threads.c $(test_files)

check: checkprogs
	tests/run_tests.sh $(test_files)

clean:
	rm -f *.o $(test_files) $(test_o_files)

# clean:
# 	rm -f *.o *~ $(prog)