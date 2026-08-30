#include <LiquidCrystal_I2C.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Bouton.h"

LiquidCrystal_I2C lcd(0x27, 20, 4);

// Arduino Nano 33 BLE : les 19 touches physiques du circuit imprime.
// D13 n'est relie a aucune touche. A4/SDA et A5/SCL sont reserves au LCD.
const uint8_t BROCHES_TOUCHES[] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
    A0, A1, A2, A3, A6, A7};

const char CARACTERES_TOUCHES[] = {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    '+', '-', '*', 'R', 'C', '=', '.', '/', 'E'};

const size_t NOMBRE_TOUCHES =
    sizeof(BROCHES_TOUCHES) / sizeof(BROCHES_TOUCHES[0]);
const size_t TAILLE_SAISIE = 19;  // 18 caracteres puis '\0'.

Bouton boutons[NOMBRE_TOUCHES];
char saisie[TAILLE_SAISIE] = "";
char expressionValidee[64] = "";
double accumulateur = 0.0;
char operateurEnAttente = '\0';
bool resultatAffiche = false;

void effacerLigne(uint8_t ligne) {
  lcd.setCursor(0, ligne);
  lcd.print("                    ");
}

void afficherLigne(uint8_t ligne, const char *texte) {
  char texteLimite[21];
  strncpy(texteLimite, texte, 20);
  texteLimite[20] = '\0';
  effacerLigne(ligne);
  lcd.setCursor(0, ligne);
  lcd.print(texteLimite);
}

void viderSaisie() {
  saisie[0] = '\0';
}

bool saisieEstValide() {
  return saisie[0] != '\0' && strcmp(saisie, "-") != 0;
}

void formaterNombre(double nombre, char *destination, size_t taille) {
  if (fabs(nombre) < 0.00000005) {
    nombre = 0.0;
  }

  snprintf(destination, taille, "%.7f", nombre);

  size_t longueur = strlen(destination);
  while (longueur > 0 && destination[longueur - 1] == '0') {
    destination[--longueur] = '\0';
  }
  if (longueur > 0 && destination[longueur - 1] == '.') {
    destination[--longueur] = '\0';
  }
}

void actualiserAffichage() {
  char expression[64];

  if (resultatAffiche && expressionValidee[0] != '\0') {
    strncpy(expression, expressionValidee, sizeof(expression));
    expression[sizeof(expression) - 1] = '\0';
  } else if (operateurEnAttente != '\0') {
    char premierNombre[32];
    formaterNombre(accumulateur, premierNombre, sizeof(premierNombre));
    if (saisie[0] != '\0') {
      snprintf(expression, sizeof(expression), "%s %c %s",
               premierNombre, operateurEnAttente, saisie);
    } else {
      snprintf(expression, sizeof(expression), "%s %c",
               premierNombre, operateurEnAttente);
    }
  } else {
    strncpy(expression, saisie, sizeof(expression));
    expression[sizeof(expression) - 1] = '\0';
  }

  afficherLigne(0, expression);
  if (resultatAffiche && saisie[0] != '\0') {
    afficherLigne(3, saisie);
  } else {
    effacerLigne(3);
  }
}

void toutEffacer() {
  viderSaisie();
  expressionValidee[0] = '\0';
  accumulateur = 0.0;
  operateurEnAttente = '\0';
  resultatAffiche = false;
  lcd.clear();
}

void afficherErreur(const char *message) {
  afficherLigne(3, message);
  delay(1500);
  toutEffacer();
}

bool effectuerOperation(double premierNombre, char operateur,
                        double secondNombre, double &resultat) {
  switch (operateur) {
    case '+': resultat = premierNombre + secondNombre; return true;
    case '-': resultat = premierNombre - secondNombre; return true;
    case '*': resultat = premierNombre * secondNombre; return true;
    case '/':
      if (secondNombre == 0.0) return false;
      resultat = premierNombre / secondNombre;
      return true;
    default: return false;
  }
}

void ajouterCaractere(char caractere) {
  const size_t longueur = strlen(saisie);
  if (longueur < TAILLE_SAISIE - 1) {
    saisie[longueur] = caractere;
    saisie[longueur + 1] = '\0';
  }
}

void ajouterChiffre(char chiffre) {
  if (resultatAffiche && operateurEnAttente == '\0') toutEffacer();

  if (strcmp(saisie, "0") == 0) {
    saisie[0] = chiffre;
  } else if (strcmp(saisie, "-0") == 0) {
    saisie[1] = chiffre;
  } else {
    ajouterCaractere(chiffre);
  }

  resultatAffiche = false;
  actualiserAffichage();
}

