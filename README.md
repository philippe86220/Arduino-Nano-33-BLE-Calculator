# Calculatrice Arduino Nano 33 BLE sur circuit imprimé

Ce projet présente une calculatrice réalisée autour d'une **Arduino Nano 33 BLE**, d'un écran LCD I2C 20 × 4 et de 19 boutons-poussoirs montés sur un circuit imprimé conçu avec EasyEDA puis fabriqué par JLCPCB.

La première version de cette calculatrice a été réalisée en 2021 avec une Arduino Nano AVR et l'aide des membres du forum Arduino francophone, alors que je débutais en programmation et en électronique. Quelques mois plus tard, en septembre 2021, j'en ai développé une nouvelle version autour d'une Nano 33 BLE.

En novembre 2024, j'ai conçu avec EasyEDA le circuit imprimé de la version présentée dans ce dépôt, puis je l'ai fait fabriquer par JLCPCB. Enfin, en août 2026, j'ai repris le projet afin de documenter le matériel, de corriger plusieurs défauts et de réécrire le programme de manière plus claire et plus robuste.

## Fonctionnalités

La calculatrice permet d'effectuer :

- les additions ;
- les soustractions ;
- les multiplications ;
- les divisions ;
- les racines carrées ;
- la saisie de nombres décimaux ;
- la suppression du dernier caractère saisi ;
- l'enchaînement de plusieurs opérations ;
- la remise à zéro complète avec la touche `A/C`.

L'expression validée reste affichée sur la première ligne du LCD et son résultat apparaît sur la dernière ligne.

Exemple :

```text
12.5 * 3 =



37.5
```

## Matériel utilisé

| Élément | Description |
|---|---|
| Microcontrôleur | Arduino Nano 33 BLE, première génération |
| Processeur | Nordic nRF52840, ARM Cortex-M4F 32 bits |
| Logique | 3,3 V |
| Écran | LCD 20 × 4 avec interface I2C |
| Adresse I2C | `0x27` |
| Commandes | 19 boutons-poussoirs |
| Circuit imprimé | Conception personnelle, fabrication JLCPCB |

La Nano 33 BLE utilise un microcontrôleur 32 bits. Le type `double` y est codé sur 64 bits, contrairement à l'ATmega328P des anciennes cartes Arduino UNO et Nano AVR, sur lesquelles `double` et `float` utilisent tous les deux 32 bits.

> **Attention :** les entrées et sorties de la Nano 33 BLE fonctionnent en 3,3 V et ne sont pas tolérantes au 5 V. Dans ce montage, l'écran et son adaptateur I2C sont alimentés par la sortie 3,3 V de la Nano 33 BLE. Le fonctionnement de l'écran LCD 20 × 4 utilisé dans ce projet a été validé sous 3,3 V. Un autre modèle d'écran ou d'adaptateur I2C pourrait cependant nécessiter une vérification de sa tension d'alimentation.

## Environnement logiciel testé

