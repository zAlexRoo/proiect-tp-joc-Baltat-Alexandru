# 🎮 Infinite Tic-Tac-Toe

Acest proiect reprezintă o reimaginare modernă și dinamică a clasicului joc X și O (Tic-Tac-Toe). A fost dezvoltat în limbajul **C**, folosind biblioteca grafică **Raylib** pentru interfață vizuală, animații și efecte sonore.

Jocul elimină complet problema clasică a meciurilor terminate la egalitate prin introducerea unui mod special numit **Infinite Mode** și oferă posibilitatea de a juca pe table de dimensiuni personalizate.

---

## 🌟 Funcționalități Principale

* **Grid Dinamic:** Utilizatorul poate alege dimensiunea tablei de joc, de la clasicul `3x3`, până la `4x4`, `5x5` sau `6x6`.
* **Modul Infinit (Mecanică Specială):** Pe tablă pot exista doar un număr limitat de piese per jucător (egal cu dimensiunea tablei). Când plasezi o piesă nouă peste limită, cea mai veche piesă a ta de pe tablă dispare! Piesa care urmează să dispară va clipi intermitent pentru a te avertiza.
* **Inteligență Artificială (AI):** Joacă împotriva calculatorului cu 3 nivele de dificultate:
    * 🟢 *EASY:* Mutări complet aleatorii.
    * 🟡 *MEDIUM:* Combinație de mutări inteligente și aleatorii.
    * 🔴 *HARD:* Folosește algoritmul Minimax (pe 3x3) și o euristică avansată de prioritizare pentru tablele mari, fiind practic imposibil de învins.
* **Efecte Vizuale și Sonore:** Animații fluide la plasarea pieselor, trasarea liniei de victorie și efecte audio pentru fiecare acțiune (click, victorie, înfrângere).

---

## 🛠️ Cum se instalează jocul (Pas cu Pas)

Acest joc a fost construit pentru medii bazate pe Linux (ex: Zorin OS, Ubuntu, Debian). Pentru a rula jocul, trebuie să instalezi compilatorul pentru C și biblioteca grafică Raylib.

### Pasul 1: Instalarea uneltelor de bază
Deschide un terminal (combinația de taste `Ctrl + Alt + T`) și rulează următoarea comandă pentru a instala compilatorul `gcc` și utilitarul `make`:
```bash
sudo apt update
sudo apt install build-essential git -y


Pasul 2: Instalarea bibliotecii Raylib
Jocul are nevoie de librăriile grafice și audio pentru a funcționa. În același terminal, copiază și rulează comanda:

sudo apt install libraylib-dev -y

Pasul 3: Descărcarea jocului
Acum trebuie să descarci fișierele jocului pe calculatorul tău. Scrie în terminal:

git clone [https://github.com/zAlexRoo/proiect-tp-joc-Baltat-Alexandru.git](https://github.com/zAlexRoo/proiect-tp-joc-Baltat-Alexandru.git)

(Notă: Asigură-te că înlocuiești numele-repo-ului cu numele exact al repository-ului tău de pe GitHub).

Pasul 4: Pornirea jocului
Intră în folderul jocului descărcat și pornește-l folosind comenzile de compilare automată:

cd proiect-tp-joc-Baltat-Alexandru
make rebuild
make run



📂 Structura Fișierelor (Ce face fiecare fișier?)
Proiectul este împărțit în module clare pentru o bună organizare a codului:

🧠 main.c - „Creierul” interfeței. Gestionează ferestrele, meniurile, animațiile, sunetele și input-ul de la mouse.

📋 board.c / board.h - Conțin logica de bază a tablei de joc (inițializarea tablei, plasarea pieselor, verificarea victoriei pe orizontală, verticală și diagonale).

🤖 ai.c / ai.h - Conțin logica Inteligenței Artificiale, inclusiv algoritmul Minimax și sistemul euristic pentru tablele mari (4x4 - 6x6).

⚙️ Makefile - Un script de automatizare care îi spune calculatorului cum să asambleze (compileze) toate fișierele .c de mai sus într-un singur joc funcțional, fără să scrii comenzi lungi manual.

🎵 Fișierele .wav (click.wav, victory.wav, lose.wav) - Sunetele folosite în joc.



🎮 Cum se joacă Modul Infinit?
Regulile sunt aceleași ca la X și O clasic (trebuie să faci o linie completă), dar cu o mică capcană:

Să zicem că joci pe tabla de 3x3. Ai voie să ai maxim 3 piese pe tablă în același timp.

Ai pus 3 piese, dar nu ai câștigat încă.

Când o pui pe a 4-a, prima piesă pe care ai pus-o la începutul meciului va dispărea de pe tablă!

⚠️ Atenție: Fii mereu atent la piesele semi-transparente (care clipesc) – ele sunt următoarele care vor dispărea la mutarea ta! Gândește cu un pas înainte!