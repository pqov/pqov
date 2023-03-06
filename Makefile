CC  ?= clang
CXX ?= clang++
LD  = $(CC)


ifndef PROJ
PROJ = ref
#PROJ = amd64
#PROJ = ssse3
#PROJ = avx2
endif


SRC_DIR  = ./src
UTIL_DIR = ./utils


CFLAGS   := -O3 $(CFLAGS) -std=c11 -Wall -Wextra -Wpedantic -Werror -fno-omit-frame-pointer
CXXFLAGS := -O3 $(CPPFLAGS) -Wall -Wextra -fno-exceptions -fno-rtti -nostdinc++
INCPATH  := -I/usr/local/include -I/opt/local/include -I/usr/include -I$(SRC_DIR) -I$(UTIL_DIR) -Iunit_tests -Ibenchmark
LIBPATH  = -L/usr/local/lib -L/opt/local/lib -L/usr/lib
LIBS     = -lcrypto


ifdef ASAN
CFLAGS  += -fsanitize=address -fno-sanitize-recover=all
LDFLAGS += -fsanitize=address -fno-sanitize-recover=all
endif

ifdef UBSAN
CFLAGS  += -fsanitize=undefined -fno-sanitize-recover=all
LDFLAGS += -fsanitize=undefined -fno-sanitize-recover=all
endif

OS := $(shell uname -s)
ifeq  ($(OS), Darwin)
CFLAGS    +=  -D_MAC_OS_
CXXFLAGS  +=  -D_MAC_OS_
endif


ifeq ($(PROJ),ref)

SRC_EXT_DIRS  = ./src/ref
INCPATH      += -I./src/ref

else ifeq ($(PROJ),amd64)

SRC_EXT_DIRS  = ./src/ref ./src/amd64
INCPATH      += -I./src/ref -I./src/amd64

CFLAGS    += -D_BLAS_UINT64_
CXXFLAGS  += -D_BLAS_UINT64_

else ifeq ($(PROJ),neon)

ifeq ($(OS), Darwin)
SRC_EXT_DIRS  = ./src/ref ./src/amd64 ./src/neon  ./utils/neon_aesinst
INCPATH      += -I./src/ref -I./src/amd64 -I./src/neon  -I./utils/neon_aesinst
CFLAGS    += -D_BLAS_NEON_ -D_UTILS_NEONAES_
CXXFLAGS  += -D_BLAS_NEON_ -D_UTILS_NEONAES_
else
SRC_EXT_DIRS  = ./src/ref ./src/amd64 ./src/neon  ./utils/neon_aesffs
INCPATH      += -I./src/ref -I./src/amd64 -I./src/neon  -I./utils/neon_aesffs
CFLAGS    += -D_BLAS_NEON_ -D_UTILS_NEONBSAES_
CXXFLAGS  += -D_BLAS_NEON_ -D_UTILS_NEONBSAES_
ifeq ($(CC),clang)
CFLAGS    += -flax-vector-conversions
CXXFLAGS  += -flax-vector-conversions
else
CFLAGS    += march=armv7-a -mfpu=neon -flax-vector-conversions
CXXFLAGS  += march=armv7-a -mfpu=neon -flax-vector-conversions
endif
endif

ifeq ($(PARAM),1)
CFLAGS    += -D_MUL_WITH_MULTAB_
CXXFLAGS  += -D_MUL_WITH_MULTAB_
endif

else ifeq ($(PROJ),ssse3)

SRC_EXT_DIRS  = ./src/ref ./src/amd64 ./src/ssse3
INCPATH      += -I./src/ref -I./src/amd64 -I./src/ssse3

CFLAGS    += -mssse3  -D_BLAS_SSE_ -D_MUL_WITH_MULTAB_
CXXFLAGS  += -mssse3  -D_BLAS_SSE_ -D_MUL_WITH_MULTAB_

else ifeq ($(PROJ),avx2)

SRC_EXT_DIRS  = ./src/ref ./src/amd64 ./src/ssse3 ./src/avx2 ./utils/x86aesni
INCPATH      += -I./src/ref -I./src/amd64 -I./src/ssse3 -I./src/avx2 -I./utils/x86aesni
CFLAGS       += -mavx2 -maes -D_BLAS_AVX2_ -D_MUL_WITH_MULTAB_ -D_UTILS_AESNI_
CXXFLAGS     += -mavx2 -maes -D_BLAS_AVX2_ -D_MUL_WITH_MULTAB_ -D_UTILS_AESNI_

endif


