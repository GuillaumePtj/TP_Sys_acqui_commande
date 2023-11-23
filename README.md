# TP_Sys_acqui_commande
TP de systèmes d'acquisitions et commandes

## 1) création du shell

Le shell fonctionne de la sorte:

  - le STM envoie tout d'abbord __RESET__ si on appuis sur reset
  - on initialise le buffer rx_buffer à 0
  - on rentre dans une boucle while qui demande un caractère
  - lorsqu'on envoie un caractere en UART, on sort de la boucle puisque le buffer n'est plus egal à 0
  -
  
## 2) Réalisation de la commande complémentaire décalée

La commande des 4 transistors du hacheur en commande complémentaire décalée se fait avec un compteur qui compte et décompte de 0 à 1023.
De plus, nous introduisons un temps mort à chaque commutation de transistors afin de ne pas avoir deux MOSFETs d'un même bras de pont conducteur en même temps.

## 3) Acquisition de courant et de vitesse 

- Mesure du courant :
Nous cherchons à mesurer le courant dans une des phases du moteur, sachant qu'il est l'opposé dans la deuxième phase.
Pour une bonne utilisation du capteur, nous l'initialisons dans la fonction "Current_Init".
Puis nous appliquons la fonction de transfert du capteur : I = (1/Sn)(3300*Vm/4095 - V0) afin de pouvoir exploiter la valeur du capteur.

- Mesure de la vitesse :
Pour mesurer la vitesse de rotation du moteur, nous mesurons le nombre de fronts montants de l'encodeur rotatif monté sur la maquette pendant 10ms.
Ainsi, nous sommes en mesure de mesurer la fréquence du signal provenant de l'encodeur.

## 4) Asservissement en temps réel
