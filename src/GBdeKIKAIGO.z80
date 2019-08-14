; 2016/09/26
; GBdeKIKAIGO
; ver 1.0
; programed by HARUKI
INCLUDE "src/Fonts2.inc"
INCLUDE "src/HARDWARE.INC"
INCLUDE "src/header.inc"
; [ _*3 ] をCntl+F検索で 要所要所にジャンプ
;------------------------------
;  ___ 1. 変数宣言部 ;
;------------------------------
_ADDR		equ $C000	; 機械語プログラムを保存するメモリの先頭

_I0			equ $DB00	; 押している間
_I1			equ $DB01	; 押した瞬間
_I2			equ $DB02	; 離した瞬間
_I3			equ $DB03	; 直前フレームの押している間
_COUNT		equ $DB04	; フレームごとにインクリメント。カーソル点滅や、更新されるDB04を見て楽しむ用。

_LF			equ $DB10	; low flag. 編集しているケタが1桁目か。
_MODE		equ $DB11	; 十字キーを押しながらの操作なのか、Aなどを押しながらの操作なのか
_ENDB		equ $DB12	; Bボタンを離してモード2が終了した瞬間フラグ
_POS		equ $DB14	; 今のカーソル位置。16bit。
_POSSCR		equ $DB16	; スクロール値と合算したもの
_POSADR		equ $DB18	; さらにアドレス値(C000)と合算したもの

_SCRY		equ $DB20	; 16bit_1行で 32 ... 小数込みスクロール値
_SCRYcol	equ $DB22	; 16bit_1行で 8  ... 整数化スクロール値(SCRY >> 2)
_SPD		equ $DB24	; 16bit_スクロールスピード
_SPD16		equ $DB26	; 16bit_スクロールスピード / 16
_SCRY2		equ $DB2A	; 16bit_1行で 4	 ... ADDRと同じ
_SCRY3		equ $DB2C	; 16bit_1行で 1	 ... TILEと同じ

_OAMY		equ $DB30	; カーソルの座標位置
_OAMX		equ $DB31	; 同上
_BGcol		equ	$DB32	; 16bit_BGを更新する行
_MAPHofs	equ	$DB34	; 16bit_MAPHにaddするオフセット
_MEMofs		equ $DB36	; 16bit_MEMHにaddするオフセット (scry>>1 & FFFC)
_MAPH		equ	$DB38	; 16bit_書き込みを開始するMAPの先頭アドレス
_MEMH		equ $DB3A	; 16bit_読み込みを開始するMEMの先頭アドレス
_VBUFj		equ $DB3C	; VBUF書き込み時の行数(更新行数固定化のため未使用)

_VLINEF		equ $DB40	; 画面更新位置(0-3)
_SHITAF		equ $DB41	; スクロールが下の方フラグ
_VLINEofs	equ $DB42	; 16bit_加算時オフセット
_MAPHofs2	equ $DB44	; 16bit_MAPHにaddするオフセット
_MEMofs2	equ $DB46	; 16bit_(一応) MEMH2にaddするオフセット。MEMの値をBG高速切り替えに対応させるため。
_MEMofs3	equ $DB48	; 16bit_MEMH2にさらにaddするオフセット。MEMの値をスクロールに追従させるため。

_SPD2		equ $DB50	; もう一つのスピード
_SPD2D		equ $DB52	; 目的地
_SPD2F		equ $DB54	; フラグ

_SPBUF		equ $DBFF	; sp避難所
_VBUF		equ $DC00	; VBLANK時、一気に書き込むデータを溜めておくバッファ。$0400byte

;---------------------
; 定数 constant
FLOAT		equ 2		; 小数部に何ビット使うか
DRAW_N		equ 8		; VBLANK時、何行更新するか
TILEADR		equ $8000
MAPADR		equ $9800
WINADR		equ $9C00

;---------------------
SECTION "TITLE"		,HOME[$0134]
DB		"GBdeKIKAIGO"
SECTION "I_VBlank"	,HOME[$40]
	jp	 vblank
SECTION "I_Timer"  	,HOME[$50]
	jp	 timer

;------------- macro ----------------
waitvblank: macro
.wait\@
	ld	 a,[rLY]
	cp	 144
	jr	 nz, .wait\@
endm

disable_lcd: macro
	ld	 a, 0
	ldh	 [rLCDC], a
endm

