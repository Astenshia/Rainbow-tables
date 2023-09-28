# AP-project
***REMOVED*** ***REMOVED***
***REMOVED*** ***REMOVED***

1. "Les mots de passe sont composés de M lettres minuscules" donc on a 26^M possibilités pour l'antécédent d'un hash donné.
on est en O(26^M). On est en compléxité exponentielle.

2. Une table de hachage correspond à cette situation : associations de clés/valeurs. On aurait donc 26^M cases dans la table de hachage (une case par hash possible). Tableau => Accès direct donc complexité en θ(1). Pour avoir un remplissage optimal on veut un facteur de charge environ égal à 0.7. Pour avoir α = 0.7, il faut que la taille de notre table soit égale à ceil(N/0.7).

TODO : trouver la taille de la BDD

3. 
1 possibilité par char, 1 octet par char
M = 1 : 26 possibilités = 26 octets
M = 2 : 26*26 ≈ 10²
M = 3 : 26³ ≈ 10⁵
M = 4 : 26⁴ ≈ 10⁶
M = 5 : 26⁵ ≈ 10⁷
M = 6 : 26⁶ ≈ 10⁸ 
M = 7 : 26⁷ ≈ 10⁹ ≈ de l'ordre du giga de RAM
Il serait donc judicieux de s'arrêter à M = 7
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
Reprenons le schéma de l'énoncé : si le candidat recherché est le dernier de la chaîne (pass(x,L)), alors il n'est pas l'antécédent d'un hash.

5. 
On peut utiliser une table hachage (complexité en O(1+n/M) pour la recherche d'un élément): association de clés/valeurs (passx,L/passx,0). On calcule passx,L (la clé) à partir d'un hash donné et on recherche la valeur passx,0 dans la table.

6. 
Si on a 2 pass (passx,i et passy,j) identiques, on a deux cas p^ossibles :
    - i = j : Dans ce cas, passx,L = passy,L (par déterminisme des fonctions de hachage et de réduction).
        Dans ce cas, il est possible d'avoir deux antécédents différents avec un pass "final" indentique.
    - i != j : Dans ce cas, passx,L != passy,L, MAIS il existe passx,k avec k dans [i, L[ tel que passx,k = passy,L OU il existe passy,m avec m dans [j, L[ tel que passy,m = passx,L (En gros, l'un des mots de passes est plus proche de la fin de la chaine que l'autre, donc on perd une fin de chaine)
        Dans ce cas, lorsqu'on compare passy,m, passx,k respectivement à passx,L et passy,L, on peut croire qu'on est arrivé "en fin" d'une autre chaine que celle actuelle, menant donc à un mauvais antécédent.

TODO : faire schéma explicatif avec terme "passe à la trappe"

7. 
C'est moins problématique lorsqu'on a une fonction de réduction qui varie car le pass du hash de passx,i est différent du pass du hash de passy,j si i et j sont différents. 

8. 
Dans ce cas, par déterminisme des fonctions de réductions (étant au même rang, la fonction de réduction est la même pour les deux chaines) et de hachage, le dernier élément de chaque chaîne est le même.
- Dans le cas où il est possible d'avoir plusieurs candidats, c'est relativement peu problématique, car on trouve forcément deux antécédents pour un même dernier élément, qui peuvent être égaux ou non : on a donc au maximum deux antécédents différents, mais aucun antécédent ne "passe à la trappe" comme expliqué à la Q6.
- Dans le cas où l'on souhaite n'avoir qu'un seul candidat, c'est problématique.

9. 
On pose M = ceil(N/0.7) la taille de notre table
Proba de collision = 1 - proba d'absence de collision = 1 - M!/((M^N)*(M-N)!)