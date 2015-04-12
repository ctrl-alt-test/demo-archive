//
// Définition des temps absolus en ms pour le déroulement
//

#include "timing.hh"


// Usine
const date factoryStartDate = 0;
const date sphereStartDate = factoryStartDate + 41000;
const date sphereAlertDate = sphereStartDate;
const date sphereEndAlertDate = sphereStartDate + 21000;

const date zeusStartDate = sphereAlertDate + 6000;

const date factoryEndDate = factoryStartDate + 104500;

// Ville
const date cityStartDate = factoryEndDate;
const date tetrisDate = cityStartDate + 14400;
const date cityGrowingBuildingDate = cityStartDate + 57000;
const date cityEndCarsDate = cityStartDate + 64700;
const date cityEndDate = cityEndCarsDate + 13000;

// Pong
const date pongStartDate = cityEndCarsDate;
const date pongEndDate = cityEndDate;

/*
// Conway
const date starfieldStartDate = cityEndDate;
const date starfieldEndDate = starfieldStartDate + 20500;

// Rampe
const date rampStartDate = starfieldEndDate + 2000;
const date rampEndDate = rampStartDate + 38850 - 2000; // transition

// Sous bois
const date forestStartDate = rampEndDate;
const date forestEndDate = forestStartDate + 50850;
*/

// Rampe
const date rampTransitionStartDate = cityEndDate;
const date rampTransitionEndDate = cityEndDate + 2000;
const date rampStartDate = rampTransitionEndDate;;
const date rampEndDate = rampStartDate + 38850 - 2000; // transition

// Conway
const date starfieldStartDate = rampEndDate;
const date starfieldEndDate = starfieldStartDate + 20000;

// Sous bois
const date forestStartDate = starfieldEndDate;
const date forestEndDate = forestStartDate + 27000;


// Élevation
const date elevationStartDate = forestEndDate;
const date elevationEndDate = elevationStartDate + 24000;

// Écran
const date screenStartDate = elevationEndDate;
const date screenEndDate = screenStartDate + 54000;


// Fin
const date demoEndDate = screenEndDate;
