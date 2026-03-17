#include "../../src/cxBase.h"
#include "FlashcardApp.h"

int main() {
    cxBase::init();
    
    FlashcardApp app(15, 60);
    app.show();
    app.run();
    
    cxBase::cleanup();
    return 0;
}