; Bit 7 - LCD Display Enable			 (0=Off, 	1=On)
; Bit 6 - Window Tile Map Display Select (0=9800, 1=9C00)
; Bit 5 - Window Display Enable			 (0=Off, 	1=On)
; Bit 4 - BG & Window Tile Data Select	 (0=8800, 1=8000)
; Bit 3 - BG Tile Map Display Select	 (0=9800, 1=9C00)
; Bit 2 - OBJ (Sprite) Size				 (0=8x8, 	1=8x16)
; Bit 1 - OBJ (Sprite) Display Enable	 (0=Off, 	1=On)
; Bit 0 - BG Display (for CGB see below) (0=Off, 	1=On)
enable_lcd: macro
	ld	 a, LCDCF_ON | LCDCF_BG8000 | LCDCF_BGON | LCDCF_OBJON | $40 | $20
	ldh	 [rLCDC], a
endm

enable_irqv: macro
	ld	 a, IEF_VBLANK
	ldh	 [rIE], a
endm

;-------------------------------
; ___ 2. main
SECTION "main", HOME[$0150]
;-------------------------------
main:
	; 初期化
	di
	waitvblank
	disable_lcd

	call initmem		; 0クリア
	call initmem2	; 初期データ配置

	enable_lcd
	enable_irqv

.mainloop
	call input
	call modeswitcher
	call modeswitcher2

	call pre_logic
	call logicselector	; 160922_ロジック部ジャンプ処理を1元化
	call post_logic

	call YEAHHH		; 実行
	call vlogic
	ei
	halt
	di
	jp	 .mainloop

;-------------------
vblank:
	; 行書き込み
	; レジスタの準備
	; de = [_MAPH]
	ld	 a, [_MAPH]
	ld	 e, a
	ld	 a, [_MAPH + 1]
	ld	 d, a

	ld	 hl, _VBUF
	ld	 c, DRAW_N

	; ループ展開で処理時間稼ぐ
.loop
	; 0
	ld	 a, [hl+]
	ld	 [de], a
	inc	 de
	; 1
	ld	 a, [hl+]
	ld	 [de], a
	inc	 de
	; 2
	ld	 a, [hl+]
	ld	 [de], a
	inc	 de
	; 3
	ld	 a, [hl+]
	ld	 [de], a
	inc	 de
	; 4 空白
	inc	 hl
	inc	 de
	; 5  -----------
	ld	 a, [hl+]
	ld	 [de], a
	inc	 de
	; 6
	ld	 a, [hl+]
	ld	 [de], a
	inc	 de
	; 7
	ld	 a, [hl+]
	ld	 [de], a
	inc	 de
	; 8 空白
	inc	 hl
	inc	 de
	; 9
	ld	 a, [hl+]
	ld	 [de], a
	inc	 de
	; 10
	ld	 a, [hl+]
	ld	 [de], a
	inc	 de
	; 11 空白
	inc	 hl
	inc	 de
	; 12
	ld	 a, [hl+]
	ld	 [de], a
	inc	 de
	; 13
	ld	 a, [hl+]
	ld	 [de], a
	inc	 de
	; 14 空白
	inc	 hl
	inc	 de
	; 15
	ld	 a, [hl+]
	ld	 [de], a
	inc	 de
	; 16
	ld	 a, [hl+]
	ld	 [de], a
	inc	 de
	; de + BG右端までの距離(15) = $20 (改行処理)
	ld	 a, 15
	add	 l
	ld	 l, a
	xor	 a
	adc	 h
	ld	 h, a
	; hl + BG右端までの距離(15) = $20 (改行処理)
	ld	 a, 15
	add	 e
	ld	 e, a
	xor	 a
	adc	 d
	ld	 d, a
	; loop
	dec	 c
	jp	 nz, .loop

	;OAM
	ld	 de, _OAMRAM
	ld	 hl, _OAMY
	ld	 a, [hl+]
	ld	 [de], a
	inc	 de
	ld	 a, [hl+]
	ld	 [de], a

	; scroll (rSCY = _SCRYcol)
	ld	 a, [_SCRYcol]
	ld	 [rSCY], a
	reti

timer:
	reti

