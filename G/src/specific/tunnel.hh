#ifndef		TUNNEL_HH
# define	TUNNEL_HH

#include "node.hh"

namespace Tunnel
{
  void generateMeshes();
  Node * create(date startDate, date endDate);
}

#endif		// TUNNEL_HH
