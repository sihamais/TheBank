<center>
<h3>AISSAOUI Siham</h3>
<h1><b>RAPPORT DE PROJET</b></h1>
</center>

## I. Wireframes
### 1. Vue générale

Voici comment je choisis de représenter la vue globale de l'application :
![Vue globale](Vue-globale.png)
La vue globale se compose de trois diagrammes représentants les statistiques de chaque canal. Les 3 diagrammes ne sont pas individuels, ils sont regroupés en un diagramme générale avec un seul axe abscisse et ordonnée, ce qui permet de mieux comparer les données dans les différents canaux. Les canaux sont distingués par leurs noms qui se place en dessous du canal ainsi que d'un espace vide. Au dessus des barres se trouvent des étiquettes correspondants à la famille que représente la barre. L'étiquette est mise au dessus de la barre pour éviter le chevauchement éventuel des étiquettes si elles étaient en dessous. Si deux barres adjacentes ont à peu près la même valeur, un espace entre les barres est prévu pour éviter la collision de deux étiquettes. Chaque diagramme contient autant de barre que de famille dans le canal. 

Un menu est à disposition pour modifier des paramêtres d'affichage :
* Une liste de checkbox à choix multiples contenant toutes les agences : L'agence sera préselectionnée en fonction de l'utilisateur. Si l'utilisateur est un directeur d'agence, alors la valeur par défaut sera sont agence. Si c'est le directeur de la banque, alors toutes les agences seront séléctionnées.
* Un date picker intelligent : date cliquable qui affiche un mini calendrier pour choisir une date de début et de fin. Lors de son choix, l'utilisateur peut ensuite avancer ou reculer de la même durée choisie sur le calendrier grâce aux flèches.
* Une liste de checkbox à choix unique permettant d'afficher les données des trois diagrammes en fonction d'un insight. Je choisis d'appliquer ce paramètre aux trois diagrammes en même temps pour des raisons de pratique.

Je choisis également de permettre à l'utilisateur de choisir entre deux modes visuels :
* Mode **clair** (light mode)
* Mode **sombre** (dark mode)

Voici le wireframe commenté avec tous les détails évoqués plus haut :
![Vue globale commentée](Vue-globale-commentée.png)


## II. Difficultés rencontrées
