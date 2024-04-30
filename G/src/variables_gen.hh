// A revoir
#if 0
#ifndef		VARIABLES_GEN_HH
# define	VARIABLES_GEN_HH
# include "variable.hh"

namespace Variables
{
  extern Variable fade;
}

#define DECL(v)	\
  do { \
    const int size = ARRAY_LEN(v##_times); \
    v = *new Variable(v##_times, v##_data, ARRAY_LEN(v##_data) / size, size); \
    IFDBG(Variables::define(#v, &v)); \
  } while (0)

#endif
#endif