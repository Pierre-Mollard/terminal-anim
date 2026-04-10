#ifndef TERMINAL_ANIM_BOX_D_H
#define TERMINAL_ANIM_BOX_D_H

// Unicode Box Drawing block: U+2500..U+257F
// Source reference: https://www.unicode.org/charts/nameslist/n_2500.html
// Generated with ChatGPT 5.2

// Generic macro
#define EDGE_KEY(t, b, l, r)                                                   \
  (((uint32_t)(t) << 0) | ((uint32_t)(b) << 4) | ((uint32_t)(l) << 8) |        \
   ((uint32_t)(r) << 12))

// Light and heavy solid lines
#define TAU_BD_LIGHT_HORIZONTAL 0x2500u // ─
#define TAU_BD_HEAVY_HORIZONTAL 0x2501u // ━
#define TAU_BD_LIGHT_VERTICAL 0x2502u   // │
#define TAU_BD_HEAVY_VERTICAL 0x2503u   // ┃

// Light and heavy dashed lines
#define TAU_BD_LIGHT_TRIPLE_DASH_HORIZONTAL 0x2504u    // ┄
#define TAU_BD_HEAVY_TRIPLE_DASH_HORIZONTAL 0x2505u    // ┅
#define TAU_BD_LIGHT_TRIPLE_DASH_VERTICAL 0x2506u      // ┆
#define TAU_BD_HEAVY_TRIPLE_DASH_VERTICAL 0x2507u      // ┇
#define TAU_BD_LIGHT_QUADRUPLE_DASH_HORIZONTAL 0x2508u // ┈
#define TAU_BD_HEAVY_QUADRUPLE_DASH_HORIZONTAL 0x2509u // ┉
#define TAU_BD_LIGHT_QUADRUPLE_DASH_VERTICAL 0x250Au   // ┊
#define TAU_BD_HEAVY_QUADRUPLE_DASH_VERTICAL 0x250Bu   // ┋