;-------------------------------------
; ___ 3. 初期化subroutine
;-------------------------------------
initmem:
	; こちらはクリア
	; VRAM
	ld	 de, $8000
	ld	 hl, 0
	ld	 bc, $2000
	call CopyDataImm

	; BGMAP
	ld	 de, $9800
	ld	 hl, $80
	ld	 bc, $0400
	call CopyDataImm

	; WINMAP
	ld	 de, $9C00
	ld	 hl, $82
	ld	 bc, $0400
	call CopyDataImm

	; RAM
	ld	 de, $C000
	ld	 hl, 0
	ld	 bc, $2000
	call CopyDataImm

	; VBUF
	ld	 de, $DC00
	ld	 hl, $80	; 空白タイルの位置をセット
	ld	 bc, $400
	call CopyDataImm

	; OAM
	ld	 de, $FE00
	ld	 hl, 0
	ld	 bc, $A0
	call CopyDataImm
	ret

initmem2:
	; こちらは配置
	ld	 hl, _ADDR
	ld	 a, $C9
	ld	 [$C000], a

	; loadtile
	ld	 de, TILEADR
	ld	 hl, Fonts
	ld	 bc, $1000	; ヘッダ部分読み込み(サイズ)
	call CopyData

	; loadoam:
	; OAM_0番 の タイルデータ(4中3番目)に [ | ]を入れる
	ld	 a, $C2						; [ | ]
	ld	 [_OAMRAM + 2], a

	; パレット GBC対策_160919追加
	ld	 a, $FC		; 0番以外黒
	ldh	 [$FF47], a
	ldh	 [$FF48], a
	ldh	 [$FF49], a

; カラーパレット設定 (BG) 190814
	; Bit 0-4   赤   (00 - 1F)
	; Bit 5-9   緑   (00 - 1F)
	; Bit 10-14 青   (00 - 1F)
	; XX 00000 00|000 00000 赤
	ld	 a, %10000000
	ld	 [$FF68], a
	
	ld	 hl, DATA_Color
	ld	 b, 8
.loop
	ld	 a, [hl+]
	ld	 [$FF69], a
	dec	 b
	jp	 nz, .loop

; カラーパレット設定 (OBJ) 190814
	ld	 a, %10000000
	ld	 [$FF6A], a
	
	ld	 hl, DATA_Color
	ld	 b, 8
.loop1
	ld	 a, [hl+]
	ld	 [$FF6B], a
	dec	 b
	jp	 nz, .loop1

	; WINの位置
	ld	 a, $80
	ldh	 [$FF4A], a
	ret

DATA_Color:
	dw %00111111111111111, %0000000000000000, %0000000000000000, %0000000000000000

input:
	; P1レジスタに30hを書き込む。
	; 20を書き込む。0123bit目から A, B, SL,ST, が拾える。
	; 10を書き込む。0123bit目から →, ←, ↑, ↓,  が拾える。
	; 押されていなければ1、押されていれば0。
	; → 01		A 10
	; ← 02		B 20
	; ↑ 04		L 40
	; ↓ 08 	T 80

	; _I3 = _I0
	ld	 a, [_I0]
	ld	 [_I3], a

	; Input_main
	call input_to_I0
	call input_to_I1_I2
	ret

input_to_I0:
	; 方向キー分
	ld	 a, $20
	ld	 [rP1], a
	ld	 a, [rP1]
	and	 $0f
	ld	 e, a

	; A B SEL SRT
	ld	 a, $10
	ld	 [rP1], a
	ld	 a, [rP1]
	and	 $0f
	swap a

	; 統合して_I0へ
	or	 e
	xor	 $FF
	ld	 [_I0], a
	ret

input_to_I1_I2:
	; d(バッファ) = _I0 ^ _I3
	ld	 a, [_I0]
	ld	 d, a
	ld	 a, [_I3]
	xor	 d
	ld	 d, a

	; _I1 = _I0 & d
	ld	 a, [_I0]
	and	 d
	ld	 [_I1], a

	; _I2 = _I3 & d
	ld	 a, [_I3]
	and	 d
	ld	 [_I2], a
	ret


;------------------------------
; ___ 4-1. モード処理
;------------------------------
modeswitcher:
; キー押した瞬間のモード切り替え
; 何もない	: _MODE = 0
; 十字キー	: _MODE = 1
; A			: _MODE = 2
; B			: _MODE = 4
; SEL		: _MODE = 8
	ld	 a, [_MODE]
	and	 a
	jr	 nz, .skip		; mode = 0 以外のときスキップ
	ld	 a, [_I1]
	and	 a
	jr	 z, .skip		; 入力がないときもスキップ
	ld	 e, a			; MODE&I1はよく使う数値なので,eに逃がしておく
						; a には I1 が入ってます
	and	 $0F			; 十字キー
	jr	 z, .else
.true
	ld	 a, 1
	jr	 .skip
