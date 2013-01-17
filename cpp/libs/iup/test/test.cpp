#include "iup/iup.hpp"
#include <iostream>
#define L(m) std::cout << m << std::endl

int exit_cb (void) {
    L("jawel");
  return IUP_CLOSE;
}

int main (int argc, char *argv[]) {

  // declare widgets
  Ihandle *btn, *lbl, *vb, *dlg;

  // initialize iup
  IupOpen(&argc, &argv);
  
  // create widgets and set their attributes
  btn=IupButton("&ok?", "");
  IupSetCallback(btn,"ACTION", (Icallback) exit_cb);
  IupSetAttribute(btn, "EXPAND", "Yes");
  IupSetAttribute(btn, "TIP", "Exit\nbutton");
  
  lbl=IupLabel("oeuao aouaoeu");

  vb=IupVbox(lbl, btn, 0);
  IupSetAttribute(vb, "GAP", "10");
  IupSetAttribute(vb, "MARGIN", "10x10");
  IupSetAttribute(vb, "ALIGNMENT", "ACENTER");

  dlg=IupDialog(vb);
  IupSetAttribute(dlg, "TITLE", "Hello");

  // Map widgets and show dialog
  IupShow(dlg);

  // Wait for user interaction
  IupMainLoop();

  // Clean up
  IupDestroy(dlg);
  IupClose();
  return 0;
}
