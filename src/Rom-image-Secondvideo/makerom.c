#include <stdio.h>
#include <stdint.h>

int main() {
    uint8_t code[] = {
        0xa9, 0xff, // lda #$ff
        0x8d, 0x02, 0x60, // sta $6002
        0xa9, 0x55, // lda #$55
        0x8d, 0x00, 0x60, // sta $6000
        0xa9, 0xaa, // lda #$aa
        0x8d, 0x00, 0x60, // sta $6000
        0x4c, 0x05, 0x80, // jmp $8005
    };

    const int code_len = sizeof(code);
    const int rom_size = 32768;

    uint8_t rom[rom_size];

    // Copy the code into the ROM
    for (int i = 0; i < code_len; i++) {
        rom[i] = code[i];
    }

    // Fill the remaining ROM with 0xea (NOP)
    for (int i = code_len; i < rom_size; i++) {
        rom[i] = 0xea;
    }

    // Set the reset vector
    rom[0x7ffc] = 0x00;
    rom[0x7ffd] = 0x80;

    // Write the ROM to a file
    FILE *out_file = fopen("rom.bin", "wb");
    if (out_file == NULL) {
        printf("Error opening file.\n");
        return 1;
    }

    fwrite(rom, sizeof(uint8_t), rom_size, out_file);
    fclose(out_file);

    return 0;
}
