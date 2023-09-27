# AP-project
***REMOVED*** ***REMOVED***
***REMOVED*** ***REMOVED***

1. "Les mots de passe sont composés de M lettres minuscules" donc on a 26^M possibilités pour l'antécédent d'un hash donné.
on est en O(26^M). On est en compléxité exponentielle.

2. Une table de hachage correspond à cette situation : associations de clés/valeurs. On aurait donc 26^M cases dans la table de hachage (une case par hash possible). Tableau => Accès direct donc complexité en θ(1).

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



