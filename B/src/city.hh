#ifndef CITY_HH_
# define CITY_HH_

#include "basicTypes.hh"
#include "renderlist.hh"

class City
{
public:
  City(int w, int h);
  ~City();

  void generate(RenderList & renderList);

  void generateMeshes();
  const Array<Array<VBO::vertex> *> & buildingsChunks() { return buildingsChunks_; }
  const Array<VBO::vertex> & groundChunk() { return groundChunk_; }
  const Array<VBO::vertex> & pavementChunk() { return pavementChunk_; }
  const Array<VBO::vertex> & streetHChunk() { return streetHChunk_; }
  const Array<VBO::vertex> & streetVChunk() { return streetVChunk_; }
  const Array<VBO::vertex> & streetOChunk() { return streetOChunk_; }
  const Array<VBO::vertex> & lightChunk() { return lightChunk_; }

private:
  void roadToFactoryMesh();
  void streetLightsMesh();
  void buildingElementsMeshes();

  void addCity(RenderList & renderList);
  void addCar(RenderList & renderList, int x, bool dir);
  void addHouse(int coef_x, int coef_y, int x, int y);
  void addCustomHouses(RenderList & renderList);
  void addRails(RenderList & renderList);
  void addTrain(RenderList & renderList);
  void addRailsCase(RenderList & renderList, float* pos, float* old, int n);
  void addFactory(RenderList & renderList);
  void addStatues(RenderList & renderList);
  void addStreetLights(RenderList & renderList);

  void fill();
  void prune();
  void random_walk(int x, int y);
  void computeHeightMap(void);

  int width;
  int height;
  char **data;
  float **hmap;
  Array<RenderList *> renderLists_;
  Array<Array<VBO::vertex> *> buildingsChunks_;
  Array<VBO::vertex> groundChunk_;
  Array<VBO::vertex> pavementChunk_; // trottoir
  Array<VBO::vertex> streetHChunk_; // horizontal road
  Array<VBO::vertex> streetVChunk_; // vertical road
  Array<VBO::vertex> streetOChunk_; // other - bend
  Array<VBO::vertex> lightChunk_; // lampadaires
};

float railsDist(float p1[3], float p2[3]);

#define rails_points 6
extern float rails_data[rails_points*4];

#endif
