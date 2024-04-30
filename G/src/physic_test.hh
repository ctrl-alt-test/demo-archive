#ifndef PHYSIC_TEST_HH_
# define PHYSIC_TEST_HH_
# include "basicTypes.hh"

class Rand;

void phy_draw();

void phy_init();
void phy_update(date t);
void phy_shoot(Rand & rand);

#endif /* !PHYSIC_TEST_HH_ */
