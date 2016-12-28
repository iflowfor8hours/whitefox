/*
Copyright 2015 Jun Wako <wakojun@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "keymap_common.h"

#include "led_controller.h"

const uint8_t keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /* Layer 0: Default Layer
     * ,---------------------------------------------------------------.
     * |Esc|  1|  2|  3|  4|  5|  6|  7|  8|  9|  0|  -|  =|  \|  `|FN0|
     * |---------------------------------------------------------------|
     * |Tab  |  Q|  W|  E|  R|  T|  Y|  U|  I|  O|  P|  [|  ]|Backs|Del|
     * |---------------------------------------------------------------|
     * | Ctrl |  A|  S|  D|  F|  G|  H|  J|  K|  L|  ;|  '|Enter   |PgU|
     * |---------------------------------------------------------------|
     * |Shif|   |  Z|  X|  C|  V|  B|  N|  M|  ,|  .|  /|Shift |Up |PgD|
     * |---------------------------------------------------------------|
     * |Ctrl|FN0 |Alt |         Space         |Alt |Ctrl|  |Lef|Dow|Rig|
     * `---------------------------------------------------------------'
     */
    [0] = KEYMAP( \
        FN3, 1,   2,   3,   4,   5,   6,   7,   8,   9,   0,   MINS,EQL, BSLS,GRV, FN1, \
        TAB, Q,   W,   E,   R,   T,   Y,   U,   I,   O,   P,   LBRC,RBRC,BSPC,     DEL, \
        LCTL,A,   S,   D,   F,   G,   H,   J,   K,   L,   SCLN,QUOT,NUHS,ENT,      PGUP,\
        LSFT,NUBS,Z,   X,   C,   V,   B,   N,   M,   COMM,DOT, SLSH,RSFT,     UP,  PGDN,\
        LCTL,FN0 ,LALT,               SPC,           RALT,RCTL,NO,       LEFT,DOWN,RGHT \
    ),
    /* Layer 1: HHKB-style FN-layer plus extras
     * ,---------------------------------------------------------------.
     * | ` | F1| F2| F3| F4| F5| F6| F7| F8| F9|F10|F11|F12|Ins|Del|   |
     * |---------------------------------------------------------------|
     * |Caps |   |Wak|Slp|   |   |   |   |PSc|SLk|Pau| Up|   |     |FN1|
     * |---------------------------------------------------------------|
     * |      |Vo-|Vo+|VoM|Ejc|   | * | / |Hom|PgU|Lef|Rgt| Enter  |Hom|
     * |---------------------------------------------------------------|
     * |    |   |FN5|FN6|   |   |   | + | - |End|PgD|Dow| Btld |   |End|
     * |---------------------------------------------------------------|
     * |    |    |    |                  |    |    |    |  |   |   |   |
     * `---------------------------------------------------------------'
     */
    [1] = KEYMAP( \
        TRNS, F1,  F2,  F3,  F4,  F5,  F6,  F7,  F8,  F9,  F10, F11, F12, INS, DEL,TRNS, \
        CAPS,TRNS,WAKE,SLEP,TRNS,TRNS,TRNS,TRNS,PSCR,SLCK,PAUS,UP,  TRNS,TRNS,     MUTE, \
        TRNS,VOLD,VOLU,MUTE,EJCT,TRNS,LEFT,DOWN,  UP,RGHT,TRNS,TRNS,TRNS,PENT,     VOLU, \
        TRNS,TRNS,FN5 ,FN6 ,TRNS,TRNS,TRNS,PPLS,PMNS,END, PGDN,DOWN,BTLD,     TRNS,VOLD, \
        TRNS,TRNS,TRNS,               TRNS,          TRNS,TRNS,TRNS,     TRNS,TRNS,TRNS  \
    ),
};

/* Give numbers some descriptive names */
#define ACTION_LEDS_LOGO 1
#define ACTION_LEDS_ENTER 2
#define SHIFT_ESC 3
#define SHIFT_BSPC 4

const uint16_t fn_actions[] = {
  [0] = ACTION_LAYER_MOMENTARY(1),
  // The following two will call action_function with parameter
  [1] = ACTION_FUNCTION(ACTION_LEDS_LOGO),
  [2] = ACTION_FUNCTION(ACTION_LEDS_ENTER),
  [3] = ACTION_FUNCTION(SHIFT_ESC),
  [4] = ACTION_FUNCTION(SHIFT_BSPC),
};

/* custom action function */
void action_function(keyrecord_t *record, uint8_t id, uint8_t opt) {
  (void)opt;
  if (record->event.pressed) dprint("P"); else dprint("R");
  if (record->tap.interrupted) dprint("i");

# define MODS_SHIFT_MASK   (MOD_BIT(KC_LSHIFT)|MOD_BIT(KC_RSHIFT))
  static uint8_t shift_mod;

  switch(id) {
        // Shift + Esc -> ~
        case SHIFT_ESC:
            shift_mod = get_mods()&MODS_SHIFT_MASK;
            if (record->event.pressed) {
                if (shift_mod) {
                    add_key(KC_GRV);
                    send_keyboard_report(); // send grave with shift for tilde result
                } else {
                    del_mods(shift_mod); // remove shift
                    add_key(KC_ESC);
                    send_keyboard_report(); // send escape
                    add_mods(shift_mod); // return shift but not sent
                }
            } else {
                if (shift_mod) {
                    del_key(KC_GRV);
                    send_keyboard_report();
                } else {
                    del_key(KC_ESC);
                    send_keyboard_report();
                }
            }
            break;
    case ACTION_LEDS_ENTER:
      // toggle enter LED on press
      if(record->event.pressed) {
        // signal the LED controller thread
        chMBPost(&led_mailbox, LED_MSG_ENTER_TOGGLE, TIME_IMMEDIATE);
      }
      break;
    case ACTION_LEDS_LOGO:
      // toggle TMK LOGO LEDs on press
      if(record->event.pressed) {
        // signal the LED controller thread
        chMBPost(&led_mailbox, LED_MSG_LOGO_TOGGLE, TIME_IMMEDIATE);
      }
      break;
        case SHIFT_BSPC:
            shift_mod = get_mods()&MODS_SHIFT_MASK;
            if (record->event.pressed) {
                if (shift_mod) {
                    add_key(KC_BSLS);
                    send_keyboard_report(); // send backslash with shift for pipe result
                } else {
                    del_mods(shift_mod); // remove shift
                    add_key(KC_BSPC);
                    send_keyboard_report(); // send backspace
                    add_mods(shift_mod); // return shift but not sent
                }
            } else {
                if (shift_mod) {
                    del_key(KC_BSLS);
                    send_keyboard_report();
                } else {
                    del_key(KC_BSPC);
                    send_keyboard_report();
                }
            }
            break;
  }
}
