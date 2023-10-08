#include <X11/XF86keysym.h>
/* See LICENSE file for copyright and license details. */

/* appearance */
static const unsigned int borderpx  = 2;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const int showbar            = 1;        /* 0 means so bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const char *fonts[]          = { "monospace:size=10" };
static const char dmenufont[]       = "monospace:size=10";
// static const char col_gray1[]       = "#222222";
static const char col_gray2[]       = "#444444";
// static const char col_gray3[]       = "#bbbbbb";
// static const char col_gray4[]       = "#eeeeee";
static const char col_black[]       = "#000000";
static const char col_white[]       = "#ffffff";
static const char col_purple[]      = "#7F00A4";
static const char *colors[][3]      = { 
	/*               fg         bg          border   */
	[SchemeNorm] = { col_white, col_black,  col_gray2  },
	[SchemeSel]  = { col_white, col_purple, col_purple },
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance    title       tags mask     isfloating   monitor */
	{ NULL,       NULL,       NULL,       0,            False,       -1 },
};

/* layout(s) */
// 19 / 32 == 0.59375. It's the largest width where the slave area is 80 chars wide.
static const float mfact     = 0.59375; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 0; /* 1 will force focus on the fullscreen window */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
};

/* key definitions */
#define MODKEY Mod1Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", col_black, "-nf", col_purple, "-sb", col_purple, "-sf", col_white, NULL };
static const char *termcmd[]  = { "st", NULL };

static const Key keys[] = {
	/* modifier                    key                function        argument */
	{      0,                       XF86XK_AudioMute, spawn,          SHCMD("/usr/bin/wpctl set-mute @DEFAULT_SINK@ toggle; kill -36 $(pidof dwmblocks)") },
	{      0,                XF86XK_AudioLowerVolume, spawn,          SHCMD("/usr/bin/wpctl set-volume @DEFAULT_SINK@ 5%-; kill -36 $(pidof dwmblocks)") },
	{      0,                XF86XK_AudioRaiseVolume, spawn,          SHCMD("/usr/bin/wpctl set-volume @DEFAULT_SINK@ 5%+; kill -36 $(pidof dwmblocks)") },
	{ ShiftMask,             XF86XK_AudioLowerVolume, spawn,          SHCMD("/usr/bin/wpctl set-volume @DEFAULT_SINK@ 1%-; kill -36 $(pidof dwmblocks)") },
	{ ShiftMask,             XF86XK_AudioRaiseVolume, spawn,          SHCMD("/usr/bin/wpctl set-volume @DEFAULT_SINK@ 1%+; kill -36 $(pidof dwmblocks)") },
	{ ControlMask,           XF86XK_AudioLowerVolume, spawn,          SHCMD("/usr/bin/wpctl set-volume @DEFAULT_SOURCE@ 5%-; kill -35 $(pidof dwmblocks)") },
	{ ControlMask,           XF86XK_AudioRaiseVolume, spawn,          SHCMD("/usr/bin/wpctl set-volume @DEFAULT_SOURCE@ 5%+; kill -35 $(pidof dwmblocks)") },
	{ ControlMask|ShiftMask, XF86XK_AudioLowerVolume, spawn,          SHCMD("/usr/bin/wpctl set-volume @DEFAULT_SOURCE@ 1%-; kill -35 $(pidof dwmblocks)") },
	{ ControlMask|ShiftMask, XF86XK_AudioRaiseVolume, spawn,          SHCMD("/usr/bin/wpctl set-volume @DEFAULT_SOURCE@ 1%+; kill -35 $(pidof dwmblocks)") },
	{      0,                    XF86XK_AudioMicMute, spawn,          SHCMD("/usr/bin/wpctl set-mute @DEFAULT_SOURCE@ toggle; kill -35 $(pidof dwmblocks)") },
	{      0,               XF86XK_MonBrightnessDown, spawn,          SHCMD("/usr/bin/brightnessctl set 5%-; dwm_brightness.sh") },
	{      0,                 XF86XK_MonBrightnessUp, spawn,          SHCMD("/usr/bin/brightnessctl set 5%+; dwm_brightness.sh") },
	{ ShiftMask,            XF86XK_MonBrightnessDown, spawn,          SHCMD("/usr/bin/brightnessctl set 1%-; dwm_brightness.sh") },
	{ ShiftMask,              XF86XK_MonBrightnessUp, spawn,          SHCMD("/usr/bin/brightnessctl set 1%+; dwm_brightness.sh") },
	// The snipping icon below PrtSc couldn't be found anywhere in
	// /usr/include/X11/{keysymdef.h,keysym.h,XF86keysym.h}.
	// {XK_Select,XF86XK_Copy,XF86XK_Cut,XF86XK_Select} - none worked.
	// So for now, Control + PrtSc = select area for screenshot.
	{ ControlMask,                 XK_Print,          spawn,          SHCMD("/usr/bin/scrot -s -fz '%Y-%m-%d_%H:%M:%S.png' -e 'mv $f ~/Pictures/Screenshots/'") },
	{      0,                      XK_Print,          spawn,          SHCMD("/usr/bin/scrot -z '%Y-%m-%d_%H:%M:%S.png' -e 'mv $f ~/Pictures/Screenshots/'") },
	{ MODKEY,                      XK_p,              spawn,          {.v = dmenucmd } },
	{ MODKEY|ShiftMask,            XK_Return,         spawn,          {.v = termcmd } },
	{ MODKEY,                      XK_b,              togglebar,      {0} },
	{ MODKEY,                      XK_j,              focusstack,     {.i = +1 } },
	{ MODKEY,                      XK_k,              focusstack,     {.i = -1 } },
	{ MODKEY,                      XK_i,              incnmaster,     {.i = +1 } },
	{ MODKEY,                      XK_d,              incnmaster,     {.i = -1 } },
	{ MODKEY,                      XK_h,              setmfact,       {.f = -(1.0 / 32)} },
	{ MODKEY,                      XK_l,              setmfact,       {.f = +(1.0 / 32)} },
	{ MODKEY,                      XK_Return,         zoom,           {0} },
	{ MODKEY,                      XK_Tab,            view,           {0} },
	{ MODKEY|ShiftMask,            XK_c,              killclient,     {0} },
	{ MODKEY,                      XK_t,              setlayout,      {.v = &layouts[0]} },
	{ MODKEY,                      XK_f,              setlayout,      {.v = &layouts[1]} },
	{ MODKEY,                      XK_m,              setlayout,      {.v = &layouts[2]} },
	{ MODKEY,                      XK_space,          setlayout,      {0} },
	{ MODKEY|ShiftMask,            XK_space,          togglefloating, {0} },
	{ MODKEY,                      XK_0,              view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,            XK_0,              tag,            {.ui = ~0 } },
	{ MODKEY,                      XK_comma,          focusmon,       {.i = -1 } },
	{ MODKEY,                      XK_period,         focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,            XK_comma,          tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,            XK_period,         tagmon,         {.i = +1 } },
	{ MODKEY|ShiftMask,            XK_q,              quit,           {0} },
	TAGKEYS(                       XK_1,                               0)
	TAGKEYS(                       XK_2,                               1)
	TAGKEYS(                       XK_3,                               2)
	TAGKEYS(                       XK_4,                               3)
	TAGKEYS(                       XK_5,                               4)
	TAGKEYS(                       XK_6,                               5)
	TAGKEYS(                       XK_7,                               6)
	TAGKEYS(                       XK_8,                               7)
	TAGKEYS(                       XK_9,                               8)
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
