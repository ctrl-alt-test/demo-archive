//
// Tous les VBOs utilisés
//

#ifndef		VBOS_HH
# define	VBOS_HH

#include "loading.hh"
#include "vbodata.hh"

namespace Mesh { struct MeshStruct; }

namespace VBO
{
#if DEBUG
  typedef unsigned char id;
  extern int numberOfVBOs;
#else
# include "../data/exported-vboid.hh"
#endif

  extern Element * list; // TODO: a virer ?
  extern int elementSize[];
  void setupData(id vboId, const Mesh::MeshStruct & mesh
		 DBGARG(char * description));
  void setupData(id vboId, const Array <vertex> & chunk
		 DBGARG(char * description));
  void loadVBOs();

#if DEBUG
  void exportIds();
  id newVBO(const char *name);
  int getVBO(const char *name);
  id forceGetVBO(const char *name);
  id newVBOs(const char* name, int nb);
#endif
}

#if DEBUG
# define SETUP_VBO(id, mesh) VBO::setupData(VBO::newVBO(#id), mesh, #id)
# define NEW_VBO(id) VBO::newVBO(#id)
# define VBO_(id_) ((VBO::id) (VBO::forceGetVBO(#id_)))
# define NEW_VBOS(id_, nb) VBO::newVBOs(#id_, nb)
#else
# define SETUP_VBO(id, mesh) VBO::setupData(VBO::id, mesh)
# define NEW_VBO(id) VBO::id
# define VBO_(id) VBO::id
# define NEW_VBOS(id_, nb) VBO::id_
#endif

#endif		// VBOS_HH
