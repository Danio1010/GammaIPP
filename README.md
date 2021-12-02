# GammaIPP
Individual Programming Project - Gamma game
Use following commands to run it:
mkdir release
cd release
cmake ..
make
./gamma
Then type: I [width_of_board] [height_of_board] [num_of_players] [num_of_areas]

# Game description:
Tegoroczne duże zadanie polega na zaimplementowaniu gry gamma. Grę rozgrywa się na prostokątnej planszy, która składa się z jednakowych kwadratowych pól. Pola sąsiadujące na planszy to takie, które stykają się bokami. Pól stykających się tylko rogami nie uważamy za sąsiadujące. Pola tworzą obszar, jeśli z każdego z nich można dojść do innego, przechodząc jedynie przez pola sąsiadujące. Pojedyncze pole też jest obszarem. Grać może jedna lub więcej osób. Na początku gry plansza jest pusta. Gracze kolejno zajmują po jednym polu, stawiając na nim swój pionek. Gracz może zająć dowolne niezajęte pole, przestrzegając jedynie zasady, że zbiór pól zajętych przez tego samego gracza nie może w żadnej fazie gry składać się z więcej niż, będącej parametrem gry, maksymalnej liczby obszarów. Każdy gracz jeden raz w całej rozgrywce może wykonać złoty ruch, który polega na zabraniu z planszy pionka innego gracza i postawieniu w jego miejsce swojego pionka, ale ten ruch nadal nie może naruszać zasady maksymalnej liczby zajętych obszarów przez żadnego z graczy. Gracz, który nie jest w stanie wykonać ruchu zgodnego z powyższą zasadą, wypada z gry, ale może wrócić do gry po jakimś złotym ruchu innego gracza. Gra kończy się, gdy już żaden gracz nie może wykonać ruchu. Wygrywa gracz, który zajmie największą liczbę pól.

In short: adjecent fields owned by the same player create an area, every player can have a maximum of [num_of_areas] areas. When game starts board is empty, player can put his pawn on empty fields IF it doesn't increase number of his areas above [num_of_areas]. Every player can make a "golden" move, it replaces a pawn of player occupying the field with a pawn of player making the move, it only can be made IF it doesn't increase number of areas of either of players above [num_of_areas].
Controls: arrows - movement, space - placing pawn, g - golden move.
