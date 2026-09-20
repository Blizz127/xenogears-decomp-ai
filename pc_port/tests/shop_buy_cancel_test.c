/* Execute the production Buy caller and row builder together. The other menu
 * helpers are isolated: one ordinary Back input exits without a transaction. */
#define main shop_list_differential_main
#include "shop_list_retail_test.c"
#undef main
#include "main/game.h"
GameState g_GameState;
int D_801D1F50 = 8;
s32 D_801D1FD4[8], D_801D1FE8[8];
u16 D_801D2260;
static MenuManager manager;
static MenuSelectionMenu selection;
static unsigned updates, selected;
extern u_char ShopMenuBuyMenu(void);
void func_801CCE1C(void* output, u8 id) { (void)output; (void)id; }
void ShopMenuUpdateAndRender(void) { ++updates; menu.input = MENU_INPUT_BACK; }
s32 func_801CEB3C(s32 row, s32 offset, u8* affordable) {
 require(row == 0 && offset == 0, "initial selection");
 for (unsigned i=0;i<8;i++) require(affordable[i] == 0x80, "all eight caller flags");
 ++selected; return 20;
}
#define NOOP(name) void name(void) {}
NOOP(ShopMenuInitializeArrowCursor)
NOOP(ShopMenuUpdateScrollBarHandle)
NOOP(ShopMenuUpdateArrowCursor)
NOOP(func_801CBC88)
NOOP(func_801CBCF0)
NOOP(ShopMenuInitializeWindow)
NOOP(ShopMenuStartOpenMenuTransition)
NOOP(ShopMenuUpdateCharacterPortraits)
NOOP(ShopMenuUpdateBuyMenuExplanationGraphics)
NOOP(ShopMenuUpdateGoldGraphics)
NOOP(ShopMenuPlaySoundEffect)
NOOP(ShopMenuInitializePointerCursors)
NOOP(ShopMenuFreePointerCursors)
NOOP(ShopMenuParseNumberToString)
int ShopMenuConfirmationWindow(void) { require(0,"unexpected confirmation"); return 0; }
int main(void) {
 menu.pShop=&shop; menu.unk330=&resources; menu.pManager=&manager;
 menu.pSelectionMenu=&selection; menu.unk2DC=(void*)ATLAS;
 resources.pItemsData=items; items[1].price=20;
 for (unsigned i=0;i<8;i++) { menu.shopItemIDs[i]=1; menu.shopItemTypes[i]=2; }
 g_GameState.gold=300;
 require(ShopMenuBuyMenu()==TRUE,"normal cancel returns");
 require(updates==2 && selected==1,"one frame and exit update");
 require(g_GameState.gold==300,"cancel preserves gold");
 for(unsigned i=0;i<MAX_SHOP_ITEMS;i++) require(shop.curItemQuantities[i]==0,"no purchase");
 puts("SHOP BUY CANCEL PASS: production caller and eight-row builder");
 return 0;
}
