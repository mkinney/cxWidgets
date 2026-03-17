#include "../../src/cxBase.h"
#include "CentipedeGame.h"

int main() {
    cxBase::init();
    
    CentipedeGame game(24, 80);
    game.show();
    game.run();
    
    cxBase::cleanup();
    return 0;
}
