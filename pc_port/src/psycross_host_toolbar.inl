/* Host-only clickable controls drawn in pixels reserved above the emulated
 * framebuffer. The bar is presented to the player but deliberately rendered
 * after recording readback, so it never contaminates game captures. */
#include "psycross_host_toolbar_logic.h"
#include "quick_checkpoint_request.h"

#if !defined(__EMSCRIPTEN__) && !defined(__ANDROID__) && \
	(defined(RENDERER_OGL) || defined(RENDERER_OGLES))

static int g_xenoHostToolbarActive = 0;
static PcPortHostToolbarAction g_xenoHostToolbarHover = PC_PORT_TOOLBAR_NONE;

static PcPortQuickUiState PsyX_HostToolbarQuickUiState()
{
#if defined(__GNUC__)
	if (PcPort_QuickCheckpointGetUiState != NULL)
		return (PcPortQuickUiState)PcPort_QuickCheckpointGetUiState();
#endif
	return PC_PORT_QUICK_UI_IDLE;
}

static int PsyX_HostToolbarContentHeight(int windowHeight)
{
	if (!g_xenoHostToolbarActive)
		return windowHeight;
	return windowHeight > PC_PORT_HOST_TOOLBAR_HEIGHT
		? windowHeight - PC_PORT_HOST_TOOLBAR_HEIGHT : 1;
}

static int PsyX_HostToolbarWarpY(int gameY)
{
	return g_xenoHostToolbarActive
		? gameY + PC_PORT_HOST_TOOLBAR_HEIGHT : gameY;
}

static void PsyX_HostToolbarInitialise()
{
	const char* enabled = getenv("XENO_HOST_TOOLBAR");
	int width;
	int height;

	if (enabled != NULL && enabled[0] == '0' && enabled[1] == '\0')
		return;
	SDL_GetWindowSize(g_window, &width, &height);
	g_xenoHostToolbarActive = 1;
	SDL_SetWindowMinimumSize(g_window, 400,
		240 + PC_PORT_HOST_TOOLBAR_HEIGHT);
	SDL_SetWindowSize(g_window, width,
		height + PC_PORT_HOST_TOOLBAR_HEIGHT);
	/* Keep renderer dimensions as the game content dimensions. The queued SDL
	 * resize event is normalized by PsyX_HostToolbarHandleEvent. */
	g_windowWidth = width;
	g_windowHeight = height;
	eprintinfo("Host toolbar enabled: Quick Save | Quick Load | Record | Speed (F11 cycles, Shift+F11 resets; hold Backspace for 5x)\n");
}

static void PsyX_HostToolbarDispatch(PcPortHostToolbarAction action)
{
	switch (action) {
	case PC_PORT_TOOLBAR_QUICK_SAVE:
#if defined(__GNUC__)
		if (PcPort_QuickCheckpointRequestSave != NULL)
			PcPort_QuickCheckpointRequestSave();
#endif
		break;
	case PC_PORT_TOOLBAR_QUICK_LOAD:
#if defined(__GNUC__)
		if (PcPort_QuickCheckpointRequestLoad != NULL)
			PcPort_QuickCheckpointRequestLoad();
#endif
		break;
	case PC_PORT_TOOLBAR_RECORD:
		PsyX_ToggleRecording();
		break;
	case PC_PORT_TOOLBAR_SPEED:
		PsyX_SetSpeedMultiplier(PsyX_GetSpeedMultiplier() % 5 + 1);
		break;
	default:
		break;
	}
}

static int PsyX_HostToolbarHandleEvent(SDL_Event* event)
{
	PcPortHostToolbarAction action;

	if (!g_xenoHostToolbarActive)
		return 0;
	if (event->type == SDL_WINDOWEVENT &&
		(event->window.event == SDL_WINDOWEVENT_RESIZED ||
		 event->window.event == SDL_WINDOWEVENT_SIZE_CHANGED)) {
		event->window.data2 =
			PsyX_HostToolbarContentHeight(event->window.data2);
		return 0;
	}
	if (event->type == SDL_MOUSEMOTION) {
		action = PcPort_HostToolbarHitTest(event->motion.x, event->motion.y);
		g_xenoHostToolbarHover = action;
		if (event->motion.y < PC_PORT_HOST_TOOLBAR_HEIGHT) {
			SDL_ShowCursor(SDL_ENABLE);
			return 1;
		}
		SDL_ShowCursor(SDL_DISABLE);
		event->motion.y -= PC_PORT_HOST_TOOLBAR_HEIGHT;
		return 0;
	}
	if (event->type == SDL_MOUSEBUTTONDOWN ||
		event->type == SDL_MOUSEBUTTONUP) {
		if (event->button.y < PC_PORT_HOST_TOOLBAR_HEIGHT) {
			if (event->type == SDL_MOUSEBUTTONUP &&
				event->button.button == SDL_BUTTON_LEFT) {
				action = PcPort_HostToolbarHitTest(
					event->button.x, event->button.y);
				PsyX_HostToolbarDispatch(action);
			}
			return 1;
		}
		event->button.y -= PC_PORT_HOST_TOOLBAR_HEIGHT;
	}
	return 0;
}

