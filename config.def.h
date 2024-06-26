/* See LICENSE file for copyright and license details. */

/* includes */
#include <X11/XF86keysym.h>

/* appearance */
static const unsigned int borderpx  = 2;        /* border pixel of windows */
static const unsigned int gappx     = 0;        /* gaps between windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const int swallowfloating    = 1;        /* 1 means swallow floating windows by default */
static const unsigned int systraypinning = 0;   /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor X */
static const unsigned int systrayonleft = 0;    /* 0: systray in the right corner, >0: systray on left of status text */
static const unsigned int systrayspacing = 3;   /* systray spacing */
static const int systraypinningfailfirst = 1;   /* 1: if pinning fails, display systray on the first monitor, False: display systray on the last monitor*/
static const int showsystray        = 1;        /* 0 means no systray */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const char *fonts[]          = { "JetBrainsMono Nerd Font:size=11", "Noto Color Emoji:pixelsize=11:antialias=true:autohint=true" };
static const char dmenufont[]       = "monospace:size=10";
static const char col_gray1[]       = "#282c34";
static const char col_gray2[]       = "#444444";
static const char col_gray3[]       = "#bbc2cf";
static const char col_gray4[]       = "#282c34";
static const char col_cyan[]        = "#a9a1e1";
static const char *colors[][3]      = {
	/*               fg         bg         border   */
    [SchemeNorm] = { col_gray3, col_gray1, col_gray2 },
	[SchemeSel]  = { col_gray4, col_cyan,  col_cyan  },
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class     instance  title           tags mask  isfloating  isterminal  noswallow  monitor */
	{ "Gimp",     NULL,     NULL,           0,         1,          0,           0,        -1 },
	{ "firefox",  NULL,     NULL,           2,         0,          0,          -1,        -1 },
	{ "Firefox",  NULL,     NULL,           2,         0,          0,          -1,        -1 },
	{ "St",       NULL,     NULL,           0,         0,          1,           0,        -1 },
	{ "Alacritty",NULL,     NULL,           0,         0,          1,           0,        -1 },
	{ "kitty",    NULL,     NULL,           0,         0,          1,           0,        -1 },
	{ "org.wezfurlong.wezterm",NULL,     NULL,           0,         0,          1,           0,        -1 },
	{ "Pavucontrol", NULL,    NULL,        0,         1,          0,            0,       -1 },
	{ NULL,      NULL,     "Event Tester", 0,         0,          0,           1,        -1 }, /* xev */

};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
};

/* key definitions */
#define XK_PrintScreen	    0x0000ff61
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-i", "-F", "-p", "Run >", NULL};
static const char *termcmd[]  = { "alacritty", NULL };
static const char *termcmd2[]  = { "wezterm", NULL };

static const Key keys[] = {
	/* modifier                     key        function        argument */
	{ MODKEY,                       XK_r,      spawn,          {.v = dmenucmd } },
	{ MODKEY,                       XK_Return, spawn,          {.v = termcmd } },
	{ MODKEY,                       XK_Return, spawn,          {.v = termcmd2 } },
	{ MODKEY,                       XK_b,      togglebar,      {0} }, // toggle bar
	{ MODKEY,                       XK_j,      focusstack,     {.i = +1 } }, // resizing
	{ MODKEY,                       XK_k,      focusstack,     {.i = -1 } }, // resizing
	{ MODKEY,                       XK_i,      incnmaster,     {.i = +1 } }, // increment to master
	{ MODKEY,                       XK_d,      incnmaster,     {.i = -1 } }, // decrement from master
	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },
	{ MODKEY|ShiftMask,             XK_Return, zoom,           {0} },
	{ MODKEY,                       XK_Tab,    view,           {0} },
	{ MODKEY|ShiftMask,             XK_q,      killclient,     {0} },
	{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} },
	{ MODKEY,                       XK_f,      setlayout,      {.v = &layouts[1]} },
	{ MODKEY,                       XK_m,      setlayout,      {.v = &layouts[2]} },
	{ MODKEY,                       XK_space,  setlayout,      {0} },
	{ MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	/* for multi monitor set up */
	/* { MODKEY,                       XK_comma,  focusmon,       {.i = -1 } }, */
	/* { MODKEY,                       XK_period, focusmon,       {.i = +1 } }, */
	/* { MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } }, */
	/* { MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } }, */

	{ MODKEY,                       XK_minus,  setgaps,        {.i = -1 } },
	{ MODKEY,                       XK_equal,  setgaps,        {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_equal,  setgaps,        {.i = 0  } },

	/* my custome maps */
	{ MODKEY,                       XK_period,  spawn,          SHCMD("emoji") },
	{ MODKEY|ShiftMask,             XK_r,       spawn,          SHCMD("rofi -show drun") },
	{ ControlMask,                  XK_Escape,  spawn,          SHCMD("dwm-power-menu") },
	{ MODKEY,                       XK_v,       spawn,          SHCMD("copyq menu") },
	{ MODKEY,                       XK_w,       spawn,          SHCMD("x11-wallpaper") },
    
	/* programs */
	/* { Mod1Mask,                     XK_e,       spawn,          SHCMD("pcmanfm") }, */
	/* { Mod1Mask,                     XK_b,       spawn,          SHCMD("firefox") }, */
	{ MODKEY,                       XK_g,       spawn,          SHCMD("dmenu_google.sh") },
    
	/* mpc control */
	/* { Mod1Mask,                     XK_l,       spawn,          SHCMD("mpc prev") }, */
	/* { Mod1Mask,                     XK_k,       spawn,          SHCMD("mpc toggle") }, */
	/* { Mod1Mask,                     XK_j,       spawn,          SHCMD("mpc next") }, */

	/* volume control */
	{ 0,                           XF86XK_AudioLowerVolume,    spawn, SHCMD("volume_set -d") },
	{ 0,                           XF86XK_AudioRaiseVolume,    spawn, SHCMD("volume_set -i") },
	{ 0,                           XF86XK_AudioMute,           spawn, SHCMD("volume_set -m") },

	/* brightness control */
	{ 0,                           XF86XK_MonBrightnessUp,     spawn, SHCMD("brightness_set -i") },
	{ 0,                           XF86XK_MonBrightnessDown,   spawn, SHCMD("brightness_set -d") },
  
	/* media control */
	{ 0,                           XF86XK_AudioPause,          spawn, SHCMD("playerctl play-pause") },
	{ 0,                           XF86XK_AudioNext,           spawn, SHCMD("playerctl next") },
	{ 0,                           XF86XK_AudioPrev,           spawn, SHCMD("playerctl previous") },
    
	{ 0,                           XK_PrintScreen,             spawn, SHCMD("flameshot gui --path=$HOME/Pictures/Screenshots/") },
	{ MODKEY,                      XK_PrintScreen,             spawn, SHCMD("flameshot full --path=$HOME/Pictures/Screenshots/") },

	/* tages */
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
	{ MODKEY|ShiftMask,             XK_c,      quit,           {0} },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static const Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

