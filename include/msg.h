#ifndef TUI_MSG_H
#define TUI_MSG_H

extern void msgnew(int row , int column, int columns, char* buffer);
extern void msgdrawall();
extern void msgfreeall();
extern void msgfreealluser();

#endif