static const unsigned char* PsyX_HostToolbarGlyph(char ch)
{
	static const unsigned char glyphA[7] = {14, 17, 17, 31, 17, 17, 17};
	static const unsigned char glyphC[7] = {14, 17, 16, 16, 16, 17, 14};
	static const unsigned char glyphD[7] = {30, 17, 17, 17, 17, 17, 30};
	static const unsigned char glyphE[7] = {31, 16, 16, 30, 16, 16, 31};
	static const unsigned char glyphI[7] = {31, 4, 4, 4, 4, 4, 31};
	static const unsigned char glyphL[7] = {16, 16, 16, 16, 16, 16, 31};
	static const unsigned char glyphO[7] = {14, 17, 17, 17, 17, 17, 14};
	static const unsigned char glyphP[7] = {30, 17, 17, 30, 16, 16, 16};
	static const unsigned char glyphR[7] = {30, 17, 17, 30, 20, 18, 17};
	static const unsigned char glyphS[7] = {15, 16, 16, 14, 1, 1, 30};
	static const unsigned char glyphT[7] = {31, 4, 4, 4, 4, 4, 4};
	static const unsigned char glyphV[7] = {17, 17, 17, 17, 17, 10, 4};
	static const unsigned char glyphW[7] = {17, 17, 17, 17, 21, 21, 10};
	static const unsigned char glyphX[7] = {17, 17, 10, 4, 10, 17, 17};
	static const unsigned char digits[5][7] = {
		{4, 12, 4, 4, 4, 4, 14},
		{14, 17, 1, 2, 4, 8, 31},
		{30, 1, 1, 14, 1, 1, 30},
		{2, 6, 10, 18, 31, 2, 2},
		{31, 16, 16, 30, 1, 1, 30}
	};
	if (ch >= '1' && ch <= '5') return digits[ch - '1'];

	switch (ch) {
	case 'A': return glyphA;
	case 'C': return glyphC;
	case 'D': return glyphD;
	case 'E': return glyphE;
	case 'I': return glyphI;
	case 'L': return glyphL;
	case 'O': return glyphO;
	case 'P': return glyphP;
	case 'R': return glyphR;
	case 'S': return glyphS;
	case 'T': return glyphT;
	case 'V': return glyphV;
	case 'W': return glyphW;
	case 'X': return glyphX;
	default: return NULL;
	}
}

