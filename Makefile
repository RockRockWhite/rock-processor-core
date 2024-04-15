SIM = ./sim
SRC = ./src
LIB = ../lib
DISPLAY = :0.0
OBJ_DIR = ./obj_dir
WAVEFORM_DIR = ./waveform
NVBOARD_DIR = ./nvboard

.PHONY: header
header: $(SRC)/$(NAME).v
	@echo "Generating header file for $(NAME)..."
	verilator -cc $< -I$(SRC)

.PHONY: sim
sim: $(SRC)/$(NAME).v \
	$(SIM)/$(NAME)_tb.cpp

	@echo "Building and running simulation..."
	verilator -cc --exe --build  --trace $^ -I$(SRC)
	@echo "Done! Running simulation..."
	mkdir -p $(WAVEFORM_DIR)
	cd $(WAVEFORM_DIR) && ../$(OBJ_DIR)/V$(NAME)
	@echo "Done! Displaying waveform..."
	export DISPLAY=$(DISPLAY) && gtkwave $(WAVEFORM_DIR)/$(NAME).vcd

.PHONY: nvboard
nvboard: $(SRC)/$(NAME).v \
	$(NVBOARD_DIR)/$(NAME)_nvboard.cpp
	@echo "Building and running simulation..."
	verilator -cc --exe --build  --trace $^ $(NVBOARD_HOME)/build/nvboard.a \
		-I$(SRC) \
		-CFLAGS -I$(NVBOARD_HOME)/usr/include \
		-CFLAGS -I$(CURDIR)/include \
		-LDFLAGS -lSDL2 -LDFLAGS -lSDL2_image -LDFLAGS -lSDL2_ttf
	@echo "Done! Running nvboard..."
	mkdir -p $(WAVEFORM_DIR)
	cd $(WAVEFORM_DIR) && export DISPLAY=$(DISPLAY) && ../$(OBJ_DIR)/V$(NAME)

.PHONY: clean
clean:
	rm -rf $(OBJ_DIR)
	rm -rf $(WAVEFORM_DIR)