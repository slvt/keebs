#pragma once

#define VIAL_KEYBOARD_UID {0x5C, 0x1E, 0x7A, 0x94, 0xB3, 0xD2, 0xF6, 0x08}
#define VIAL_UNLOCK_COMBO_ROWS {0, 0}
#define VIAL_UNLOCK_COMBO_COLS {0, 2}

/* Left handed mode is only switched by the Vial checkbox, not by left + right,
 * so the checkbox and the stored option never disagree. */
#define TB_NO_CHORD_LEFT_HANDED