| Élément | Version ou référence |
|---|---|
| Système | Debian 13 |
| Arduino IDE | 1.8.19 |
| Carte | Arduino Nano 33 BLE |
| Paquet de cartes | Arduino Mbed OS Nano Boards 4.6.0 |
| Bibliothèque LCD | `LiquidCrystal_I2C` 1.1.4 |
| Auteur de la bibliothèque | Frank de Brabander |
| Mainteneur | Marco Schwartz |
| Source | [marcoschwartz/LiquidCrystal_I2C](https://github.com/marcoschwartz/LiquidCrystal_I2C) |
| Architecture déclarée | `avr` |

La Nano 33 BLE est une carte Arduino officielle. Aucune URL supplémentaire ne doit être ajoutée dans les préférences de l'IDE.

Pour installer son support :

```text
Outils
→ Type de carte
→ Gestionnaire de cartes…
```

Rechercher puis installer :

```text
Arduino Mbed OS Nano Boards
```

Sélectionner ensuite :

```text
Outils
→ Type de carte
→ Arduino Mbed OS Nano Boards
→ Arduino Nano 33 BLE
```

La bibliothèque de l'écran doit être téléchargée depuis le dépôt
[marcoschwartz/LiquidCrystal_I2C](https://github.com/marcoschwartz/LiquidCrystal_I2C).
Après avoir téléchargé l'archive ZIP depuis GitHub, l'installer dans Arduino IDE avec :

```text
Croquis
→ Inclure une bibliothèque
→ Ajouter la bibliothèque .ZIP…
```

La bibliothèque utilisée se déclare compatible avec l'architecture AVR uniquement. Elle compile néanmoins avec le cœur `mbed_nano`, mais produit des avertissements concernant son architecture et les anciennes notations binaires `B00000001` et `B00000100`. Ces avertissements n'empêchent ni la compilation ni le fonctionnement de l'écran dans la configuration testée. Une bibliothèque LCD plus récente et officiellement compatible avec plusieurs architectures pourra la remplacer ultérieurement.

## Organisation du programme

Les trois fichiers suivants doivent être placés dans un même dossier nommé `calculatrice_optimisee` :

```text
calculatrice_optimisee/
├── calculatrice_optimisee.ino
├── Bouton.h
└── Bouton.cpp
```

### `calculatrice_optimisee.ino`

Ce fichier contient :

- l'association entre les broches et les touches ;
- la saisie des nombres sous forme de C-string ;
- les quatre opérations arithmétiques ;
- la racine carrée ;
- la gestion de l'affichage ;
- la touche Retour ;
- la remise à zéro de la calculatrice.

### `Bouton.h` et `Bouton.cpp`

Ces fichiers contiennent une classe légère, écrite spécialement pour ce projet, qui :

- configure chaque broche avec `INPUT_PULLUP` ;
- lit l'état physique du bouton ;
- filtre les rebonds pendant 20 ms ;
- signale une seule fois chaque nouvel appui.

Cette implémentation n'effectue aucune allocation dynamique.

## Brochage des touches

Le programme ne déclare que les 19 touches réellement présentes sur le circuit imprimé.

| Broche | Touche | Fonction |
|---|---:|---|
| `D0` | `0` | Chiffre 0 |
| `D1` | `1` | Chiffre 1 |
| `D2` | `2` | Chiffre 2 |
| `D3` | `3` | Chiffre 3 |
| `D4` | `4` | Chiffre 4 |
| `D5` | `5` | Chiffre 5 |
| `D6` | `6` | Chiffre 6 |
| `D7` | `7` | Chiffre 7 |
| `D8` | `8` | Chiffre 8 |
| `D9` | `9` | Chiffre 9 |
| `D10` | `+` | Addition |
| `D11` | `-` | Soustraction |
| `D12` | `*` | Multiplication |
| `A0` | `R` | Racine carrée |
| `A1` | `C` | Remise à zéro `A/C` |
| `A2` | `=` | Calcul du résultat |
| `A3` | `.` | Séparateur décimal |
| `A6` | `/` | Division |
| `A7` | `E` | Retour arrière |

Les autres broches ont les rôles suivants :

- `D13` n'est reliée à aucune touche ;
- `A4/SDA` est utilisée par l'écran I2C ;
- `A5/SCL` est utilisée par l'écran I2C.

Les anciennes versions utilisaient trois entrées fictives nommées `X`, `Y` et `Z` pour conserver l'alignement de deux tableaux. Elles ont été supprimées de la version réécrite.

## Gestion de la saisie

La saisie est conservée dans un tableau de caractères de taille fixe :

```cpp
const size_t TAILLE_SAISIE = 19;
char saisie[TAILLE_SAISIE] = "";
```

Ce tableau peut contenir 18 caractères utiles, suivis obligatoirement du caractère de fin de chaîne `\0`.

Le programme n'utilise pas la classe Arduino `String` pour la saisie. Il évite ainsi les allocations dynamiques répétées et les risques de fragmentation de la mémoire associés à leur utilisation prolongée.

Les principales fonctions utilisées sont :

| Fonction | Rôle |
|---|---|
| `strlen()` | connaître la longueur de la saisie |
| `strchr()` | rechercher un point déjà présent |
| `strcmp()` | comparer deux C-strings |
| `strtod()` | convertir la saisie en `double` |
| `snprintf()` | convertir et composer du texte avec une taille limitée |
| `strncpy()` | copier une C-string en limitant le nombre de caractères |

## Workflow d'une opération : `12.5 * 3`

### Touche `1`

`ajouterChiffre('1')` appelle `ajouterCaractere('1')`.

```text
saisie = "1"
```

### Touche `2`

`ajouterChiffre('2')` appelle `ajouterCaractere('2')`.

```text
saisie = "12"
```

### Touche `.`

`ajouterPointDecimal()` :

- `strchr(saisie, '.')` renvoie `nullptr`, car aucun point n'est encore présent ;
- `saisie[0]` n'est pas `\0`, il n'est donc pas nécessaire d'ajouter un zéro ;
- `ajouterCaractere('.')` ajoute le point.

```text
saisie = "12."
```

### Touche `5`

`ajouterChiffre('5')` ajoute le chiffre à la C-string.

```text
saisie = "12.5"
```

### Touche `*`

`choisirOperateur('*')` :

- convertit `"12.5"` en `double` avec `strtod()` ;
- place la valeur `12.5` dans `accumulateur` ;
- place `'*'` dans `operateurEnAttente` ;
- vide la C-string `saisie` pour recevoir le second nombre.

```text
accumulateur       = 12.5
operateurEnAttente = '*'
saisie             = ""
```

Affichage :

```text
12.5 *
```

### Touche `3`

`ajouterChiffre('3')` place le second nombre dans `saisie`.

```text
saisie = "3"
```

Affichage :

```text
12.5 * 3
```

### Touche `=`

`calculerResultat()` :

- mémorise `"12.5 * 3 ="` dans `expressionValidee` ;
- convertit `"3"` en `double` avec `strtod()` ;
- appelle `effectuerOperation(12.5, '*', 3.0, resultat)` ;
- obtient `37.5` ;
- reconvertit le résultat en C-string avec `formaterNombre()` ;
- place cette C-string dans `saisie` pour l'affichage.

```text
expressionValidee  = "12.5 * 3 ="
saisie             = "37.5"
accumulateur       = 37.5
operateurEnAttente = '\0'
resultatAffiche    = true
```

Résultat final :

```text
Ligne 0 : 12.5 * 3 =
Ligne 3 : 37.5
```

`strtod()` permet donc de convertir directement une C-string décimale en `double`, sans construire manuellement sa partie décimale. Après le calcul, `formaterNombre()` effectue la conversion dans l'autre sens, limite l'affichage à sept chiffres après le point et supprime les zéros finaux devenus inutiles.

## Fonctionnement de la touche Retour

La version de 2024 transformait immédiatement chaque saisie en valeur numérique. Elle ne pouvait donc plus distinguer les trois saisies suivantes :

```text
12
12.
12.0
```

Ces trois écritures correspondent à la même valeur numérique, mais pas au même texte saisi.

Dans la version réécrite, la touche Retour supprime directement le dernier caractère de la C-string :

```cpp
saisie[longueur - 1] = '\0';
```

Le comportement devient prévisible :

```text
12.3 → 12. → 12 → 1 → saisie vide
```

## Enchaînement des opérations

La calculatrice exécute les opérations dans l'ordre des appuis. Elle ne mémorise pas une expression mathématique complète et n'applique pas de priorité entre multiplication, division, addition et soustraction.

Par exemple, la suite de touches :

```text
2 → + → 3 → * → 4 → =
```

est traitée ainsi :

```text
2 + 3
↓ appui sur *
5 * 4
↓ appui sur =
20
```

L'expression `2 + 3 * 4` n'est jamais conservée ni affichée en entier.

## Formatage des résultats

La fonction `formaterNombre()` utilise :

```cpp
snprintf(destination, taille, "%.7f", nombre);
```

Elle affiche au maximum sept chiffres après le point, puis supprime les zéros finaux et le point devenu inutile.

Exemples :

```text
37.5000000 → 37.5
15.0000000 → 15
0.2500000  → 0.25
```

Cette limitation concerne l'affichage. Les calculs intermédiaires restent effectués avec le type `double`.

## Gestion des erreurs

### Division par zéro

Avant une division, le programme vérifie si le second nombre vaut zéro. Dans ce cas, il affiche :

```text
DIVISION PAR ZERO
```

La division n'est pas exécutée.

### Racine carrée négative

Si le nombre saisi est négatif, le programme affiche :

```text
RACINE IMPOSSIBLE
```

## Historique

### Version 3.4 - programme précédemment présent sur la calculatrice

Les essais réalisés en août 2026 ont confirmé les comportements suivants :

```text
12.3 + Retour → 12
12.  + Retour → 1
12 + 3 =      → 15
```

La suppression du point ne fonctionnait pas correctement parce que la saisie `12.` était immédiatement convertie en nombre et devenait simplement `12`.

### Version 3.5 - code retrouvé

Cette version améliorait notamment la détection de la division par zéro, mais conservait l'ancienne gestion complexe de la saisie et du retour arrière.

### Version réécrite - août 2026

- remplacement de l'ancienne gestion des boutons ;
- anti-rebond écrit spécialement pour le projet ;
- suppression des trois fausses touches `X`, `Y` et `Z` ;
- remplacement de la construction mathématique des décimales par une C-string ;
- correction du retour arrière sur le point ;
- conservation séparée de l'expression et du résultat ;
- contrôle de la division par zéro ;
- contrôle des racines carrées négatives ;
- suppression de l'utilisation de `String` dans la logique de la calculatrice.

## Origine et droits du code

Le programme personnel de 2024 utilisait une adaptation d'une classe de gestion de boutons publiée sur le forum Arduino sans licence de redistribution clairement identifiée. Son auteur avait été cité et contacté, mais aucune autorisation de republication n'avait été obtenue.

Cette ancienne classe n'est donc pas publiée dans ce dépôt. Les fichiers `Bouton.h` et `Bouton.cpp` de la nouvelle version constituent une implémentation différente et indépendante, écrite spécialement pour cette calculatrice.

Le principe général d'un anti-rebond temporel reste classique : une nouvelle lecture doit demeurer stable pendant une durée déterminée avant d'être acceptée.

## Tests conseillés

```text
12.3 → retours successifs
12.  → retour
0.5 + 0.25 =
12.5 * 3 =
10 / 0 =
0 / 5 =
9 → racine carrée
remplacement d'un opérateur avant le second nombre
enchaînement de plusieurs opérations
A/C pendant une saisie et après un résultat
```

## Schéma électronique

Le schéma complet est disponible ici :

[Consulter le schéma électronique](hardware/Schematic_calculatrice.pdf)

## Auteur

**Philippe COSTES**

- conception du circuit imprimé ;
- assemblage de la calculatrice ;
- développement du programme initial ;
- essais et validation de la version réécrite.

## Assistance à la réécriture

La reprise du projet en août 2026 a été réalisée avec l'assistance de **ChatGPT d'OpenAI** pour :

- analyser le programme historique ;
- identifier les défauts de la gestion du retour arrière ;
- concevoir une nouvelle gestion indépendante de l'anti-rebond ;
- réorganiser la saisie autour de C-strings de taille fixe ;
- améliorer la gestion des opérations, des erreurs et de l'affichage ;
- préparer et structurer la documentation du dépôt.

Les essais sur la calculatrice réelle, les observations de fonctionnement et la validation finale du programme ont été effectués par Philippe COSTES.

Projet commencé en 2021, transposé sur circuit imprimé en novembre 2024 et entièrement repris en août 2026.
