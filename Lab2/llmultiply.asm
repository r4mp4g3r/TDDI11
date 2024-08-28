;
;  Test cases. All numbers are in hex.
;
;      111122223333 *     555566667777 =          5B061D958BF0ECA7C0481B5
;  3456FEDCAAAA1000 * EDBA00112233FF01 = 309A912AF7188C57E62072DD409A1000
;  FFFFEEEEDDDDCCCC * BBBBAAAA99998888 = BBBB9E2692C5DDDCC28F7531048D2C60
;  FFFFFFFFFFFFFFFF * FFFFFFFFFFFFFFFF = FFFFFFFFFFFFFFFE0000000000000001
;  00000001FFFFFFFF * 00000001FFFFFFFF = 0000000000000003FFFFFFFC00000001
;  FFFEFFFFFFFFFFFF * FFFF0001FFFFFFFF = FFFE0002FFFDFFFE0001FFFE00000001
;

;  void llmultiply(unsigned long long int l1,
;                  unsigned long long int l2,
;                  unsigned char *result);
        
;  a = AH * (2 << 32) + AL
;  b = BH * (2 << 32) + BL
;        
;  a * b = (AH * BH            * (2 << 64)) +
;          (AH * BL + AL * BH) * (2 << 32)  +
;           AL * BL

; For more infromation, see the lab manual!
        
	SECTION .data



	SECTION .text
	ALIGN	16
	BITS	32

	;; To access parameter BH on the stack you do
	;;   MOV EAX, [EBP + 20]
	;; Replace the zero below with the correct value to be able
	;; to write your assembly code a little bit more readable: 
	;;   MOV EAX, [EBP + BH_OFF]
AL_OFF	EQU     8	; Offset from EBP to low  bits of a (AL)
AH_OFF	EQU     12	; Offset from EBP to high bits of a (AH)
BL_OFF	EQU     16	; Offset from EBP to low  bits of b (BL)
BH_OFF	EQU     20	; Offset from EBP to high bits of b (BH)
RES_OFF	EQU     24	; Offset from EBP to result array pointer
	;;    ^^^^^ Replace 0 with correct values above
        
	GLOBAL llmultiply

llmultiply:
	PUSH EBP
	MOV EBP, ESP
	MOV EBX, [EBP+RES_OFF]
	MOV EDX, 0
	MOV EAX, 0
	MOV [EBX+8], dword 0
	MOV [EBX+12], dword 0

	; AL * BL
	MOV EAX, [EBP+AL_OFF]
	MOV EDX, [EBP+BL_OFF]
	MUL EDX
	; AL * BL L
	MOV [EBX], EAX
	; AL * BL H
	MOV [EBX+4], EDX
	
	MOV EAX, [EBP+AH_OFF]
	MOV EDX, [EBP+BL_OFF]
	MUL EDX
	; BH * AL L
	ADD [EBX+4], EAX
	; BH * AL H
	ADC [EBX+8], EDX
	
	MOV EAX, [EBP+AL_OFF]
	MOV EDX, [EBP+BH_OFF]
	MUL EDX
	; AH * BL L
	ADD [EBX+4], EAX
	; AH * BL H
	ADC [EBX+8], EDX
	; CARRY
	ADC [EBX+12], dword 0
	
	MOV EAX, [EBP+AH_OFF]
	MOV EDX, [EBP+BH_OFF]
	MUL EDX
	; AH * BH L
	ADD [EBX+8], EAX
	; AH * BH H
	ADC [EBX+12], EDX
	

	POP EBP				; restore EBP reg
	RET				;  return

