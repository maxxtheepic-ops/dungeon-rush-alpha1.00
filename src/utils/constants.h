// src/utils/constants.h
#ifndef CONSTANTS_H
#define CONSTANTS_H

// ==============================================
// HARDWARE CONFIGURATION - CURRENT SETUP
// ==============================================

// TFT Display Pins (ST7789 SPI)
#define TFT_DC      11      // Data/Command
#define TFT_CS      12      // Chip Select
#define TFT_SCK     10      // SPI Clock
#define TFT_MOSI    13      // SPI Data Out
#define TFT_RST     9       // Reset
#define TFT_BL      14      // Backlight (INVERTED - LOW = ON!)

// Input Pins - Current Setup
#define BUTTON_UP   18      // Pin 18 = UP
#define BUTTON_DOWN 17      // Pin 17 = DOWN
#define BUTTON_A    21      // Pin 21 = A
#define BUTTON_B    38      // Pin 38 = B

// Future pins (when you expand)
#define BUTTON_LEFT     -1  // To be assigned
#define BUTTON_RIGHT    -1  // To be assigned
#define BUTTON_X        -1  // To be assigned
#define BUTTON_Y        -1  // To be assigned

// TF Card pins (when you add the reader)
#define SD_CS           -1  // To be assigned
#define SD_MOSI         -1  // To be assigned
#define SD_MISO         -1  // To be assigned
#define SD_SCK          -1  // To be assigned

// Audio pins (if you add a buzzer later)
#define BUZZER_PIN      -1  // To be assigned

// ==============================================
// DISPLAY CONFIGURATION
// ==============================================

#define SCREEN_WIDTH        170
#define SCREEN_HEIGHT       320
#define SCREEN_ROTATION     2   // Your current rotation

// Color definitions (16-bit RGB565)
#define COLOR_BLACK         0x0000
#define COLOR_WHITE         0xFFFF
#define COLOR_RED           0xF800
#define COLOR_GREEN         0x07E0
#define COLOR_BLUE          0x001F
#define COLOR_YELLOW        0xFFE0
#define COLOR_CYAN          0x07FF
#define COLOR_MAGENTA       0xF81F
#define COLOR_ORANGE        0xFD20
#define COLOR_PURPLE        0x8010
#define COLOR_GRAY          0x8410
#define COLOR_DARK_GRAY     0x4208
#define COLOR_LIGHT_GRAY    0xC618

// UI Colors
#define COLOR_BACKGROUND    COLOR_BLACK
#define COLOR_TEXT          COLOR_WHITE
#define COLOR_HIGHLIGHT     COLOR_YELLOW
#define COLOR_MENU_SELECT   COLOR_BLUE
#define COLOR_HEALTH_FULL   COLOR_GREEN
#define COLOR_HEALTH_LOW    COLOR_RED
#define COLOR_MANA          COLOR_BLUE
#define COLOR_XP            COLOR_PURPLE

// ==============================================
// INPUT CONFIGURATION
// ==============================================

#define INPUT_DEBOUNCE_MS       50
#define INPUT_HOLD_THRESHOLD_MS 500
#define INPUT_REPEAT_DELAY_MS   150

// ==============================================
// GAME BALANCE CONSTANTS
// ==============================================

// Player starting stats
#define PLAYER_START_HP     50
#define PLAYER_START_ATK    12
#define PLAYER_START_DEF    8
#define PLAYER_START_SPD    10

// Dungeon progression
#define ROOMS_PER_FLOOR     10      // Rooms before boss becomes available
#define FLOORS_PER_DUNGEON  5       // Total floors in dungeon (future expansion)

// Item constants
#define STARTING_POTIONS    3
#define STARTING_GOLD       100  // Increased so player can afford to rest
#define POTION_HEAL_AMOUNT  30
#define HEALTH_POTION_COST  25
#define MIN_DAMAGE          1

// Campfire costs
#define CAMPFIRE_REST_COST  20      // Gold cost to rest at campfire

// Enemy stats - Goblin (fast, weak)
#define GOBLIN_HP           25
#define GOBLIN_ATK          8
#define GOBLIN_DEF          3
#define GOBLIN_SPD          12

// Enemy stats - Skeleton (balanced)
#define SKELETON_HP         35
#define SKELETON_ATK        10
#define SKELETON_DEF        6
#define SKELETON_SPD        8

// Enemy stats - Orc (strong, slow)
#define ORC_HP              60
#define ORC_ATK             15
#define ORC_DEF             8
#define ORC_SPD             6

// Combat constants
#define MAX_COMBAT_TURNS    20
#define DEFEND_BONUS_MULTIPLIER 1.5

// ==============================================
// UI LAYOUT CONSTANTS
// ==============================================

// Combat layout - optimized for your 170x320 display
#define COMBAT_SPRITE_AREA_HEIGHT   200     // Top area for player/enemy sprites
#define COMBAT_MENU_AREA_HEIGHT     70      // Bottom area for menu
#define COMBAT_MENU_Y_START         250     // Where menu starts

// Menu layout
#define MENU_ITEM_HEIGHT    20
#define MENU_MARGIN_X       10
#define MENU_MARGIN_Y       10
#define MENU_TEXT_SIZE      1       // Smaller text as requested

// Health bars
#define HEALTH_BAR_WIDTH    60
#define HEALTH_BAR_HEIGHT   8
#define HEALTH_BAR_MARGIN   5

// ==============================================
// SYSTEM CONSTANTS
// ==============================================

#define SERIAL_BAUD_RATE    115200
#define MAIN_LOOP_DELAY_MS  10      // Your current delay

// Memory management
#define MAX_COMBAT_LOG_ENTRIES  10
#define MAX_INVENTORY_SLOTS     15

// Version info
#define GAME_VERSION        "0.1.0"
#define GAME_TITLE          "ESP32 Dungeon Crawler"

#endif