static void PsyX_HostToolbarClearRect(int x, int y, int width, int height,
	float red, float green, float blue)
{
	glScissor(x, y, width, height);
	glClearColor(red, green, blue, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

static void PsyX_HostToolbarDrawText(const char* text, int x, int y)
{
	const int scale = 2;
	for (; *text != '\0'; ++text, x += 12) {
		const unsigned char* glyph = PsyX_HostToolbarGlyph(*text);
		int row;
		if (glyph == NULL)
			continue;
		for (row = 0; row < 7; ++row) {
			int column = 0;
			while (column < 5) {
				int start;
				while (column < 5 && !(glyph[row] & (1 << (4 - column))))
					column++;
				start = column;
				while (column < 5 && (glyph[row] & (1 << (4 - column))))
					column++;
				if (column > start)
					PsyX_HostToolbarClearRect(x + start * scale,
						y + (6 - row) * scale,
						(column - start) * scale, scale,
						0.94f, 0.94f, 0.97f);
			}
		}
	}
}

static void PsyX_HostToolbarDrawButton(int x, int width,
	PcPortHostToolbarAction action, const char* label, int labelX, int state)
{
	float shade = g_xenoHostToolbarHover == action ? 0.30f : 0.20f;
	float red = shade;
	float green = shade;
	float blue = shade + 0.04f;

	if (state == 1) {
		red = 0.64f;
		green = 0.43f;
		blue = 0.05f;
	} else if (state == 2) {
		red = 0.08f;
		green = 0.48f;
		blue = 0.18f;
	} else if (state == 3 ||
		(action == PC_PORT_TOOLBAR_RECORD && PsyX_IsRecording())) {
		red = 0.62f;
		green = 0.10f;
		blue = 0.12f;
	}
	PsyX_HostToolbarClearRect(x, g_windowHeight + 4, width, 26,
		0.48f, 0.48f, 0.53f);
	PsyX_HostToolbarClearRect(x + 1, g_windowHeight + 5, width - 2, 24,
		red, green, blue);
	PsyX_HostToolbarDrawText(label, labelX, g_windowHeight + 10);
}

static void PsyX_HostToolbarDraw()
{
	PcPortQuickUiState quickState;
	const char* saveLabel = "SAVE";
	const char* loadLabel = "LOAD";
	int saveLabelX = 24;
	int loadLabelX = 112;
	int saveState = 0;
	int loadState = 0;
	char speedLabel[] = "SPEED 1X";
	GLboolean scissorEnabled;
	GLboolean colorMask[4];
	GLint oldScissor[4];
	GLfloat oldClearColor[4];

	if (!g_xenoHostToolbarActive)
		return;
	scissorEnabled = glIsEnabled(GL_SCISSOR_TEST);
	glGetBooleanv(GL_COLOR_WRITEMASK, colorMask);
	glGetIntegerv(GL_SCISSOR_BOX, oldScissor);
	glGetFloatv(GL_COLOR_CLEAR_VALUE, oldClearColor);
	glEnable(GL_SCISSOR_TEST);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	quickState = PsyX_HostToolbarQuickUiState();
	if (quickState == PC_PORT_QUICK_UI_SAVE_PENDING) {
		saveLabel = "WAIT";
		saveState = 1;
	} else if (quickState == PC_PORT_QUICK_UI_SAVE_OK) {
		saveLabel = "SAVED";
		saveLabelX = 18;
		saveState = 2;
	} else if (quickState == PC_PORT_QUICK_UI_SAVE_ERROR) {
		saveLabel = "ERROR";
		saveLabelX = 18;
		saveState = 3;
	} else if (quickState == PC_PORT_QUICK_UI_LOAD_PENDING) {
		loadLabel = "WAIT";
		loadState = 1;
	} else if (quickState == PC_PORT_QUICK_UI_LOAD_OK) {
		loadLabel = "LOADED";
		loadLabelX = 100;
		loadState = 2;
	} else if (quickState == PC_PORT_QUICK_UI_LOAD_ERROR) {
		loadLabel = "ERROR";
		loadLabelX = 106;
		loadState = 3;
	}

	PsyX_HostToolbarClearRect(0, g_windowHeight, g_windowWidth,
		PC_PORT_HOST_TOOLBAR_HEIGHT, 0.07f, 0.07f, 0.09f);
	PsyX_HostToolbarDrawButton(8, 80, PC_PORT_TOOLBAR_QUICK_SAVE,
		saveLabel, saveLabelX, saveState);
	PsyX_HostToolbarDrawButton(96, 80, PC_PORT_TOOLBAR_QUICK_LOAD,
		loadLabel, loadLabelX, loadState);
	PsyX_HostToolbarDrawButton(184, 96, PC_PORT_TOOLBAR_RECORD,
		PsyX_IsRecording() ? "STOP" : "RECORD",
		PsyX_IsRecording() ? 208 : 196, 0);
	speedLabel[6] = '0' + PsyX_GetSpeedMultiplier();
	PsyX_HostToolbarDrawButton(288, 104, PC_PORT_TOOLBAR_SPEED,
		speedLabel, 294, PsyX_GetSpeedMultiplier() > 1 ? 1 : 0);

	glClearColor(oldClearColor[0], oldClearColor[1],
		oldClearColor[2], oldClearColor[3]);
	glColorMask(colorMask[0], colorMask[1], colorMask[2], colorMask[3]);
	glScissor(oldScissor[0], oldScissor[1], oldScissor[2], oldScissor[3]);
	if (!scissorEnabled)
		glDisable(GL_SCISSOR_TEST);
}

static void PsyX_HostToolbarShutdown()
{
	g_xenoHostToolbarActive = 0;
	g_xenoHostToolbarHover = PC_PORT_TOOLBAR_NONE;
}

#else

static int PsyX_HostToolbarContentHeight(int height) { return height; }
static int PsyX_HostToolbarWarpY(int y) { return y; }
static void PsyX_HostToolbarInitialise() {}
static int PsyX_HostToolbarHandleEvent(SDL_Event*) { return 0; }
static void PsyX_HostToolbarDraw() {}
static void PsyX_HostToolbarShutdown() {}

#endif
