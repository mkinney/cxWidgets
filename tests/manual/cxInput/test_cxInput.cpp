// Copyright (c) 2026 E. Oulashin
#include <cxBase.h>
#include <cxInput.h>
#include <string>
#include <fstream>
#include <iostream>

using namespace std;

int main(int argc, char* argv[]) {
   cxBase::init();

   string initial;
   if (argc == 2) {
      initial=argv[1];
   }

   cxInput input1(NULL, 0, 0, 5, "A:");
   // input1.setValue(initial);
   input1.showModal();
   cxBase::cleanup();

   // now output what we set the value to
   ofstream log("test_cxInput.log", ios::out | ios::trunc); // writing/truncating
   log << input1.getValue();
   log.close();

   return(0);
}
