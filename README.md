# TP_Sys_acqui_commande
TP de systèmes d'acquisitions et commandes

## 1) création du shell

Le shell fonctionne de la sorte:

  - le STM envoie tout d'abbord __RESET__ si on appuis sur reset
  - on initialise le buffer rx_buffer à 0
  - on rentre dans une boucle while qui demande un caractère
  - lorsqu'on envoie un caractere en UART, on sort de la boucle puisque le buffer n'est plus egal à 0
  -