// Light and heavy line box components
#define TAU_BD_LIGHT_DOWN_AND_RIGHT 0x250Cu                // ┌
#define TAU_BD_DOWN_LIGHT_AND_RIGHT_HEAVY 0x250Du          // ┍
#define TAU_BD_DOWN_HEAVY_AND_RIGHT_LIGHT 0x250Eu          // ┎
#define TAU_BD_HEAVY_DOWN_AND_RIGHT 0x250Fu                // ┏
#define TAU_BD_LIGHT_DOWN_AND_LEFT 0x2510u                 // ┐
#define TAU_BD_DOWN_LIGHT_AND_LEFT_HEAVY 0x2511u           // ┑
#define TAU_BD_DOWN_HEAVY_AND_LEFT_LIGHT 0x2512u           // ┒
#define TAU_BD_HEAVY_DOWN_AND_LEFT 0x2513u                 // ┓
#define TAU_BD_LIGHT_UP_AND_RIGHT 0x2514u                  // └
#define TAU_BD_UP_LIGHT_AND_RIGHT_HEAVY 0x2515u            // ┕
#define TAU_BD_UP_HEAVY_AND_RIGHT_LIGHT 0x2516u            // ┖
#define TAU_BD_HEAVY_UP_AND_RIGHT 0x2517u                  // ┗
#define TAU_BD_LIGHT_UP_AND_LEFT 0x2518u                   // ┘
#define TAU_BD_UP_LIGHT_AND_LEFT_HEAVY 0x2519u             // ┙
#define TAU_BD_UP_HEAVY_AND_LEFT_LIGHT 0x251Au             // ┚
#define TAU_BD_HEAVY_UP_AND_LEFT 0x251Bu                   // ┛
#define TAU_BD_LIGHT_VERTICAL_AND_RIGHT 0x251Cu            // ├
#define TAU_BD_VERTICAL_LIGHT_AND_RIGHT_HEAVY 0x251Du      // ┝
#define TAU_BD_UP_HEAVY_AND_RIGHT_DOWN_LIGHT 0x251Eu       // ┞
#define TAU_BD_DOWN_HEAVY_AND_RIGHT_UP_LIGHT 0x251Fu       // ┟
#define TAU_BD_VERTICAL_HEAVY_AND_RIGHT_LIGHT 0x2520u      // ┠
#define TAU_BD_DOWN_LIGHT_AND_RIGHT_UP_HEAVY 0x2521u       // ┡
#define TAU_BD_UP_LIGHT_AND_RIGHT_DOWN_HEAVY 0x2522u       // ┢
#define TAU_BD_HEAVY_VERTICAL_AND_RIGHT 0x2523u            // ┣
#define TAU_BD_LIGHT_VERTICAL_AND_LEFT 0x2524u             // ┤
#define TAU_BD_VERTICAL_LIGHT_AND_LEFT_HEAVY 0x2525u       // ┥
#define TAU_BD_UP_HEAVY_AND_LEFT_DOWN_LIGHT 0x2526u        // ┦
#define TAU_BD_DOWN_HEAVY_AND_LEFT_UP_LIGHT 0x2527u        // ┧
#define TAU_BD_VERTICAL_HEAVY_AND_LEFT_LIGHT 0x2528u       // ┨
#define TAU_BD_DOWN_LIGHT_AND_LEFT_UP_HEAVY 0x2529u        // ┩
#define TAU_BD_UP_LIGHT_AND_LEFT_DOWN_HEAVY 0x252Au        // ┪
#define TAU_BD_HEAVY_VERTICAL_AND_LEFT 0x252Bu             // ┫
#define TAU_BD_LIGHT_DOWN_AND_HORIZONTAL 0x252Cu           // ┬
#define TAU_BD_LEFT_HEAVY_AND_RIGHT_DOWN_LIGHT 0x252Du     // ┭
#define TAU_BD_RIGHT_HEAVY_AND_LEFT_DOWN_LIGHT 0x252Eu     // ┮
#define TAU_BD_DOWN_LIGHT_AND_HORIZONTAL_HEAVY 0x252Fu     // ┯
#define TAU_BD_DOWN_HEAVY_AND_HORIZONTAL_LIGHT 0x2530u     // ┰
#define TAU_BD_RIGHT_LIGHT_AND_LEFT_DOWN_HEAVY 0x2531u     // ┱
#define TAU_BD_LEFT_LIGHT_AND_RIGHT_DOWN_HEAVY 0x2532u     // ┲
#define TAU_BD_HEAVY_DOWN_AND_HORIZONTAL 0x2533u           // ┳
#define TAU_BD_LIGHT_UP_AND_HORIZONTAL 0x2534u             // ┴
#define TAU_BD_LEFT_HEAVY_AND_RIGHT_UP_LIGHT 0x2535u       // ┵
#define TAU_BD_RIGHT_HEAVY_AND_LEFT_UP_LIGHT 0x2536u       // ┶
#define TAU_BD_UP_LIGHT_AND_HORIZONTAL_HEAVY 0x2537u       // ┷
#define TAU_BD_UP_HEAVY_AND_HORIZONTAL_LIGHT 0x2538u       // ┸
#define TAU_BD_RIGHT_LIGHT_AND_LEFT_UP_HEAVY 0x2539u       // ┹
#define TAU_BD_LEFT_LIGHT_AND_RIGHT_UP_HEAVY 0x253Au       // ┺
#define TAU_BD_HEAVY_UP_AND_HORIZONTAL 0x253Bu             // ┻
#define TAU_BD_LIGHT_VERTICAL_AND_HORIZONTAL 0x253Cu       // ┼
#define TAU_BD_LEFT_HEAVY_AND_RIGHT_VERTICAL_LIGHT 0x253Du // ┽
#define TAU_BD_RIGHT_HEAVY_AND_LEFT_VERTICAL_LIGHT 0x253Eu // ┾
#define TAU_BD_VERTICAL_LIGHT_AND_HORIZONTAL_HEAVY 0x253Fu // ┿
#define TAU_BD_UP_HEAVY_AND_DOWN_HORIZONTAL_LIGHT 0x2540u  // ╀
#define TAU_BD_DOWN_HEAVY_AND_UP_HORIZONTAL_LIGHT 0x2541u  // ╁
#define TAU_BD_VERTICAL_HEAVY_AND_HORIZONTAL_LIGHT 0x2542u // ╂
#define TAU_BD_LEFT_UP_HEAVY_AND_RIGHT_DOWN_LIGHT 0x2543u  // ╃
#define TAU_BD_RIGHT_UP_HEAVY_AND_LEFT_DOWN_LIGHT 0x2544u  // ╄
#define TAU_BD_LEFT_DOWN_HEAVY_AND_RIGHT_UP_LIGHT 0x2545u  // ╅
#define TAU_BD_RIGHT_DOWN_HEAVY_AND_LEFT_UP_LIGHT 0x2546u  // ╆
#define TAU_BD_DOWN_LIGHT_AND_UP_HORIZONTAL_HEAVY 0x2547u  // ╇
#define TAU_BD_UP_LIGHT_AND_DOWN_HORIZONTAL_HEAVY 0x2548u  // ╈
#define TAU_BD_RIGHT_LIGHT_AND_LEFT_VERTICAL_HEAVY 0x2549u // ╉
#define TAU_BD_LEFT_LIGHT_AND_RIGHT_VERTICAL_HEAVY 0x254Au // ╊
#define TAU_BD_HEAVY_VERTICAL_AND_HORIZONTAL 0x254Bu       // ╋

// Light and heavy dashed line box components
#define TAU_BD_LIGHT_DOUBLE_DASH_HORIZONTAL 0x254Cu // ╌
#define TAU_BD_HEAVY_DOUBLE_DASH_HORIZONTAL 0x254Du // ╍
#define TAU_BD_LIGHT_DOUBLE_DASH_VERTICAL 0x254Eu   // ╎
#define TAU_BD_HEAVY_DOUBLE_DASH_VERTICAL 0x254Fu   // ╏