void ajouterPointDecimal() {
  if (resultatAffiche && operateurEnAttente == '\0') toutEffacer();
  if (strchr(saisie, '.') != nullptr) return;

  if (saisie[0] == '\0' || strcmp(saisie, "-") == 0) {
    ajouterCaractere('0');
  }
  ajouterCaractere('.');
  resultatAffiche = false;
  actualiserAffichage();
}

void retourArriere() {
  const size_t longueur = strlen(saisie);

  if (longueur > 0) {
    saisie[longueur - 1] = '\0';
    resultatAffiche = false;
  } else if (operateurEnAttente != '\0') {
    formaterNombre(accumulateur, saisie, sizeof(saisie));
    accumulateur = 0.0;
    operateurEnAttente = '\0';
  }

  actualiserAffichage();
}

void choisirOperateur(char nouvelOperateur) {
  // Un premier appui sur "-" commence un nombre negatif.
  // Un second appui annule le signe.
  if (nouvelOperateur == '-' &&
      (saisie[0] == '\0' || strcmp(saisie, "-") == 0)) {
    if (strcmp(saisie, "-") == 0) {
      viderSaisie();
    } else {
      ajouterCaractere('-');
    }
    resultatAffiche = false;
    actualiserAffichage();
    return;
  }

  // Un signe "-" seul ne constitue pas encore un nombre.
  if (strcmp(saisie, "-") == 0) return;

  if (saisie[0] == '\0') {
    if (operateurEnAttente != '\0') {
      operateurEnAttente = nouvelOperateur;
      actualiserAffichage();
    }
    return;
  }

  const double nombreSaisi = strtod(saisie, nullptr);
  if (operateurEnAttente == '\0') {
    accumulateur = nombreSaisi;
  } else {
    double resultat;
    if (!effectuerOperation(accumulateur, operateurEnAttente,
                            nombreSaisi, resultat)) {
      afficherErreur("DIVISION PAR ZERO");
      return;
    }
    accumulateur = resultat;
  }

  operateurEnAttente = nouvelOperateur;
  viderSaisie();
  resultatAffiche = false;
  actualiserAffichage();
}

void calculerResultat() {
  if (operateurEnAttente == '\0' || !saisieEstValide()) return;

  char premierNombre[32];
  formaterNombre(accumulateur, premierNombre, sizeof(premierNombre));
  snprintf(expressionValidee, sizeof(expressionValidee), "%s %c %s =",
           premierNombre, operateurEnAttente, saisie);

  double resultat;
  if (!effectuerOperation(accumulateur, operateurEnAttente,
                          strtod(saisie, nullptr), resultat)) {
    afficherErreur("DIVISION PAR ZERO");
    return;
  }

  formaterNombre(resultat, saisie, sizeof(saisie));
  accumulateur = resultat;
  operateurEnAttente = '\0';
  resultatAffiche = true;
  actualiserAffichage();
}

void calculerRacineCarree() {
  if (!saisieEstValide()) return;

  const double nombre = strtod(saisie, nullptr);
  if (nombre < 0.0) {
    afficherErreur("RACINE IMPOSSIBLE");
    return;
  }

  snprintf(expressionValidee, sizeof(expressionValidee), "RC : %s =", saisie);
  formaterNombre(sqrt(nombre), saisie, sizeof(saisie));
  resultatAffiche = true;
  actualiserAffichage();
}

void traiterTouche(char touche) {
  if (touche >= '0' && touche <= '9') {
    ajouterChiffre(touche);
    return;
  }

  switch (touche) {
    case '.': ajouterPointDecimal(); break;
    case '+':
    case '-':
    case '*':
    case '/': choisirOperateur(touche); break;
    case '=': calculerResultat(); break;
    case 'R': calculerRacineCarree(); break;
    case 'E': retourArriere(); break;
    case 'C': toutEffacer(); break;
  }
}

void setup() {
  lcd.init();
  lcd.backlight();
  lcd.print("By Philippe COSTES");
  lcd.setCursor(0, 1);
  lcd.print("Version 4.0");

  for (size_t i = 0; i < NOMBRE_TOUCHES; ++i) {
    boutons[i].begin(BROCHES_TOUCHES[i]);
  }

  delay(1000);
  lcd.clear();
}

void loop() {
  for (size_t i = 0; i < NOMBRE_TOUCHES; ++i) {
    if (boutons[i].nouvelAppui()) {
      traiterTouche(CARACTERES_TOUCHES[i]);
    }
  }
}
