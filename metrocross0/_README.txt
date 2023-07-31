
 Metro-Cross, (c) Namco 1985
 2013 version by Cl�ment CORDE / c1702@yahoo.com

 All graphical content (c) Namco, rips by myself.
 Using ST-Sound by Leonard/OXG (http://leonard.oxg.free.fr/), and ST music.



--- *FR*

> Pr�ambule :

 Il s'agit d'un petit portage de Metro-Cross, jeu que j'avais beaucoup aim� � l'�poque sur le ST. Il y a plein de choses dont je ne suis pas content dans ce projet, mais en jouant je retrouve du fun de l'original.



> OpenGL :

 Pour ce jeu, j'ai du utiliser OpenGL car le tearing en SDL pur est insupportable. Avec un scrolling lent, �a passe assez inaper�u, mais l� non.

 Probl�me, je suis un noob total en OpenGL.

 Le timing du jeu est bas� sur la V-Sync. Il faut donc que cette derni�re soit activ�e, sinon �a risque d'aller tr�s tr�s vite. Sous les deux configs Linux que j'ai test�es, la V-Sync �tait activ�e par d�faut. Avec mon vieux XP, la carte NVidia propose un panneau de configuration. Pour les autres je ne sais pas...

 Il faut aussi que la fr�quence de rafra�chissement soit 60 Hz.

 C'est aussi pour avoir la V-Sync que le jeu ne tourne qu'en full screen. (En mode fen�tr�, il n'y a pas de V-Sync).

 Au cas o�, il est quand m�me possible de lancer une version SDL pure en passant '/noGL' en param�tre au programme. Mais ce n'est vraiment pas beau.



> Contr�les :

- Haut, bas, droite, gauche : D�placement.
- Bouton A : Saut.

 Les touches par d�faut sont : Fl�ches de direction et Ctrl gauche pour sauter.
 Vous pouvez red�finir les contr�les avec l'outil "metro_cfg".
 Le joystick est support� (test� avec un pad).

* Pour la version SDL + OpenGL (conseill�e) :
- F9 : Toggle mode normal / TV. Full screen obligatoire.

* Pour la version SDL pure (d�conseill�e) :
- F9 : Toggle mode normal 320x224 / 2x 640x448 / TV2x 640x448.
- F10 : Toggle full screen / Windowed.



> Fichier "high.scr" :

 Fichier des high-scores. Vous pouvez le supprimer pour remettre les scores � z�ro, un nouveau fichier sera recr��.



--- *EN*

> Foreword:

 It's a port of Metro-Cross, a game I enjoyed a lot at the time on my Atari ST. There are many things I'm not happy about with this project, but I think part of the fun is here anyway.



> OpenGL:

 I had to use OpenGL for this game, because I couldn't find any way to get rid of the tearing when using only SDL. It's more or less ok when the scrolling in slow, but here it was unbearable.

 Problem, I'm a complete noob with OpenGL.

 The game timing is based on the V-Sync. It has then to be activated, otherwise the game might run rather quickly... On the two Linuxes I tried the game on, the V-Sync was on by default. On my old XP, the NVidia card has a control panel through which it is easy to turn the V-Sync on or off. I have no idea what others set ups are like.

 The refresh rate should be set at 60 Hz.

 It's also due to the need of V-Sync that the game only runs in full screen mode. (There is no V-Sync in windowed mode).

 Just in case, there is a way to run a SDL only version by specifying the '/noGL' parameter. You've been warned, the result is ugly.



> Controls:

- Up, down, left, right: Moves.
- Button A: Jump.

 The default keys are the arrow keys and the left Ctrl to jump.
 You can redefine the controls with the "metro_cfg" tool.
 Joystick is supported (tested with a pad).

* SDL + OpenGL version (recommended):
- F9: Toggle modes - Normal / TV. Full screen only.

* SDL only version (not recommended):
- F9: Toggle modes - Normal 320x224 / 2x 640x448 / TV2x 640x448.
- F10: Toggle full screen / Windowed.



> "high.scr" file:

 High-scores file. You can delete it to reset the scores, a new file will be created.



--- EOF
