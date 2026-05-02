#include <cstdint>
#include <godot_cpp/classes/global_constants.hpp>

using namespace godot;

typedef enum QKeyCode {
    Q_KEY_CODE_UNMAPPED,
    Q_KEY_CODE_SHIFT,
    Q_KEY_CODE_SHIFT_R,
    Q_KEY_CODE_ALT,
    Q_KEY_CODE_ALT_R,
    Q_KEY_CODE_CTRL,
    Q_KEY_CODE_CTRL_R,
    Q_KEY_CODE_MENU,
    Q_KEY_CODE_ESC,
    Q_KEY_CODE_1,
    Q_KEY_CODE_2,
    Q_KEY_CODE_3,
    Q_KEY_CODE_4,
    Q_KEY_CODE_5,
    Q_KEY_CODE_6,
    Q_KEY_CODE_7,
    Q_KEY_CODE_8,
    Q_KEY_CODE_9,
    Q_KEY_CODE_0,
    Q_KEY_CODE_MINUS,
    Q_KEY_CODE_EQUAL,
    Q_KEY_CODE_BACKSPACE,
    Q_KEY_CODE_TAB,
    Q_KEY_CODE_Q,
    Q_KEY_CODE_W,
    Q_KEY_CODE_E,
    Q_KEY_CODE_R,
    Q_KEY_CODE_T,
    Q_KEY_CODE_Y,
    Q_KEY_CODE_U,
    Q_KEY_CODE_I,
    Q_KEY_CODE_O,
    Q_KEY_CODE_P,
    Q_KEY_CODE_BRACKET_LEFT,
    Q_KEY_CODE_BRACKET_RIGHT,
    Q_KEY_CODE_RET,
    Q_KEY_CODE_A,
    Q_KEY_CODE_S,
    Q_KEY_CODE_D,
    Q_KEY_CODE_F,
    Q_KEY_CODE_G,
    Q_KEY_CODE_H,
    Q_KEY_CODE_J,
    Q_KEY_CODE_K,
    Q_KEY_CODE_L,
    Q_KEY_CODE_SEMICOLON,
    Q_KEY_CODE_APOSTROPHE,
    Q_KEY_CODE_GRAVE_ACCENT,
    Q_KEY_CODE_BACKSLASH,
    Q_KEY_CODE_Z,
    Q_KEY_CODE_X,
    Q_KEY_CODE_C,
    Q_KEY_CODE_V,
    Q_KEY_CODE_B,
    Q_KEY_CODE_N,
    Q_KEY_CODE_M,
    Q_KEY_CODE_COMMA,
    Q_KEY_CODE_DOT,
    Q_KEY_CODE_SLASH,
    Q_KEY_CODE_ASTERISK,
    Q_KEY_CODE_SPC,
    Q_KEY_CODE_CAPS_LOCK,
    Q_KEY_CODE_F1,
    Q_KEY_CODE_F2,
    Q_KEY_CODE_F3,
    Q_KEY_CODE_F4,
    Q_KEY_CODE_F5,
    Q_KEY_CODE_F6,
    Q_KEY_CODE_F7,
    Q_KEY_CODE_F8,
    Q_KEY_CODE_F9,
    Q_KEY_CODE_F10,
    Q_KEY_CODE_NUM_LOCK,
    Q_KEY_CODE_SCROLL_LOCK,
    Q_KEY_CODE_KP_DIVIDE,
    Q_KEY_CODE_KP_MULTIPLY,
    Q_KEY_CODE_KP_SUBTRACT,
    Q_KEY_CODE_KP_ADD,
    Q_KEY_CODE_KP_ENTER,
    Q_KEY_CODE_KP_DECIMAL,
    Q_KEY_CODE_SYSRQ,
    Q_KEY_CODE_KP_0,
    Q_KEY_CODE_KP_1,
    Q_KEY_CODE_KP_2,
    Q_KEY_CODE_KP_3,
    Q_KEY_CODE_KP_4,
    Q_KEY_CODE_KP_5,
    Q_KEY_CODE_KP_6,
    Q_KEY_CODE_KP_7,
    Q_KEY_CODE_KP_8,
    Q_KEY_CODE_KP_9,
    Q_KEY_CODE_LESS,
    Q_KEY_CODE_F11,
    Q_KEY_CODE_F12,
    Q_KEY_CODE_PRINT,
    Q_KEY_CODE_HOME,
    Q_KEY_CODE_PGUP,
    Q_KEY_CODE_PGDN,
    Q_KEY_CODE_END,
    Q_KEY_CODE_LEFT,
    Q_KEY_CODE_UP,
    Q_KEY_CODE_DOWN,
    Q_KEY_CODE_RIGHT,
    Q_KEY_CODE_INSERT,
    Q_KEY_CODE_DELETE,
    Q_KEY_CODE_STOP,
    Q_KEY_CODE_AGAIN,
    Q_KEY_CODE_PROPS,
    Q_KEY_CODE_UNDO,
    Q_KEY_CODE_FRONT,
    Q_KEY_CODE_COPY,
    Q_KEY_CODE_OPEN,
    Q_KEY_CODE_PASTE,
    Q_KEY_CODE_FIND,
    Q_KEY_CODE_CUT,
    Q_KEY_CODE_LF,
    Q_KEY_CODE_HELP,
    Q_KEY_CODE_META_L,
    Q_KEY_CODE_META_R,
    Q_KEY_CODE_COMPOSE,
    Q_KEY_CODE_PAUSE,
    Q_KEY_CODE_RO,
    Q_KEY_CODE_HIRAGANA,
    Q_KEY_CODE_HENKAN,
    Q_KEY_CODE_YEN,
    Q_KEY_CODE_MUHENKAN,
    Q_KEY_CODE_KATAKANAHIRAGANA,
    Q_KEY_CODE_KP_COMMA,
    Q_KEY_CODE_KP_EQUALS,
    Q_KEY_CODE_POWER,
    Q_KEY_CODE_SLEEP,
    Q_KEY_CODE_WAKE,
    Q_KEY_CODE_AUDIONEXT,
    Q_KEY_CODE_AUDIOPREV,
    Q_KEY_CODE_AUDIOSTOP,
    Q_KEY_CODE_AUDIOPLAY,
    Q_KEY_CODE_AUDIOMUTE,
    Q_KEY_CODE_VOLUMEUP,
    Q_KEY_CODE_VOLUMEDOWN,
    Q_KEY_CODE_MEDIASELECT,
    Q_KEY_CODE_MAIL,
    Q_KEY_CODE_CALCULATOR,
    Q_KEY_CODE_COMPUTER,
    Q_KEY_CODE_AC_HOME,
    Q_KEY_CODE_AC_BACK,
    Q_KEY_CODE_AC_FORWARD,
    Q_KEY_CODE_AC_REFRESH,
    Q_KEY_CODE_AC_BOOKMARKS,
    Q_KEY_CODE_LANG1,
    Q_KEY_CODE_LANG2,
    Q_KEY_CODE_F13,
    Q_KEY_CODE_F14,
    Q_KEY_CODE_F15,
    Q_KEY_CODE_F16,
    Q_KEY_CODE_F17,
    Q_KEY_CODE_F18,
    Q_KEY_CODE_F19,
    Q_KEY_CODE_F20,
    Q_KEY_CODE_F21,
    Q_KEY_CODE_F22,
    Q_KEY_CODE_F23,
    Q_KEY_CODE_F24,
    Q_KEY_CODE__MAX,
} QKeyCode;