// Double lines
#define TAU_BD_DOUBLE_HORIZONTAL 0x2550u                     // ═
#define TAU_BD_DOUBLE_VERTICAL 0x2551u                       // ║
#define TAU_BD_DOWN_SINGLE_AND_RIGHT_DOUBLE 0x2552u          // ╒
#define TAU_BD_DOWN_DOUBLE_AND_RIGHT_SINGLE 0x2553u          // ╓
#define TAU_BD_DOUBLE_DOWN_AND_RIGHT 0x2554u                 // ╔
#define TAU_BD_DOWN_SINGLE_AND_LEFT_DOUBLE 0x2555u           // ╕
#define TAU_BD_DOWN_DOUBLE_AND_LEFT_SINGLE 0x2556u           // ╖
#define TAU_BD_DOUBLE_DOWN_AND_LEFT 0x2557u                  // ╗
#define TAU_BD_UP_SINGLE_AND_RIGHT_DOUBLE 0x2558u            // ╘
#define TAU_BD_UP_DOUBLE_AND_RIGHT_SINGLE 0x2559u            // ╙
#define TAU_BD_DOUBLE_UP_AND_RIGHT 0x255Au                   // ╚
#define TAU_BD_UP_SINGLE_AND_LEFT_DOUBLE 0x255Bu             // ╛
#define TAU_BD_UP_DOUBLE_AND_LEFT_SINGLE 0x255Cu             // ╜
#define TAU_BD_DOUBLE_UP_AND_LEFT 0x255Du                    // ╝
#define TAU_BD_VERTICAL_SINGLE_AND_RIGHT_DOUBLE 0x255Eu      // ╞
#define TAU_BD_VERTICAL_DOUBLE_AND_RIGHT_SINGLE 0x255Fu      // ╟
#define TAU_BD_DOUBLE_VERTICAL_AND_RIGHT 0x2560u             // ╠
#define TAU_BD_VERTICAL_SINGLE_AND_LEFT_DOUBLE 0x2561u       // ╡
#define TAU_BD_VERTICAL_DOUBLE_AND_LEFT_SINGLE 0x2562u       // ╢
#define TAU_BD_DOUBLE_VERTICAL_AND_LEFT 0x2563u              // ╣
#define TAU_BD_DOWN_SINGLE_AND_HORIZONTAL_DOUBLE 0x2564u     // ╤
#define TAU_BD_DOWN_DOUBLE_AND_HORIZONTAL_SINGLE 0x2565u     // ╥
#define TAU_BD_DOUBLE_DOWN_AND_HORIZONTAL 0x2566u            // ╦
#define TAU_BD_UP_SINGLE_AND_HORIZONTAL_DOUBLE 0x2567u       // ╧
#define TAU_BD_UP_DOUBLE_AND_HORIZONTAL_SINGLE 0x2568u       // ╨
#define TAU_BD_DOUBLE_UP_AND_HORIZONTAL 0x2569u              // ╩
#define TAU_BD_VERTICAL_SINGLE_AND_HORIZONTAL_DOUBLE 0x256Au // ╪
#define TAU_BD_VERTICAL_DOUBLE_AND_HORIZONTAL_SINGLE 0x256Bu // ╫
#define TAU_BD_DOUBLE_VERTICAL_AND_HORIZONTAL 0x256Cu        // ╬

// Arcs
#define TAU_BD_LIGHT_ARC_DOWN_AND_RIGHT 0x256Du // ╭
#define TAU_BD_LIGHT_ARC_DOWN_AND_LEFT 0x256Eu  // ╮
#define TAU_BD_LIGHT_ARC_UP_AND_LEFT 0x256Fu    // ╯
#define TAU_BD_LIGHT_ARC_UP_AND_RIGHT 0x2570u   // ╰

// Diagonal line box components
#define TAU_BD_LIGHT_DIAGONAL_UPPER_RIGHT_TO_LOWER_LEFT 0x2571u // ╱
#define TAU_BD_LIGHT_DIAGONAL_UPPER_LEFT_TO_LOWER_RIGHT 0x2572u // ╲
#define TAU_BD_LIGHT_DIAGONAL_CROSS 0x2573u                     // ╳

// Light and heavy half line box components
#define TAU_BD_LIGHT_LEFT 0x2574u                 // ╴
#define TAU_BD_LIGHT_UP 0x2575u                   // ╵
#define TAU_BD_LIGHT_RIGHT 0x2576u                // ╶
#define TAU_BD_LIGHT_DOWN 0x2577u                 // ╷
#define TAU_BD_HEAVY_LEFT 0x2578u                 // ╸
#define TAU_BD_HEAVY_UP 0x2579u                   // ╹
#define TAU_BD_HEAVY_RIGHT 0x257Au                // ╺
#define TAU_BD_HEAVY_DOWN 0x257Bu                 // ╻
#define TAU_BD_LIGHT_LEFT_AND_HEAVY_RIGHT 0x257Cu // ╼
#define TAU_BD_LIGHT_UP_AND_HEAVY_DOWN 0x257Du    // ╽
#define TAU_BD_HEAVY_LEFT_AND_LIGHT_RIGHT 0x257Eu // ╾
#define TAU_BD_HEAVY_UP_AND_LIGHT_DOWN 0x257Fu    // ╿

#endif