.else
	ld	 a, e
	and	 $10			; A
	jr	 z, .else1
.true1
	ld	 a, 2
	jr	 .skip
.else1
	ld	 a, e
	and	 $20			; B
	jr	 z, .else2
.true2
	ld	 a, 4
	jr	 .skip
.else2
	ld	 a, e
	and	 $40			; SEL
	jr	 z, .else3
.true3
	ld	 a, 8
	jr	 .skip
.else3
.skip
	ld	 [_MODE], a
	ret

modeswitcher2:
; キー押されたままか、モード切り替えするかチェック
; 何もなくなった: _MODE = 0
; 十字キー		: _MODE 1 のまま
; A				: _MODE 2 のまま
; B				: _MODE 4 のまま
; SEL			: _MODE 8 のまま
	ld	 a, [_MODE]
	and	 a
	jr	 z, .skip		; こちらはモードが0のときスキップされます。
	; e = MODE
	ld	 a, [_MODE]
	ld	 e, a

	; if(MODE & 1)
.if
	and	 1
	jr	 z, .elif
.true
	ld	 a, [_I0]
	and	 $0F
	jr	 nz, .skip
	jr	 .endif
	; if(MODE & 2)
.elif
	ld	 a, e
	and	 2
	jr	 z, .elif1
.true1
	ld	 a, [_I0]
	and	 $10
	jr	 nz, .skip
	jr	 .endif
	; if(MODE & 4)
.elif1
	ld	 a, e
	and	 4
	jr	 z, .elif2
.true2
	ld	 a, [_I0]
	and	 $20
	jr	 nz, .skip
	jr	 .endif
	; if(MODE & 8)
.elif2
	ld	 a, e
	and	 8
	jr	 z, .elif3
.true3
	ld	 a, [_I0]
	and	 $40
	jr	 nz, .skip
	jr	 .endif
.elif3
.endif
; 継続入力がなかった時、mode = 0
	ld	 a, 0
	ld	 [_MODE], a
	ld	 a, [_I2]
	and	 $20
	call nz, ifENDB
.skip
; あればそのまま
	ret

ifENDB:
	ld	 a, 1
	ld	 [_ENDB], a
	ret


;------------------------------
; ___ 4-2. 計算・処理部1
;------------------------------
pre_logic:
	; _POSSCR = _POS + SCRY2
	ld	 de, _POSSCR
	ld	 hl, _POS
	ld	 bc, _SCRY2
	call Add16

	; _POSADR = _POSSCR + ADDR
	ld	 de, _POSADR
	ld	 hl, _POSSCR
	ld	 bc, _ADDR
	call Add16Imm

	ret

post_logic:
	; POS --------------------
	ld	 a, [_POS]
	and	 $3F
	ld	 [_POS], a
	ld	 a, [_POS + 1]
	xor	 a
	ld	 [_POS + 1], a
	; SPD -----------------------
	; _SPD16 = _SPD / 16
	ld	 de, _SPD16
	ld	 hl, _SPD
	ld	 bc, 4		; /16
	call Srl16_s

	;_SPD = _SPD - _SPD16
	ld	 de, _SPD
	ld	 hl, _SPD
	ld	 bc, _SPD16
	call Sub16

	; if (_SPD != 0)
	ld	 a, [_SPD]
	ld	 b, a
	ld	 a, [_SPD + 1]
	or	 b
	jr	 z, ._SPD_DEC_end
._SPD_DEC
	ld	 de, _SPD
	call Mem2HL
	ld	 a, h
	and	 $80		; _SPD < 0
	jr	 nz, ._SPD_DEC_end
.spd_is_plus
	; _SPD -= 1
	dec	 hl
	call HL2Mem
._SPD_DEC_end

	; B を 離したとき、SPD を 0 に。
	; if(ENDB)
	ld	 a, [_ENDB]
	and	 a
	jr	 z, ._s0
	; spd = 0
.true
	ld	 de, _SPD
	ld	 hl, 0
	ld	 bc, 2
	call CopyDataImm
	; 自然な方向にカクつかせるために必要
	; SCRY += 00000000 000100.00
	ld	 de, _SCRY
	ld	 hl, _SCRY
	ld	 bc, $0010
	call Add16Imm
	; SCRYl &= 0xE0
	ld	 a, [_SCRY]
	and	 $E0
	ld	 [_SCRY], a
	; ENDB = 0
	ld	 a, 0
	ld	 [_ENDB], a
