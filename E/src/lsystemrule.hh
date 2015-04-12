//
// L system rule
//

#ifndef		L_SYSTEM_RULE_HH
# define	L_SYSTEM_RULE_HH

#define ALPHABET ".+-&^<>[]XFGH"

namespace LSystem
{
  class Rule
  {
  public:
    Rule(const char * src);
    ~Rule();

    int		length() const;
    char	operator [](unsigned int i) const;

  private:
    int		_length;
    char *	_rule;
  };
}

#endif		// L_SYSTEM_RULE_HH