int map_godot_key_to_qemu_qcode(int64_t godot_key) {
    // 过滤掉修饰键标志（Shift, Ctrl, Alt 等），只保留基础键码
    int64_t base_key = godot_key & KEY_CODE_MASK;

    switch (base_key) {
        // ---------------- 字母 ----------------
        case KEY_A: return Q_KEY_CODE_A;
        case KEY_B: return Q_KEY_CODE_B;
        case KEY_C: return Q_KEY_CODE_C;
        case KEY_D: return Q_KEY_CODE_D;
        case KEY_E: return Q_KEY_CODE_E;
        case KEY_F: return Q_KEY_CODE_F;
        case KEY_G: return Q_KEY_CODE_G;
        case KEY_H: return Q_KEY_CODE_H;
        case KEY_I: return Q_KEY_CODE_I;
        case KEY_J: return Q_KEY_CODE_J;
        case KEY_K: return Q_KEY_CODE_K;
        case KEY_L: return Q_KEY_CODE_L;
        case KEY_M: return Q_KEY_CODE_M;
        case KEY_N: return Q_KEY_CODE_N;
        case KEY_O: return Q_KEY_CODE_O;
        case KEY_P: return Q_KEY_CODE_P;
        case KEY_Q: return Q_KEY_CODE_Q;
        case KEY_R: return Q_KEY_CODE_R;
        case KEY_S: return Q_KEY_CODE_S;
        case KEY_T: return Q_KEY_CODE_T;
        case KEY_U: return Q_KEY_CODE_U;
        case KEY_V: return Q_KEY_CODE_V;
        case KEY_W: return Q_KEY_CODE_W;
        case KEY_X: return Q_KEY_CODE_X;
        case KEY_Y: return Q_KEY_CODE_Y;
        case KEY_Z: return Q_KEY_CODE_Z;

        // ---------------- 数字 ----------------
        case KEY_0: return Q_KEY_CODE_0;
        case KEY_1: return Q_KEY_CODE_1;
        case KEY_2: return Q_KEY_CODE_2;
        case KEY_3: return Q_KEY_CODE_3;
        case KEY_4: return Q_KEY_CODE_4;
        case KEY_5: return Q_KEY_CODE_5;
        case KEY_6: return Q_KEY_CODE_6;
        case KEY_7: return Q_KEY_CODE_7;
        case KEY_8: return Q_KEY_CODE_8;
        case KEY_9: return Q_KEY_CODE_9;

        // ---------------- 符号区 ----------------
        case KEY_MINUS:         return Q_KEY_CODE_MINUS;
        case KEY_EQUAL:         return Q_KEY_CODE_EQUAL;
        case KEY_BRACKETLEFT:   return Q_KEY_CODE_BRACKET_LEFT;
        case KEY_BRACKETRIGHT:  return Q_KEY_CODE_BRACKET_RIGHT;
        case KEY_BACKSLASH:     return Q_KEY_CODE_BACKSLASH;
        case KEY_SEMICOLON:     return Q_KEY_CODE_SEMICOLON;
        case KEY_APOSTROPHE:    return Q_KEY_CODE_APOSTROPHE;
        case KEY_QUOTELEFT:     return Q_KEY_CODE_GRAVE_ACCENT; // ` / ~
        case KEY_COMMA:         return Q_KEY_CODE_COMMA;
        case KEY_PERIOD:        return Q_KEY_CODE_DOT;
        case KEY_SLASH:         return Q_KEY_CODE_SLASH;
        case KEY_SPACE:         return Q_KEY_CODE_SPC;

        // ---------------- 控制键 ----------------
        case KEY_ESCAPE:        return Q_KEY_CODE_ESC;
        case KEY_TAB:           return Q_KEY_CODE_TAB;
        case KEY_BACKSPACE:     return Q_KEY_CODE_BACKSPACE;
        case KEY_ENTER:         return Q_KEY_CODE_RET;
        case KEY_INSERT:        return Q_KEY_CODE_INSERT;
        case KEY_DELETE:        return Q_KEY_CODE_DELETE;
        case KEY_PAUSE:         return Q_KEY_CODE_PAUSE;
        case KEY_PRINT:         return Q_KEY_CODE_PRINT;
        case KEY_SYSREQ:        return Q_KEY_CODE_SYSRQ;
        case KEY_HOME:          return Q_KEY_CODE_HOME;
        case KEY_END:           return Q_KEY_CODE_END;
        case KEY_PAGEUP:        return Q_KEY_CODE_PGUP;
        case KEY_PAGEDOWN:      return Q_KEY_CODE_PGDN;

        // ---------------- 方向键 ----------------
        case KEY_LEFT:          return Q_KEY_CODE_LEFT;
        case KEY_UP:            return Q_KEY_CODE_UP;
        case KEY_RIGHT:         return Q_KEY_CODE_RIGHT;
        case KEY_DOWN:          return Q_KEY_CODE_DOWN;

        // ---------------- 修饰键 ----------------
        case KEY_SHIFT:         return Q_KEY_CODE_SHIFT;
        case KEY_CTRL:          return Q_KEY_CODE_CTRL;
        case KEY_META:          return Q_KEY_CODE_META_L; // Windows键 / Command键
        case KEY_ALT:           return Q_KEY_CODE_ALT;
        case KEY_CAPSLOCK:      return Q_KEY_CODE_CAPS_LOCK;
        case KEY_NUMLOCK:       return Q_KEY_CODE_NUM_LOCK;
        case KEY_SCROLLLOCK:    return Q_KEY_CODE_SCROLL_LOCK;

        // ---------------- F1 - F12 ----------------
        case KEY_F1:  return Q_KEY_CODE_F1;
        case KEY_F2:  return Q_KEY_CODE_F2;
        case KEY_F3:  return Q_KEY_CODE_F3;
        case KEY_F4:  return Q_KEY_CODE_F4;
        case KEY_F5:  return Q_KEY_CODE_F5;
        case KEY_F6:  return Q_KEY_CODE_F6;
        case KEY_F7:  return Q_KEY_CODE_F7;
        case KEY_F8:  return Q_KEY_CODE_F8;
        case KEY_F9:  return Q_KEY_CODE_F9;
        case KEY_F10: return Q_KEY_CODE_F10;
        case KEY_F11: return Q_KEY_CODE_F11;
        case KEY_F12: return Q_KEY_CODE_F12;

        // ---------------- 小键盘 (Numpad) ----------------
        case KEY_KP_MULTIPLY:   return Q_KEY_CODE_KP_MULTIPLY;
        case KEY_KP_DIVIDE:     return Q_KEY_CODE_KP_DIVIDE;
        case KEY_KP_SUBTRACT:   return Q_KEY_CODE_KP_SUBTRACT;
        case KEY_KP_ADD:        return Q_KEY_CODE_KP_ADD;
        case KEY_KP_ENTER:      return Q_KEY_CODE_KP_ENTER;
        case KEY_KP_PERIOD:     return Q_KEY_CODE_KP_DECIMAL;
        case KEY_KP_0:          return Q_KEY_CODE_KP_0;
        case KEY_KP_1:          return Q_KEY_CODE_KP_1;
        case KEY_KP_2:          return Q_KEY_CODE_KP_2;
        case KEY_KP_3:          return Q_KEY_CODE_KP_3;
        case KEY_KP_4:          return Q_KEY_CODE_KP_4;
        case KEY_KP_5:          return Q_KEY_CODE_KP_5;
        case KEY_KP_6:          return Q_KEY_CODE_KP_6;
        case KEY_KP_7:          return Q_KEY_CODE_KP_7;
        case KEY_KP_8:          return Q_KEY_CODE_KP_8;
        case KEY_KP_9:          return Q_KEY_CODE_KP_9;

        // ---------------- 多媒体与其他功能键 ----------------
        case KEY_MENU:          return Q_KEY_CODE_MENU;
        case KEY_HELP:          return Q_KEY_CODE_HELP;
        case KEY_STOP:          return Q_KEY_CODE_STOP;
        case KEY_VOLUMEDOWN:    return Q_KEY_CODE_VOLUMEDOWN;
        case KEY_VOLUMEMUTE:    return Q_KEY_CODE_AUDIOMUTE;
        case KEY_VOLUMEUP:      return Q_KEY_CODE_VOLUMEUP;
        case KEY_MEDIAPLAY:     return Q_KEY_CODE_AUDIOPLAY;
        case KEY_MEDIASTOP:     return Q_KEY_CODE_AUDIOSTOP;
        case KEY_MEDIAPREVIOUS: return Q_KEY_CODE_AUDIOPREV;
        case KEY_MEDIANEXT:     return Q_KEY_CODE_AUDIONEXT;
        case KEY_YEN:           return Q_KEY_CODE_YEN;

        default:
            // 未知或未映射的按键
            return Q_KEY_CODE_UNMAPPED;
    }
}