._s0

	; SPD2 関連 ----------------------
	; if(!SPD2F)
	ld	 a, [_SPD2F]
	and	 a
	jr	 nz, ._sk_ifSPD2F
	; _SPD2D = _SCRY
	ld	 de, _SPD2D
	ld	 hl, _SCRY
	call CopyData2
._sk_ifSPD2F

	; _SPD2 = _SPD2D - _SCRY
	ld	 de, _SPD2
	ld	 hl, _SPD2D
	ld	 bc, _SCRY
	call Sub16

	; _SPD2 >>= 2
	ld	 de, _SPD2
	ld	 hl, _SPD2
	ld	 bc, 2
	call Srl16_s

	; if(_SPD2 == 0)
	ld	 a, [_SPD2 + 1]
	ld	 b, a
	ld	 a, [_SPD2]
	or	 b
	jr	 nz, ._sk_ifSPD2is0
	; SPD2F = 0
	ld	 a, 0
	ld	 [_SPD2F], a
	; SCRY = spd2D
	ld	 de, _SCRY
	ld	 hl, _SPD2D
	call CopyData2
._sk_ifSPD2is0


	; SCRY ------------------------------
	; _SCRY += _SPD + _SPD2
	ld	 de, _SCRY
	ld	 hl, _SCRY
	ld	 bc, _SPD
	call Add16
	ld	 bc, _SPD2
	call Add16

	; _SCRYcol = _SCRY >> FLOAT
	; 1行分のとき、8になる
	ld	 de, _SCRYcol
	ld	 hl, _SCRY
	ld	 bc, FLOAT
	call Srl16

	; SCRY2 = (SCRYcol >> 1)
	; 1行分のとき、4になる。
	ld	 de, _SCRY2
	ld	 hl, _SCRYcol
	ld	 bc, 1
	call Srl16
	ld	 a, [_SCRY2]
	and	 $FC
	ld	 [_SCRY2], a

	; SCRY3 = (SCRYcol >> 3)
	; 1行分のとき、1になる。
	ld	 de, _SCRY3
	ld	 hl, _SCRYcol
	ld	 bc, 3
	call Srl16

	; 固定行更新 ----------------------
	; VLINEF = VLINEF + 1 & 3
	ld	 a, [_VLINEF]
	inc	 a
	and	 3
	ld	 [_VLINEF], a

	; MAPHofs2 = VLINEF << 8
	ld	 de, _MAPHofs2
	ld	 hl, _VLINEF
	ld	 bc, 8
	call Sll16

	; MEMofs2 = VLINEf << 5
	ld	 de, _MEMofs2
	ld	 hl, _VLINEF
	ld	 bc, 5
	call Sll16

	; MEMofs3 = (_SCRY2) & $FF80
	ld	 de, _MEMofs3
	ld	 hl, _SCRY2
	ld	 a, [hl+]
	and	 $80
	ld	 [de], a
	inc	 de
	ld	 a, [hl]
	ld	 [de], a

	; MEMofs3 = MEMofs3 + MEMofs2
	ld	 de, _MEMofs3
	ld	 hl, _MEMofs3
	ld	 bc, _MEMofs2
	call Add16

	; SHITAF = SCRY3 & 0x10
	ld	 a, [_SCRY3]
	and	 $10
	ld	 [_SHITAF], a

	; if(SHITAF)
	ld	 a, [_SHITAF]
	and	 a
	jr	 z, ._s2_ifshitaf
	; if(VLINEF < 2)
	ld	 a, [_VLINEF]
	cp	 2
	jr	 nc, ._s2_ifshitaf
	; MEMofs3 = MEMofs3 - 0x0080
	ld	 de, _MEMofs3
	ld	 hl, _MEMofs3
	ld	 bc, $0080
	call Add16Imm
	; E000問題対策
	ld	 a, [_MEMofs3 + 1]
	and	 $DF
	ld	 [_MEMofs3 + 1], a
._s2_ifshitaf

	; --------------------------------
	; 書き込みを開始するMAP先頭アドレスを求める
	; MAPH = VLINEofs + MAP($9800)
	ld	 de, _MAPH
	ld	 hl, _MAPHofs2
	ld	 bc, MAPADR
	call Add16Imm

	; 読み込みを開始するMEM先頭アドレスを求める
	; MEMH = MEMofs3 + ADDR($C000)
	ld	 de, _MEMH
	ld	 hl, _MEMofs3
	ld	 bc, _ADDR
	call Add16Imm

	; COUNT++
	ld	 de, _COUNT
	ld	 hl, _COUNT
	ld	 bc, 1
	call Add16Imm

	ret

