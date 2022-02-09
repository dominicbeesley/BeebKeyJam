#include "keymap.h"

unsigned int map1[] = {
    /* 00..0F */
    0,
    C1+R3+CTRL, // Ctrl A
    C4+R1+CTRL, // Ctrl B
    C2+R2+CTRL, // Ctrl C
    C2+R4+CTRL, // Ctrl D
    C2+R5+CTRL, // Ctrl E
    C3+R3+CTRL, // Ctrl F
    C3+R2+CTRL, // Ctrl G
    C4+R2+CTRL, // Ctrl H
    C0+R1,      // Ctrl I ( TAB)
    C5+R3+CTRL, // Ctrl J
    C6+R3+CTRL, // Ctrl K
    C6+R2+CTRL, // Ctrl L
    C9+R3     , // Ctrl M ( Return)
    C5+R2+CTRL, // Ctrl N
    C6+R4+CTRL, // Ctrl O

    /* 10..1F */
    C7+R4+CTRL, // Ctrl P
    C0+R6+CTRL, // Ctrl Q
    C3+R4+CTRL, // Ctrl R
    C1+R2+CTRL, // Ctrl S
    C3+R5+CTRL, // Ctrl T
    C5+R4+CTRL, // Ctrl U
    C3+R1+CTRL, // Ctrl V
    C1+R5+CTRL, // Ctrl W
    C2+R3+CTRL, // Ctrl X 
    C4+R3+CTRL, // Ctrl Y
    C1+R1+CTRL, // Ctrl Z
    C0+R0     , // Escape
    0, //0x1c
    0, //0x1d
    0, //0x1e
    0, //0x1f

    /* 20..2F */
    C2+R1,       // Space
    C0+R4+SHIFT, // !
    C1+R4+SHIFT, // "
    C1+R6+SHIFT, // # << remapped
    C2+R6+SHIFT, // $
    C3+R6+SHIFT, // %
    C4+R4+SHIFT, // & << remapped
    C4+R5+SHIFT, // ' << remapped
    C5+R6+SHIFT, // ( << remapped 
    C6+R5+SHIFT, // ) << remapped
    C8+R3+SHIFT, // * << remapped
    C7+R2+SHIFT, // + << remapped
    C6+R1,       // ,
    C7+R6,       // -
    C7+R1,       // .
    C8+R1,       // /

    /* 30..3F */
    C7+R5,       // 0
    C0+R4,       // 1
    C1+R4,       // 2
    C1+R6,       // 3
    C2+R6,       // 4
    C3+R6,       // 5
    C4+R4,       // 6
    C4+R5,       // 7
    C5+R6,       // 8
    C6+R5,       // 9
    C8+R3,       // :
    C7+R2,       // ;
    C6+R1+SHIFT, // <
    C7+R6+SHIFT, // = << remapped
    C7+R1+SHIFT, // >
    C8+R1+SHIFT, // ?

    /* 40..4F */
    C7+R3,       // @ << remapped
    C1+R3+SHIFT, // SHIFT A
    C4+R1+SHIFT, // SHIFT B
    C2+R2+SHIFT, // SHIFT C
    C2+R4+SHIFT, // SHIFT D
    C2+R5+SHIFT, // SHIFT E
    C3+R3+SHIFT, // SHIFT F
    C3+R2+SHIFT, // SHIFT G
    C4+R2+SHIFT, // SHIFT H
    C5+R5+SHIFT, // SHIFT I
    C5+R3+SHIFT, // SHIFT J
    C6+R3+SHIFT, // SHIFT K
    C6+R2+SHIFT, // SHIFT L
    C5+R1+SHIFT, // SHIFT M
    C5+R2+SHIFT, // SHIFT N
    C6+R4+SHIFT, // SHIFT O

    /* 50..5F */
    C7+R4+SHIFT, // SHIFT P
    C0+R6+SHIFT, // SHIFT Q
    C3+R4+SHIFT, // SHIFT R
    C1+R2+SHIFT, // SHIFT S
    C3+R5+SHIFT, // SHIFT T
    C5+R4+SHIFT, // SHIFT U
    C3+R1+SHIFT, // SHIFT V
    C1+R5+SHIFT, // SHIFT W
    C2+R3+SHIFT, // SHIFT X 
    C4+R3+SHIFT, // SHIFT Y
    C1+R1+SHIFT, // SHIFT Z
    C8+R4,       // [
    C8+R0,       // back slash NB this in C is line continuation
    C8+R2,       // ]
    C8+R6,       // ^ remapped
    C8+R5,       // _

    /* 60..6F */
    C8+R5+SHIFT, // ` used for £
    C1+R3, //  A
    C4+R1, //  B
    C2+R2, //  C
    C2+R4, //  D
    C2+R5, //  E
    C3+R3, //  F
    C3+R2, //  G
    C4+R2, //  H
    C5+R5, //  I
    C5+R3, //  J
    C6+R3, //  K
    C6+R2, //  L
    C5+R1, //  M
    C5+R2, //  N
    C6+R4, //  O

    /* 70..7F */
    C7+R4, //  P
    C0+R6, //  Q
    C3+R4, //  R
    C1+R2, //  S
    C3+R5, //  T
    C5+R4, //  U
    C3+R1, //  V
    C1+R5, //  W
    C2+R3, //  X 
    C4+R3, //  Y
    C1+R1, //  Z    
    C8+R4+SHIFT, // {
    C8+R0+SHIFT, // |
    C8+R2+SHIFT, // }
    C8+R6+SHIFT, // ~
    C9+R2,       // Delete

    /* 80..8F */
    C4+R0+SHIFT+CTRL, // 1b 5b 40 SHIFT+CTRL F5
    C9+R4+CTRL,       // 1b 5b 41 up
    C9+R5+CTRL,       // 1b 5b 42 down
    C9+R0+CTRL,       // 1b 5b 43 right
    C9+R6+CTRL,       // 1b 5b 44 left
    0,
    C9+R1,            // 1b 5b 46 end ( COPY)
    0,                // 1b 5b 47 page down
    CTRL,             // 1b 5b 48 home
    0,                // 1b 5b 49 page up
    0,
    0,
    SHIFT,            // 1b 5b 4c insert
    C1+R0,            // 1b 5b 4d F1
    C2+R0,            // 1b 5b 4e F2
    C3+R0,            // F3

    /* 90..9F */
    C4+R5,            // F4
    C4+R0,            // F5
    C5+R0,            // F6
    C6+R5,            // F7
    C6+R0,            // F8
    C7+R0,            // F9
    C0+R5,            // 1b 5b 56 F10 (F0)
    MAGIC,            // 1b 5b 57 F11
    BREAK,            // 1b 5b 58 F12 ( break)                                         
    C1+R0+SHIFT,      // 1b 5b 59 F1
    C2+R0+SHIFT,      // 1b 5b 5a F2
    C5+R0+SHIFT+CTRL, // 1b 5b 5b SHIFT+CTRL F6
    C6+R5+SHIFT+CTRL, // 1b 5b 5c SHIFT+CTRL F7
    C6+R0+SHIFT+CTRL, // 1b 5b 5d SHIFT+CTRL F8
    C7+R0+SHIFT+CTRL, // 1b 5b 5e SHIFT+CTRL F9
    C0+R5+SHIFT+CTRL, // 1b 5b 5f SHIFT+CTRL F10


    0,                // 1b 5b 60 SHIFT+CTRL F11

    C3+R0+SHIFT,      // 1b 5b 61 shift F3
    C4+R5+SHIFT,      // 1b 5b 62 shift F4
    C4+R0+SHIFT,      // 63 Shift F5
    C5+R0+SHIFT,      // 64 F6
    C6+R5+SHIFT,      // 65 F7
    C6+R0+SHIFT,      // 66 F8
    C7+R0+SHIFT,      // 67 F9
    C0+R5+SHIFT,      // 1b 5b 68 F10 (F0)
    MAGIC+SHIFT,      // 1b 5b 69 F11
    BREAK+SHIFT,      // 1b 5b 6A F12 ( break)                                 
                        
    C1+R0+CTRL,       // 6b F1
    C2+R0+CTRL,       // 6c F2
    C3+R0+CTRL,       // 6d F3
    C4+R5+CTRL,       // 6e F4
    C4+R0+CTRL,       // 6f F5
    C5+R0+CTRL,       // 70 F6
    C6+R5+CTRL,       // 71 F7
    C6+R0+CTRL,       // 72 F8
    C7+R0+CTRL,       // 73 F9
    C0+R5+CTRL,       // 74 F0
    MAGIC+CTRL,       // 1b 5b 75 F11
    BREAK+CTRL,       // 1b 5b 76 F12 ( break)   

    C1+R0+SHIFT+CTRL, // 77 F1
    C2+R0+SHIFT+CTRL, // 78 F2
    C3+R0+SHIFT+CTRL, // 79 F3
    C4+R5+SHIFT+CTRL, // 7a F4
    0                 // SHIFT +CTRL F12
};