SRCS    :=  $(foreach dir, $(SRC_EXT_DIRS), $(wildcard  $(dir)/*.c) )
SRCS    +=  $(wildcard $(SRC_DIR)/*.c)   $(wildcard $(UTIL_DIR)/*.c)

SRCS_O         :=  $(SRCS:.c=.o)
SRCS_O_NOTDIR  :=  $(notdir $(SRCS_O))


ifdef PARAM
ifeq ($(PARAM),3)
CFLAGS    += -D_OV256_112_44
CXXFLAGS  += -D_OV256_112_44
else ifeq ($(PARAM),4)
CFLAGS    += -D_OV256_184_72
CXXFLAGS  += -D_OV256_184_72
else ifeq ($(PARAM),5)
CFLAGS    += -D_OV256_244_96
CXXFLAGS  += -D_OV256_244_96
else
CFLAGS    += -D_OV16_160_64
CXXFLAGS  += -D_OV16_160_64
endif
else
PARAM=3
endif


ifdef VARIANT
ifeq ($(VARIANT),2)
CFLAGS += -D_OV_PKC
CXXFLAGS += -D_OV_PKC
else ifeq ($(VARIANT),3)
CFLAGS += -D_OV_PKC_SKC
CXXFLAGS += -D_OV_PKC_SKC
else
CFLAGS += -D_OV_CLASSIC
CXXFLAGS += -D_OV_CLASSIC
endif
else
VARIANT=1
endif



OBJ = $(SRCS_O_NOTDIR)

ifeq  ($(OS), Darwin)
  OBJ += m1cycles.o
endif


EXE= format sign_api-test sign_api-benchmark rec-sign-benchmark

ifdef DEBUG
        CFLAGS+=  -D_DEBUG_ -g
        CXXFLAGS+= -D_DEBUG_ -g
endif

ifdef KAT
	CFLAGS       += -D_NIST_KAT_
	CXXFLAGS     += -D_NIST_KAT_
	SRC_EXT_DIRS += ./utils/nistkat
	INCPATH      += -I./utils/nistkat
	OBJ          += rng.o
	EXE          += PQCgenKAT_sign
endif

ifdef AVX2
	CFLAGS += -mavx2 -D_USE_AVX2_
	CXXFLAGS += -mavx2 -D_USE_AVX2_
endif

ifdef AVX
	CFLAGS += -mavx -D_USE_AVX_
	CXXFLAGS += -mavx -D_USE_AVX_
endif

ifdef GPROF
	CFLAGS += -pg
	CXXFLAGS += -pg
	LDFLAGS += -pg
endif

ifdef VALGRIND
	CFLAGS   += -D_VALGRIND_ #-g
	CXXFLAGS += -D_VALGRIND_ #-g
	CFLAGS   := $(CFLAGS:-O%=-O1)
	CXXFLAGS := $(CXXFLAGS:-O%=-O1)
endif


.INTERMEDIATE:  $(OBJ)

.PHONY: all tests tables clean format

all: $(EXE)

%-test: $(OBJ) %-test.o
	$(LD) $(LDFLAGS) $(LIBPATH) -o $@ $^ $(LIBS)

%-benchmark: $(OBJ) %-benchmark.o
	$(LD) $(LDFLAGS) $(LIBPATH) -o $@ $^ $(LIBS)

PQCgenKAT_sign: $(OBJ) PQCgenKAT_sign.o
	$(LD) $(LDFLAGS) $(LIBPATH) -o $@ $^ $(LIBS)

PQCgenKAT_sign.o: nist-submission-project/PQCgenKAT_sign.c
	$(CC) $(CFLAGS) $(INCPATH) -c $<

%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $(INCPATH) -c $<

%.o: $(UTIL_DIR)/%.c
	$(CC) $(CFLAGS) $(INCPATH) -c $<

%.o: unit_tests/%.c
	$(CC) $(CFLAGS) $(INCPATH) -c $<

%.o: benchmark/%.c
	$(CC) $(CFLAGS) $(INCPATH) -c $<

m1cycles.o: benchmark/m1cycles.c
	$(CC) $(CFLAGS) $(INCPATH) -c $<

%.s: unit_tests/%.c
	$(CC) $(CFLAGS) $(INCPATH) -S -c $<

%.s: benchmark/%.c
	$(CC) $(CFLAGS) $(INCPATH) -S -c $<


define GEN_O
%.o: $(1)/%.c
	$(CC) $(CFLAGS) $(INCPATH) -c $$<
endef
$(foreach dir, $(SRC_EXT_DIRS), $(eval $(call GEN_O,$(dir))))


%.o: %.c
	$(CC) $(CFLAGS) $(INCPATH) -c $<

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCPATH) -c $<

test: sign_api-test
	./sign_api-test



check-NISTKAT: PQCgenKAT_sign
	./check-vector.sh $(PARAM) $(VARIANT)

tests:
	cd tests; make

tables:
	cd supplement; make

valgrind: sign_api-test
	valgrind --leak-check=yes --track-origins=yes -s ./sign_api-test

format:
	astyle --project src/*.[chs] src/**/*.[chs] utils/*.[chs] utils/**/*.[chs] unit_tests/*.[chs] benchmark/*.[chs]

clean:
	-rm -f *.o *-test *-benchmark massif.out.* *.req *.rsp PQCgenKAT_sign $(EXE);