;------------------------------
; ___ 4-3. ボタン入力時処理
;------------------------------
logicselector:
; _MODEフラグを読み取って、行う処理を選ぶ。
	ld	 a, [_MODE]
	and	 1
	call nz, logic1

	ld	 a, [_MODE]
	and	 2
	call nz, logic2

	ld	 a, [_MODE]
	and	 4
	call nz, logic3

	ld	 a, [_MODE]
	and	 8
	call nz, logic4
	ret

logic1:
; 数値入力ロジック
; 十字キー押しながら のロジック
; A B どちらか押した瞬間でないと実行されません。
	ld	 a, [_I1]
	and	 $30
	jr	 z, .return

;十字キーの処理
	ld	 hl, _POSADR
	call Mem2HL_stack
	ld	 de, d_key
	ld	 a, [_I0]
	and	 $0F			; 十字キーのみゲット
	add	 a, e
	ld	 e, a
	jr	 nc, .skip2
	inc	 d
.skip2

; ABの処理
	ld	 a, [de]		; キーオフセットをbに
	ld	 b, a
	ld	 a, [_I1]		; if (_I1 & 0x20)
	and	 $20
	jr	 z, .skip3
	inc	 b				; キーオフセット + 1
.skip3

; もしLフラグたってたら上を、でなければ下を操作
	ld	 a, [_LF]		; if(_LF)
	and	 1
	jr	 z, .else
.true
	ld	 a, [hl]
	and	 $F0			; 下を消す
	jr	 .endif
.else
	ld	 a, [hl]
	and	 $0F			; 上を消す
	swap b
.endif
	or	 b
	ld	 [hl], a

	; 次のために操作ケタ反転
	ld	 a, [_LF]
	xor	 1
	ld	 [_LF], a

	; カーソルもずらす
	jr	 nz, .return
	ld	 a, [_POS]
	inc	 a
	ld	 [_POS], a
.return
	ret


logic2:
; Aボタン押しながら のロジック
; カーソル移動です
; 十字キー押した瞬間でないと実行されません。
	ld	 a, [_I1]
	and	 $F
	jr	 z, .end

; hl = _POS
	ld	 a, [_POS]
	ld	 l, a
	ld	 a, [_POS + 1]
	ld	 h, a

; if(_I1 & 左右)
	ld	 a, [_I1]
	ld	 b, a			; b に逃がす
	and	 $3
	jr	 z, .skip
.true
	and	 $1
	jr	 z, .skp
; 右おした時
	inc	 hl
	ld	 a, 0
	ld	 [_LF], a		; LFクリア
	jr	 .skip
; 左おした時
.skp
	ld	 a, [_LF]
	and	 a
	jr	 nz, .sk		; LFが1のときはdecしない
	dec	  hl
.sk
	ld	 a, 0
	ld	 [_LF], a

; if(I1 & 上下)
.skip
	ld	 a, b
	and	 $c
	jr	 z, .skip1
.true1
	and	 $8
	jr	 z, .skp1
; 下
	ld	 de, $0004
	add	 hl, de
	jr	 .skip1
; 上
.skp1
	ld	 de, $FFFC ; -4
	add	 hl, de

.skip1
	ld	 a, l
	ld	 [_POS], a
	ld	 a, h
	ld	 [_POS + 1], a

.end
	ret

logic3:
; Bボタン押しながら のロジック
; 右キー
	ld	 a, [_I1]
	and	 1
	jr	 z, .skip_r
.right
	; spd += 0x0100
	ld	 de, _SPD
	ld	 hl, _SPD
	ld	 bc, $0040
	call Add16Imm
.skip_r

; 左キー
	ld	 a, [_I1]
	and	 2
	jr	 z, .skip_l
.left
	ld	 de, _SPD
	ld	 hl, _SPD
	ld	 bc, $0040
	call Sub16Imm
.skip_l

	; 上か下
	ld	 a, [_I1]
	and	 $0C
	jr	 z, .skip_ud
.ud
	ld	 a, 1
	ld	 [_SPD2F], a

	; 下キー
	ld	 a, [_I1]
	and	 8
	jr	 z, .skip_d
.down
	; spd2D += 0x0100
	ld	 de, _SPD2D
	ld	 hl, _SPD2D
	ld	 bc, $8 << FLOAT
	call Add16Imm
