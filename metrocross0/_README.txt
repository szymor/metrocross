
 Metro-Cross, (c) Namco 1985
 2013 version by Clément CORDE / c1702@yahoo.com

 All graphical content (c) Namco, rips by myself.
 Using ST-Sound by Leonard/OXG (http://leonard.oxg.free.fr/), and ST music.



--- *FR*

> Préambule :

 Il s'agit d'un petit portage de Metro-Cross, jeu que j'avais beaucoup aimé à l'époque sur le ST. Il y a plein de choses dont je ne suis pas content dans ce projet, mais en jouant je retrouve du fun de l'original.



> OpenGL :

 Pour ce jeu, j'ai du utiliser OpenGL car le tearing en SDL pur est insupportable. Avec un scrolling lent, ça passe assez inaperçu, mais là non.

 Problème, je suis un noob total en OpenGL.

 Le timing du jeu est basé sur la V-Sync. Il faut donc que cette dernière soit activée, sinon ça risque d'aller très très vite. Sous les deux configs Linux que j'ai testées, la V-Sync était activée par défaut. Avec mon vieux XP, la carte NVidia propose un panneau de configuration. Pour les autres je ne sais pas...

 Il faut aussi que la fréquence de rafraîchissement soit 60 Hz.

 C'est aussi pour avoir la V-Sync que le jeu ne tourne qu'en full screen. (En mode fenêtré, il n'y a pas de V-Sync).

 Au cas où, il est quand même possible de lancer une version SDL pure en passant '/noGL' en paramètre au programme. Mais ce n'est vraiment pas beau.



> Contrôles :

- Haut, bas, droite, gauche : Déplacement.
- Bouton A : Saut.

 Les touches par défaut sont : Flèches de direction et Ctrl gauche pour sauter.
 Vous pouvez redéfinir les contrôles avec l'outil "metro_cfg".
 Le joystick est supporté (testé avec un pad).

* Pour la version SDL + OpenGL (conseillée) :
- F9 : Toggle mode normal / TV. Full screen obligatoire.

* Pour la version SDL pure (déconseillée) :
- F9 : Toggle mode normal 320x224 / 2x 640x448 / TV2x 640x448.
- F10 : Toggle full screen / Windowed.



> Fichier "high.scr" :

 Fichier des high-scores. Vous pouvez le supprimer pour remettre les scores à zéro, un nouveau fichier sera recréé.



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
