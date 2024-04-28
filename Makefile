SRC = $(CURDIR)/src
BUILD_DIR = $(CURDIR)/build
WAVEFORM_DIR = ./waveform
NVBOARD_DIR = $(CURDIR)/nvboard
ROM_DIR = ./rom
TOOLS_DIR = ./tools

SIM_DIR = $(CURDIR)/sim
SIM_SRC = $(SIM_DIR)/src
SIM_INC = $(SIM_DIR)/include
SIM_INC += /usr/share/verilator/include/
SIM_INC += /usr/share/verilator/include/vltstd/
SIM_INC += $(BUILD_DIR)

CFLAGS = -O0
CFLAGS += -g
CFLAGS += $(addprefix -I, $(SIM_INC))
CFLAGS += -std=c++20
CFLAGS += -Wall
CFLAGS += -Werror
CFLAGS += -D__GUEST_ISA__=riscv32

LDFLAGS = -L$(BUILD_DIR)
LDFLAGS += -lverilated

SRCS = $(wildcard $(SIM_SRC)/*.cpp)
OBJS = $(SRCS:$(SIM_SRC)/%.cpp=$(BUILD_DIR)/%.o)

CXX = g++
LD := $(CXX)
BINARY = $(BUILD_DIR)/rpc
DISPLAY = :0.0  

$(BUILD_DIR)/V%.h: $(SRC)/%.v
	@echo "verilator $<"
	@verilator -cc --build --trace -I$(SRC) -Mdir $(BUILD_DIR) -CFLAGS "$(CFLAGS)" $<


$(BUILD_DIR)/%.o: $(SIM_SRC)/%.cc
	@mkdir -p $(BUILD_DIR)
	@echo "CXX $<"
	@$(CXX) $(CFLAGS) -I/usr/include -std=c++17 -fno-exceptions -D_GNU_SOURCE -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS -fPIE -D__GUEST_ISA__=riscv32  -c $< -o $@
	
$(BUILD_DIR)/%.o: $(SIM_SRC)/%.cpp
	@mkdir -p $(BUILD_DIR)
	@echo "CXX $<"
	@$(CXX) $(CFLAGS) -c $< -o $@

# Depencies
-include $(OBJS:.o=.d)


$(BINARY): $(BUILD_DIR)/VProcessorCore.h $(OBJS)
	@echo LD $(OBJS)
	$(LD) $(OBJS) -o $@ $(LDFLAGS) -lVProcessorCore -lLLVM-16

$(BUILD_DIR)/%_tb.o: $(SIM_DIR)/module_test/%_tb.cpp $(BUILD_DIR)/V%.h
	@echo "CXX $<"
	@$(CXX) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%_tb: $(BUILD_DIR)/%_tb.o
	@echo LD $<
	@$(LD) $< -o $@ $(CFLAGS) $(LDFLAGS) -lV$*

.PHONY: all
all: $(BINARY)

.PHONY: run
run: $(BINARY)
	@echo "Building and running simulation..."
	$< --img $(IMG) $(RPC_ARGS)

.PHONY: gdb
gdb: $(BINARY)
	@echo "Building and running simulation via gdb..."
	gdbserver :1234 $< --img $(IMG) $(RPC_ARGS)

.PHONY: header
header: $(BUILD_DIR)/V$(NAME).h
	@echo "Generating header file for $(NAME)..."

# makefile for module test
.PHONY: module_test
module_test: $(BUILD_DIR)/$(NAME)_tb
	@echo "Building and running simulation..."
	mkdir -p $(WAVEFORM_DIR)
	cd $(WAVEFORM_DIR) && $<
	@echo "Done! Displaying waveform..."
	export DISPLAY=$(DISPLAY) && gtkwave $(WAVEFORM_DIR)/$(NAME).vcd

$(ROM_DIR)/%.rom: $(ROM_DIR)/asm/%.asm
	@echo "Generating ROM file..."
	java -jar $(TOOLS_DIR)/venus.jar $< --dump > $@

# $(BUILD_DIR)/disasm.o: $(SIM_DIR)/src/disasm.cc
# 	@echo "Building disassembler..."
# 	g++ -c $<  $(shell llvm-config --cxxflags) -fPIE $(shell llvm-config --libs)

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)
	rm -rf $(WAVEFORM_DIR)

# old mkfile
.PHONY: rom
rom: $(ROM_DIR)/$(NAME).rom

.PHONY: nvboard
nvboard: $(SRC)/$(NAME).v \
	$(NVBOARD_DIR)/$(NAME)_nvboard.cpp
	@echo "Building and running simulation..."
	verilator -cc --exe --build --trace $^ $(NVBOARD_HOME)/build/nvboard.a \
		-I$(SRC) \
		-CFLAGS -std=c++20 \
		-CFLAGS -I$(NVBOARD_HOME)/usr/include \
		-CFLAGS -I$(CURDIR)/nvboard/include \
		-LDFLAGS -lSDL2 -LDFLAGS -lSDL2_image -LDFLAGS -lSDL2_ttf
	@echo "Done! Running nvboard..."
	mkdir -p $(WAVEFORM_DIR)
	cd $(WAVEFORM_DIR) && export DISPLAY=$(DISPLAY) && ../$(BUILD_DIR)/V$(NAME)





