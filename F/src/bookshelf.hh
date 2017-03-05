#ifndef		BOOK_SHELF_HH
# define	BOOK_SHELF_HH

#include "node.hh"

namespace BookShelf
{
  Node * create();
  void generateMeshes();
  void bookDominoFall(const Node &node, date d);
  void lastBookDominoFall(const Node &node, date d);

  enum font {
    georgia = 0,
    impact,
    palatino,
    serif,
    roman,
    verdana,
  };

  struct bookDesc {
    char color;
    char fontName;
    char fontSize;
    char * title;
  };

  const bookDesc & greetingsBook(int i);
  int numberOfGreetingsBooks();
}

#endif BOOK_SHELF_HH
