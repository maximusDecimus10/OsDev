ASM=nasm

SRC_DIR=src
BUILD_DIR=build

.PHONY: all floppy_image kernel bootloader clean always

#
# Floppy image
#
floppy_image: $(BUILD_DIR)/main_floppy.img

$(BUILD_DIR)/main_floppy.img: bootloader kernel
	dd if=/dev/zero of=build/main_floppy.img bs=512 count=2880
	mkfs.fat -F 12 -n "NBOS" build/main_floppy.img
	dd if=build/bootloader.bin of=build/main_floppy.img conv=notrunc
	mv build/kernel.bin build/KERNEL.BIN
	mcopy -i build/main_floppy.img build/KERNEL.BIN "::"


#
# Bootloader
#
bootloader: $(BUILD_DIR)/bootloader.bin

$(BUILD_DIR)/bootloader.bin: always
	$(ASM) $(SRC_DIR)/bootloader/boot.asm -f bin -o $(BUILD_DIR)/bootloader.bin

#
# Kernel
#
kernel: $(BUILD_DIR)/kernel.bin

$(BUILD_DIR)/kernel.bin: always
	$(ASM) $(SRC_DIR)/kernel/main.asm -f bin -o $(BUILD_DIR)/kernel.bin

#
# Always
#
always:
	mkdir -p $(BUILD_DIR)

#
# Clean
#
clean:
	rm -rf $(BUILD_DIR)/*
# ASM = nasm
# BUILD_DIR = build
# SRC_DIR = src
# BOOTLOADER = $(BUILD_DIR)/bootloader.bin
# KERNEL = $(BUILD_DIR)/kernel.bin
# IMAGE = $(BUILD_DIR)/main_floppy.img

# .PHONY: all clean always

# all: $(IMAGE)

# #
# # Floppy image
# #
# $(IMAGE): $(BOOTLOADER) $(KERNEL)
# 	@echo "[+] Creating empty floppy image"
# 	dd if=/dev/zero of=$(IMAGE) bs=512 count=2880

# 	@echo "[+] Formatting image with FAT12"
# 	mkfs.fat -F 12 -n "NBOS" $(IMAGE)

# 	@echo "[+] Writing bootloader to boot sector"
# 	dd if=$(BOOTLOADER) of=$(IMAGE) conv=notrunc

# 	@echo "[+] Copying kernel into image"
# 	mcopy -o -i $(IMAGE) $(KERNEL) ::KERNEL.BIN

# #
# # Bootloader
# #
# $(BOOTLOADER): always
# 	@echo "[+] Assembling bootloader"
# 	$(ASM) $(SRC_DIR)/bootloader/boot.asm -f bin -o $(BOOTLOADER)

# #
# # Kernel
# #
# $(KERNEL): always
# 	@echo "[+] Assembling kernel"
# 	$(ASM) $(SRC_DIR)/kernel/main.asm -f bin -o $(KERNEL)

# #
# # Helper
# #
# always:
# 	@mkdir -p $(BUILD_DIR)

# clean:
# 	@rm -rf $(BUILD_DIR)
