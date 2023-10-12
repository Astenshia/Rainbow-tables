# AP-project
***REMOVED*** ***REMOVED***  
***REMOVED*** ***REMOVED***

1. "Les mots de passe sont composés de M lettres minuscules" donc on a 26^M possibilités pour l'antécédent d'un hash donné.
on est en O(26^M). On est en complexité exponentielle.

2. Une table de hachage correspond à cette situation : associations de clés/valeurs. On aurait donc 26^M valeurs à stocker dans la table de hachage.  
Table de hachage => recherche en O(1+(26^M)/A) avec A le nombre de cases de la table.  
Pour avoir un remplissage optimal on veut un facteur de charge environ égal à 0.7. Pour avoir α = 0.7, il faut que la taille de notre table soit égale à A = ceil(N/0.7) avec N = 26^M.

3. 
1 possibilité par char, 1 octet par char  
M = 1 : 26 possibilités = 26 octets  
M = 2 : 26*26 ≈ 10²  
M = 3 : 26³ ≈ 10⁵  
M = 4 : 26⁴ ≈ 10⁶  
M = 5 : 26⁵ ≈ 10⁷  
M = 6 : 26⁶ ≈ 10⁸   
M = 7 : 26⁷ ≈ 10⁹ ≈ de l'ordre du giga de RAM  
Il serait donc judicieux de s'arrêter à M = 7 au plus. (On préfèrera même prendre M = 6 pour éviter de dépasser plusieurs Go de RAM)
```
Justification mathématique : 
On part du principe que nous avons au maximum 32 Go de RAM pour stocker ce tableau.
32Go = 32*10⁹ octets
Or 10⁹ = 10³*10³*10³ ≈ 2¹⁰*2¹⁰*2¹⁰≈ 32²*32²*32² ≈ 32⁶
Donc 32*10⁹ ≈ 32⁷ 
Et 26⁷ < 32⁷
Donc il est raisonnable de s'arrêter à M = 7
``` 
4. 
La fonction de réduction ne varie pas (encore).

D'après la définition "le candidat est un pass dont le hash est égal au hash attaqué" et non "le candidat est le mot de passe recherché" alors un candidat est UN antécédent du hash attaqué.

Dans le cas où l'on considère que "le candidat est le mot de passe recherché" alors on peut avoir deux mots de passes donnent le même hash, et donc le pass renvoyé n'est pas forcément le mot de passe recherché. Mais le candidat est bien un antécédent du hash donné.

Dans tous les cas c'est UN antécédent du hash recherché.

[INSERER SCHEMA ICI]

5. 
On peut utiliser une table hachage (complexité en O(1+n/M) pour la recherche d'un élément): association de clés/valeurs (passx,L/passx,0). On génère passx,0, on calcule passx,L à partir de passx,0 et on stocke le couple (passx,L, passx,0) dans la table.

6. 
Nous avons traité ce cas dans la question 2 (voir schéma).  
Si on a 2 pass (passx,i et passy,j) identiques, on a deux cas possibles :  

    - i = j : Dans ce cas, passx,L = passy,L (par déterminisme des fonctions de hachage et de réduction).
    Dans ce cas, il est possible d'avoir deux antécédents différents avec un pass "final" indentique.
    Ca veut dire qu'on a deux pass0 qui donnent le même passL : on n'autorisera pas cette situation dans une table lors de l'implémentation.  

    - i != j : Dans ce cas, passx,L != passy,L, MAIS il existe passx,k avec k dans [i, L[ tel que passx,k = passy,L OU il existe passy,m avec m dans [j, L[ tel que passy,m = passx,L (En gros, l'un des mots de passes est plus proche de la fin de la chaine que l'autre, donc on perd une fin de chaine)
        Dans ce cas, lorsqu'on compare passy,m, passx,k respectivement à passx,L et passy,L, on peut croire qu'on est arrivé "en fin" d'une autre chaine que celle actuelle, menant donc à un mauvais antécédent.


7. 
C'est moins problématique lorsqu'on a une fonction de réduction qui varie car le passx,(i+1) du hashx,i de passx,i est différent du passy,(j+1) du hashy,j de passy,j si i et j sont différents. 

8. 
Dans ce cas, par déterminisme des fonctions de réductions (étant au même rang, la fonction de réduction est la même pour les deux chaines) et de hachage, le dernier élément de chaque chaîne est le même.
- Dans le cas où il est possible d'avoir plusieurs candidats, c'est relativement problématique, car on trouve forcément deux antécédents pour un même dernier élément, qui peuvent être égaux ou non : on a donc deux antécédents différents.
- Dans le cas où l'on souhaite n'avoir qu'un seul candidat, c'est problématique.

9. 
On a vu que le seul cas de collision possible survient quand on a deux passx,i passy,i identiques sur la même colonne (au même indice).  
On cherche à savoir à partir de combien d'itérations en moyenne on a cette collision. C'est-à-dire qu'on a une collision à partir d'un certain rang k et donc aucune collision avant (car une collision implique que tous les passx,i avec i > k seront identitiques aux passy, i)  
[SCHEMA ICI]  
On utilise donc une loi géométrique de paramètre p = 1/26^M (proba d'équivalence entre deux mots de passes aléatoires).  
On a donc :  
Prob(X=1) = p(1-p)^(1-1) = p (Donc u0 = p)  
.......  
Prob(X=L) = p(1-p)^(L-1)  

Lorsque l'on fait la somme des termes de cette suite on a :  
Proba(collision) = **somme des termes pour i allant de 0 à L-1 de (p*(1-p)^i)** avec p = 1/26^M  
= p*[(1-(1-p)^L) / 1-(1-p)]  
= 1-(1-p)^L   
= 1-((1-(1/26^M))^L)

ATTENTION: RESULTAT QUI PART DU PRINCIPE QUE ri et h SONT PARFAITES ET NE PRODUISENT PAS DEUX RESULTATS IDENTIQUES POUR DEUX ENTREES DIFFERENTES!
IL FAUDRAIT MULTIPLIER NOTRE RESULTAT PAR UN FACTEUR DE FIABILITE DE h ET DE r.

10. 
Si on utilise la question précédente, on a une proba de collision proche de 0 quelques soient les valeurs de M et de L (et donc en théorie, on n'a pas de collision donc on peut insérer 26^M valeurs dans une table d'au moins 26^M cases sans difficulté).  
En revanche, en ajoutant un coefficient d'erreur pour les fonctions de hash et de réduction, on est vite limités si on a pas pas plus de cases dns la table que de mots à stocker.  

De manière empirque, on observe que l'on peut stocker 10000 mots dans 100000 cases sans avoir beaucoup de collisions, soit un facteur 10 de N : si on a 10*N cases pour stocker N valeurs, on arrive à insérer les N valeurs sans avoir à gérer beaucoup de collisions sur les dernières insertions.  

Dans l'idéal, on cherche à avoir un coeff nbDeValeursAStocker/TailleDeLaTable = α = 0.7 => TailleDeLaTable = nbDeValeursAStocker/0.7 = nbDeValeursAStocker x 1.4286.  
Donc pour 10 000 valeurs à stocker, une table de 14 286  cases serait satisfaisante. Dans ce cas on serait en O(n).  

11.  
[SCREEN DES RESULTATS]  

12.  

Environ 86% de hash ont donné un antécédent.  
Nous avons utilisé une unique hashtable pour stocker les hash à attaquer, et avons géré les collisions avec des listes chainées.  
Afin de réduire le temps d'exécution de notre programme, nous l'avons réparti en utilisant des threads. 

13.  [TO DO]