.skip_d

	; 上キー
	ld	 a, [_I1]
	and	 4
	jr	 z, .skip_u
.up
	ld	 de, _SPD2D
	ld	 hl, _SPD2D
	ld	 bc, $8 << FLOAT
	call Sub16Imm
.skip_u
.skip_ud

	ret


logic4:
; SELおしながら
	ret

YEAHHH:
	ld	 a, [_I1]		; if(_I1 & 0x40)
	and	 $80			; スタートボタン
	jr	 z, .skip

	waitvblank
	ld	 a, 0
	ld	 bc, 0
	ld	 de, 0
	ld	 hl, 0
	call _ADDR			; $C000 にジャンプ！！！

.skip
	ret


;------------------------------
; ___ 5. 描画関連の処理
;------------------------------
cput: macro
; (a - source, hl - dest) 引数にタイル番号
	ld	 a, \1
	ld	 [hl+], a
endm

vlogic:
	call makevbuf
	call movecursol
	ret

makevbuf:
; メモリの内容を元にVBUFに値を書き込んでいく。
; 後にVBUFはVBLANK割り込みルーチン、または全画面書き換えルーチンによって実際にVRAMに書き込まれる
	; 初期化
	ld	 hl, _VBUF	; 書き
	ld	 de, _MEMH	; 読み
	call Mem2DE_stack
	ld	 b, DRAW_N	; 繰り返し2 (更新する行数)

	; C000 |
.loop2
	call printADDR
	inc	 hl		; スペース
	cput $C2	; |	 ←	 これ

	; 00 00 00 00
	ld	 c, 4
.loop
	call printbyte
	inc	 hl			; スペース
	dec	 c
	jr	 nz, .loop

	;スペース14個
rept 14
	inc	 hl
endr

	;loop2 end
	dec	 b
	jr	 nz, .loop2
	ret

printADDR:
; C000 等、アドレスを描画する部分
	ld	 a, d
	call printa
	ld	 a, e
	call printa
	ret

printbyte:
; (de - source, hl - dest)
	ld	 a, [de]
	call printa
	inc	 de
	ret

printa:
; (a - source, hl - dest)
; hl は1つインクリメントされる
	; 上4ビット分
	push af
	swap a
	and	 $0F
;	add	 $80			;タイルデータ「0」位置が$80の場合
	ld	 [hl+], a
	; 下4ビット
	pop	 af
	and	 $0F
;	add	 $80
	ld	 [hl+], a
	ret


movecursol:
	; x
	; 空白分
	ld	 a, [_POS]
	and	 $03
	ld	 b, a
	; バイト分
	rla	 		; a * 2
	add	 a, b	; (a * 2) + a
	ld	 b, a	; ...逃がす
	; 編集場所半バイト分
	ld	 a, [_LF]
	add	 a, b	; + 1
	; 「C000 | 」 の部分
	add	 7
	; 1文字ぶんするため *8
	rla
	rla
	rla
	; 完了
	ld	 [_OAMX], a

	; y
	ld	 a, [_POS]
	rla
	and	 $F8
	; 左下+8がオフセット?
	add	 16
	ld	 [_OAMY], a
	ret


;-------------------------
; ___ 6. 便利ルーチン
;------------------------
CopyData:
; (de - source, hl - dest, bc - size)
	ld	 a, [hl+]
	ld	 [de], a
	inc	 de
	dec	 bc
	ld	 a, c
	or	 b
	jr	 nz, CopyData
	ret

CopyData2:
; (de - source, hl - dest)	2byteバージョン
	ld	 a, [hl+]
	ld	 [de], a
	inc	 de
	ld	 a, [hl-]
	ld	 [de], a
	dec	 de
	ret

CopyDataImm:
; ( l - source, de - dest, bc - size)
.loop
	ld	 a, l
	ld	 [de], a
	inc	 de
	dec	 bc
	ld	 a, c
	or	 b
	jr	 nz, .loop
	ret

Srl16:
; (hl - source, de - dest, c - 回数)
	ld	 a, [hl+]
	ld	 [de], a
	inc	 de
	ld	 a, [hl-]
	ld	 [de], a
	dec	 de
.loop
	inc	 de
	ld	 a, [de]
	srl	 a
	ld	 [de], a
	dec	 de
	ld	 a, [de]
	rra
	ld	 [de], a
	dec	 c
	jr	 nz, .loop
	ret

