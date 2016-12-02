SECTION "header", HOME[$0000]
ret         ; $0000-$003F: RST handlers.
DS 7
ret         ; $0008
DS 7
ret         ; $0010
DS 7
ret         ; $0018
DS 7
ret         ; $0020
DS 7
ret         ; $0028
DS 7
ret         ; $0030
DS 7
ret         ; $0038
DS 7

;jp vblank     ; $0040-$0067: Interrupt handlers.
;jp stat     ; $0048
;jp timer    ; $0050
;jp serial   ; $0058
;jp joypad   ; $0060
;$0068-$00FF: Free space.
SECTION "header2", HOME[$0100]
nop         ; $0100-$0103: Startup handler.
jp main
DB $CE, $ED, $66, $66, $CC, $0D, $00, $0B   ; $0104-$0133: The Nintendo Logo.
DB $03, $73, $00, $83, $00, $0C, $00, $0D
DB $00, $08, $11, $1F, $88, $89, $00, $0E
DB $DC, $CC, $6E, $E6, $DD, $DD, $D9, $99
DB $BB, $BB, $67, $63, $6E, $0E, $EC, $CC
DB $DD, $DC, $99, $9F, $BB, $B9, $33, $3E
;DS 11      ; $0134-$013E: The title(11 letters).
SECTION "manufacturer", HOME[$013F]
DS 4        ; $013F-$0142: The manufacturer code.
DB $00      ; $0143: Gameboy Color compatibility flag.
DB "00"     ; $0144-$0145: "New" Licensee Code, a two character name.
DB $00      ; $0146: Super Gameboy compatibility flag.
DB $00      ; $0147: Cartridge type. Either no ROM or MBC5 is recommended.
DB $00      ; $0148: Rom size.
DB $00      ; $0149: Ram size.
DB $00      ; $014A: Destination code.
DB $33      ; $014B: Old licensee code.
DB $00      ; $014C: ROM version number
DB $FF      ; $014D: Header checksum. Assembler needs to patch this.
DW $FACE    ; $014E- $014F: Global checksum. Assembler needs to patch this.