Srl16_s:
; 符号付き
; (hl - source, de - dest, c - 回数)
; まずsourceからdestにコピー
	ld	 a, [hl+]
	ld	 [de], a
	inc	 de
	ld	 a, [hl-]
	ld	 [de], a
	dec	 de
.loop
; 上ビット
	inc	 de
	ld	 a, [de]
	sra	 a
	ld	 [de], a
; 下ビット
	dec	 de
	ld	 a, [de]
	rra
	ld	 [de], a
	dec	 c
	jr	 nz, .loop
	ret

Sll16:
; (hl - source, de - dest, c - 回数)
	ld	 a, [hl+]
	ld	 [de], a
	inc	 de
	ld	 a, [hl-]
	ld	 [de], a
	dec	 de
.loop
	ld	 a, [de]
	sla	 a
	ld	 [de], a
	inc	 de

	ld	 a, [de]
	rla
	ld	 [de], a
	dec	 de

	dec	 c
	jr	 nz, .loop
	ret

Add16:
; (hl - source, de - dest, bc - 足す数のアドレス)
	;1バイト目
	ld	 a, [hl]
	push hl
	push bc
	pop	 hl
	add	 a, [hl]	 ; hl は bc
	ld	 [de], a
	pop	 hl

	inc	 bc
	inc	 de
	inc	 hl

	;2バイト目
	ld	 a, [hl]
	push hl
	push bc
	pop	 hl
	adc	 a, [hl]	 ; hl は bc
	ld	 [de], a
	pop	 hl

	dec	 bc
	dec	 de
	dec	 hl
	ret

Add16Imm:
; (hl - source, de - dest, bc - 足す数そのもの)
	ld	 a, [hl]
	add	 a, c	; hl は bc
	ld	 [de], a

	inc	 de
	inc	 hl

	;2バイト目
	ld	 a, [hl]
	adc	 a, b
	ld	 [de], a

	dec	 de
	dec	 hl
	ret

Sub16:
; (hl - source, de - dest, bc - 引く数のアドレス)
	;1バイト目
	ld	 a, [hl]
	push hl
	push bc
	pop	 hl
	sub	 a, [hl]	 ; hl は bc
	ld	 [de], a
	pop	 hl

	inc	 bc
	inc	 de
	inc	 hl

	;2バイト目
	ld	 a, [hl]
	push hl
	push bc
	pop	 hl
	sbc	 a,[hl]	; hl は bc
	ld	 [de], a
	pop	 hl

	dec	 bc
	dec	 de
	dec	 hl
	ret

Sub16Imm:
; (hl - source, de - dest, bc - 引く数そのもの)
	ld	 a, [hl]
	sub	 a, c	; hl は bc
	ld	 [de], a

	inc	 de
	inc	 hl

	;2バイト目
	ld	 a, [hl]
	sbc	 a, b
	ld	 [de], a

	dec	 bc
	dec	 de
	ret


Mem2HL:
; (hl <- [de])
	ld	 a, [de]
	ld	 l, a
	inc	 de
	ld	 a, [de]
	ld	 h, a
	dec	 de
	ret

HL2Mem:
; (de - dest, source - hl)
	ld	 a, l
	ld	 [de], a
	inc	 de
	ld	 a, h
	ld	 [de], a
	dec	 de
	ret

Mem2HL_stack:
; (hl - dest & source)
	inc	 hl
	ld	 a, [hl-]
	push af
	ld	 a, [hl]
	pop	 hl
	ld	 l, a
	ret

Mem2BC_stack:
; (BC - dest & source)
	inc	 bc
	ld	 a, [bc]
	dec	 bc
	push af
	ld	 a, [bc]
	pop	 bc
	ld	 c, a
	ret

Mem2DE_stack:
; (DE - dest & source)
	inc	 de
	ld	 a, [de]
	dec	 de
	push af
	ld	 a, [de]
	pop	 de
	ld	 e, a
	ret


;----------- DATA -----------------
d_key:
; 方向を目的の数値に変換するテーブル
; 上 = 0, 右上 = 2, 右 = 4, 右下 = 6
; 下 = 8, 左下 = A, 左 = C, 左上 = E
; 本来ありえない = FF
db $FF, $04, $0c, $FF	; ‐ → ← ‐
db $00, $02, $0E, $FF	; ↑ ↗ ↖ ‐
db $08, $06, $0A, $FF	; ↓ ↘ ↙ ‐
db $FF, $FF, $FF, $FF	; ‐ ‐ ‐ ‐
