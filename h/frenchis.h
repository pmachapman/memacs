/*      FRENCHISO.H:    French language text strings for
                        MicroEMACS 3.12

                        (C)opyright 1988 by Daniel Lawrence
                        Translated by Bertrand DECOUTY, 10/88
                        Modified 12/89 (accent...)
                        decouty@caroline.irisa.fr,
                        {uunet,mcvax,inria}!irisa!decouty
                        IRISA-INRIA, Campus de Beaulieu, F-35042 Rennes Cedex
                        Modified 10/91 (ISO-Latin-1, 8 bits chars)
                        picheral@cicb.fr
*/

#define TEXT1   "[Lancement d'un nouvel interpr\351teur]"               /*"[Starting new CLI]"*/
#define TEXT2   "[\311criture du fichier filtre impossible]"    /*"[Cannot write filter file]"*/
#define TEXT3   "[L'ex\351cution a \351chou\351]"                       /*"[Execution failed]"*/
#define TEXT4   "Variable TERM non d\351finie"                  /*"Shell variable TERM not defined!"*/
#define TEXT5   "Terminal non 'vt100'!"                         /*"Terminal type not 'vt100'!"*/
#define TEXT6   "\r\n\n[Termin\351]"                            /*"\r\n\n[End]"*/
#define TEXT7   "Aller \340 la ligne: "                                /*"Line to GOTO: "*/
#define TEXT8   "[Annul\351]"                                   /*"[Aborted]"*/
#define TEXT9   "[Marque %d pos\351e]"                          /*"[Mark set]"*/
#define TEXT10  "[Marque %d enlev\351e]"                                /*"[Mark removed]"*/
#define TEXT11  "Pas de marque %d dans cette fen\352tre"                /*"No mark %d in this window"*/
#define TEXT12  "[Fichier Help absent]"                         /*"[Help file is not online]"*/
#define TEXT13  ": d\351crire-la-touche "                               /*": describe-key "*/
#define TEXT14  "Non li\351e"                                   /*"Not Bound"*/
#define TEXT15  ": lier-une-touche "                            /*": bind-to-key "*/
#define TEXT16  "[Fonction inexistante]"                        /*"[No such function]"*/
#define TEXT17  "La table des liens est PLEINE!"                /*"Binding table FULL!"*/
#define TEXT18  ": touche-\340-d\351lier "                              /*": unbind-key "*/
#define TEXT19  "[Touche non li\351e]"                          /*"[Key not bound]"*/
#define TEXT20  "Cha\356ne a-propos: "                          /*"Apropos string: "*/
#define TEXT21  "Liste des liens"                               /*"Binding list"*/
#define TEXT22  "Affichage de la liste des liens impossible"    /*"Can not display binding list"*/
#define TEXT23  "[Cr\351ation de la liste des liens]"              /*"[Building binding list]"*/
#define TEXT24  "Tampon \340 utiliser"                          /*"Use buffer"*/
#define TEXT25  "Tampon \340 utiliser: "                                /*"Use buffer: "*/
#define TEXT26  "Tampon \340 d\351truire"                               /*"Kill buffer"*/
#define TEXT27  "Tampon \340 d\351piler: "                              /*"Pop buffer: "*/
#define TEXT28  "Le tampon est affich\351"                              /*"Buffer is being displayed"*/
#define TEXT29  "Changer le nom du tampon en: "                 /*"Change buffer name to: "*/

/*              "ACTN   Modes      Size  Buffer          File"*/
#define TEXT30  "ACTN   Modes    Taille  Tampon          Fichier"
#define TEXT31  "         Modes Globaux"                        /*"         Global Modes"*/
#define TEXT32  "Annuler les modifications"                     /*"Discard changes"*/
#define TEXT33  "Cha\356ne d'encryptage: "                              /*"Encryption String: "*/
#define TEXT34  "Fichier: "                                     /*"File: "*/
#define TEXT35  "autre utilisateur"                             /*"another user"*/
#define TEXT36  "ERREUR AU VERROUILLAGE"                        /*"LOCK ERROR -- "*/
#define TEXT37  "v\351rification de l'existence de %s\n"                /*"checking for existence of %s\n"*/
#define TEXT38  "cr\351ation du r\351pertoire %s\n"                     /*"making directory %s\n"*/
#define TEXT39  "en cours de cr\351ation de %s\n"                       /*"creating %s\n"*/
#define TEXT40  "cr\351ation d'un fichier verrouill\351 impossible"     /*"could not create lock file"*/
#define TEXT41  "pid: %ld\n"                                    /*"pid is %ld\n"*/
#define TEXT42  "lecture du fichier verrouill\351 %s\n"         /*"reading lock file %s\n"*/
#define TEXT43  "lecture du fichier verrouill\351 impossible"   /*"could not read lock file"*/
#define TEXT44  "le pid de %s est %ld\n"                        /*"pid in %s is %ld\n"*/
#define TEXT45  "envoi d'un signal au processus %ld\n"          /*"signaling process %ld\n"*/
#define TEXT46  "le processus existe"                           /*"process exists"*/
#define TEXT47  "kill a echou\351"                                      /*"kill was bad"*/
#define TEXT48  "succ\350s du kill; le processus existe"                /*"kill was good; process exists"*/
#define TEXT49  "unlink de %s en cours\n"                               /*"attempting to unlink %s\n"*/
#define TEXT50  "destruction impossible d'un fichier verrouill\351"     /*"could not remove lock file"*/
#define TEXT51  "Variable \340 positionner: "                           /*"Variable to set: "*/
#define TEXT52  "%%La variable '%s' n'existe pas"               /*"%%No such variable as '%s'"*/
#define TEXT53  "Valeur: "                                      /*"Value: "*/
#define TEXT54  "[Macro termin\351e anormalement]"                      /*"[Macro aborted]"*/
#define TEXT55  "Affichage de la variable: "                    /*"Variable to display: "*/
#define TEXT56  "Liste des variables"                           /*"Variable list"*/
#define TEXT57  "Affichage impossible de la liste des variables"        /*"Can not display variable list"*/
#define TEXT58  "[Construction de la liste des variables]"      /*"[Building variable list]"*/
#define TEXT59  "[Marge droite colonne: %d]"                   /*"[Fill column is %d]"*/

/*              "Line %d/%d Col %d/%d Char %D/%D (%d%%) char = 0x%x"*/
#define TEXT60  "Ligne %D/%D Col %d/%d Car %D/%D (%d%%) car = 0x%x"

#define TEXT61  "<NOT USED>"                                    /*"not used"*/
#define TEXT62  "Mode global \340 "                             /*"Global mode to "*/
#define TEXT63  "Mode \340 "                                    /*"Mode to "*/
#define TEXT64  "ajouter: "                                     /*"add: "*/
#define TEXT65  "supprimer: "                                   /*"delete: "*/
#define TEXT66  "Mode inexistant!"                              /*"No such mode!"*/
#define TEXT67  "Message \340 \351crire: "                              /*"Message to write: "*/
#define TEXT68  "Cha\356ne \340 ins\351rer: "                   /*"String to insert<META>: "*/
#define TEXT69  "Cha\356ne \340 substituer: "                   /*"String to overwrite<META>: "*/
#define TEXT70  "[R\351gion copi\351e]"                         /*"[region copied]"*/
#define TEXT71  "%%Ce tampon est d\351j\340 diminu\351"                 /*"%%This buffer is already narrowed"*/

/*              "%%Must narrow at least 1 full line"*/
#define TEXT72  "%%La diminution doit \352tre d'au moins une ligne compl\350te"

#define TEXT73  "[Le tampon est diminu\351]"                    /*"[Buffer is narrowed]"*/
#define TEXT74  "%%Ce tampon n'est pas diminu\351"                      /*"%%This buffer is not narrowed"*/
#define TEXT75  "[Tampon \351largi]"                            /*"[Buffer is widened]"*/
#define TEXT76  "Pas de marque dans cette fen\352tre"           /*"No mark set in this window"*/
#define TEXT77  "Bogue: marque perdue"                            /*"Bug: lost mark"*/
#define TEXT78  "Recherche avant "                              /*"Search"*/
#define TEXT79  "Pas trouv\351"                                 /*"Not found"*/
#define TEXT80  "Cha\356ne non initialis\351e"                  /*"No pattern set"*/
#define TEXT81  "Recherche arri\350re "                         /*"Reverse search"*/
#define TEXT82  "Prenant en compte %d avec un saut de %d"       /*"Considering %d with jump %d"*/
#define TEXT83  "Un autre cas est %d"                           /*"Its other case is %d"*/
#define TEXT84  "Remplacer "                                    /*"Replace"*/
#define TEXT85  "Remplacer avec confirmation "                  /*"Query replace"*/
#define TEXT86  "par "                                          /*"with"*/
#define TEXT87  "Remplacer '"                                   /*"Replace '"*/
#define TEXT88  "' par '"                                       /*"' with '"*/
#define TEXT89  "Annul\351!"                                    /*"Aborted!"*/

/*              "(Y)es, (N)o, (!)Do rest, (U)ndo last, (^G)Abort, (.)Abort back, (?)Help: "*/
#define TEXT90  "(O)Oui, (N)on, (!)Remp.tout, (U)ndo prec., (^G)Fin,(.)Fin debut, (?)Aide: "

#define TEXT91  "Cha\356ne vide remplac\351e, arr\352t"                 /*"Empty string replaced, stopping."*/
#define TEXT92  "%d substitutions"                              /* same in French */
#define TEXT93  "%%ERREUR pendant la suppression"               /*"%%ERROR while deleting"*/
#define TEXT94  "%%Plus de m\351moire disponible"                       /*"%%Out of memory"*/
#define TEXT95  "%%mceq: qu'est-ce que %d?"                     /*"%%mceq: what is %d?"*/
#define TEXT96  "%%Pas de caract\350res dans cette classe"              /*"%%No characters in character class"*/
#define TEXT97  "%%Classe de caract\350res non termin\351e"             /*"%%Character class not ended"*/
#define TEXT98  "Marge droite non initialis\351e"                       /*"No fill column set"*/
#define TEXT99  "[PLUS DE M\311MOIRE]"                          /*"[OUT OF MEMORY]"*/

/*              "Words %D Chars %D Lines %d Avg chars/word %f"*/
#define TEXT100 "Mots %D Car %D Lignes %d Moyenne car/mots %f"
#define TEXT101 "[Je ne peux pas chercher et me d\351placer en m\352me temps!]" /*"[Can not search and goto at the same time!]"*/
#define TEXT102 "[Aller-a: Param\350tre erron\351]"                             /*"[Bogus goto argument]"*/
#define TEXT103 "[Sauvegarde de %s]"                                    /*"[Saving %s]"*/
#define TEXT104 "Des tampons ont \351t\351 modifi\351s. On quitte malgr\351 tout"       /*"Modified buffers exist. Leave anyway"*/
#define TEXT105 "%%Macro d\351j\340 active"                                     /*"%%Macro already active"*/
#define TEXT106 "[D\351but de la macro]"                                /*"[Start macro]"*/
#define TEXT107 "%%Macro non active"                            /*"%%Macro not active"*/
#define TEXT108 "[Fin de la macro]"                             /*"[End macro]"*/
#define TEXT109 "[Touche interdite dans le mode VIEW]"          /*"[Key illegal in VIEW mode]"*/
#define TEXT110 "[Commande \340 usage RESTREINT]"                       /*"[That command is RESTRICTED]"*/
#define TEXT111 "Pas de macro indiqu\351e"                              /*"No macro specified"*/
#define TEXT112 "Num\351ro de macro hors limite"                        /*"Macro number out of range"*/
#define TEXT113 "Cr\351ation de macro impossible"                       /*"Can not create macro"*/
#define TEXT114 "Nom de la proc\351dure: "                     /*"Procedure name: "*/
#define TEXT115 "Ex\351cuter la proc\351dure"                           /*"Execute procedure"*/
#define TEXT116 "Proc\351dure inexistante: "                           /*"No such procedure"*/
#define TEXT117 "Ex\351cuter le tampon: "                              /*"Execute buffer"*/
#define TEXT118 "Tampon inexistant"                             /*"No such buffer"*/
#define TEXT119 "%%Plus de m\351moire pendant la recherche"     /*"%%Out of memory during while scan"*/

#define TEXT120 "%%!BREAK en-dehors d'une boucle !WHILE"        /*"%%!BREAK outside of any !WHILE loop"*/
#define TEXT121 "%%!ENDWHILE sans !WHILE"                       /*"%%!ENDWHILE with no preceding !WHILE in '%s'"*/
#define TEXT122 "%%!WHILE sans !ENDWHILE"                       /*"%%!WHILE with no matching !ENDWHILE in '%s'"*/
#define TEXT123 "%%Plus de m\351moire pendant l'ex\351cution d'une macro"       /*"%%Out of Memory during macro execution"*/
#define TEXT124 "%%Directive inconnue"                                  /*"%%Unknown Directive"*/
#define TEXT125 "Plus de m\351moire pour l'enregistrement d'une macro"  /*"Out of memory while storing macro"*/
#define TEXT126 "%%Erreur interne boucle While"                         /*"%%Internal While loop error"*/
#define TEXT127 "%%Etiquette inexistante"                       /*"%%No such label"*/

/*              "(e)val exp, (c/x)ommand, (t)rack exp, (^G)abort, <SP>exec, <META>stop debug"*/
#define TEXT128 "(e)val exp, (c/x)ommand, (t)rack exp, (^G)avorte,<SP>exec, <META> stop debug"

#define TEXT129 "Ex\351cuter le fichier: "                     /*"File to execute: "*/
#define TEXT130 "Macro non d\351finie"                          /*"Macro not defined"*/
#define TEXT131 "Lire le fichier "                               /*"Read file: "*/
#define TEXT132 "Ins\351rer le fichier "                         /*"Insert file: "*/
#define TEXT133 "Charger le fichier "                            /*"Find file: "*/
#define TEXT134 "Voir le fichier"                               /*"View file: "*/
#define TEXT135 "[Ancien tampon]"                               /*"[Old buffer]"*/
#define TEXT136 "Nom du tampon: "                               /*"Buffer name: "*/
#define TEXT137 "Cr\351ation du tampon impossible"                      /*"Cannot create buffer"*/
#define TEXT138 "[Nouveau fichier]"                             /*"[New file]"*/
#define TEXT139 "[Lecture du fichier en cours]"                 /*"[Reading file]"*/
#define TEXT140 "Lu "                                           /*"Read "*/
#define TEXT141 "ERREUR ENTR\311E/SORTIE"                               /*"I/O ERROR, "*/
#define TEXT142 "PLUS DE M\311MOIRE"                            /*"OUT OF MEMORY, "*/
#define TEXT143 " ligne"                                        /*" line"*/
#define TEXT144 "\311crire le fichier "                               /*"Write file"*/
#define TEXT145 "Pas de nom de fichier"                         /*"No file name"*/
#define TEXT146 "Fichier tronqu\351..\340 \351crire ailleurs"           /*"Truncated file..write it out"*/

#define TEXT147 "Tampon raccourci.. \340 sauvegarder"           /*"Narrowed Buffer..write it out"*/
#define TEXT148 "[\311criture en cours...]"                     /*"[Writing...]"*/
#define TEXT149 "[\311crit "                                    /*"[Wrote "*/
#define TEXT150 ", sauvegard\351 sous le nom "                  /*", saved as "*/
#define TEXT151 "Nom: "                                         /*"Name: "*/
#define TEXT152 "[Fichier inexistant]"                          /*"[No such file]"*/
#define TEXT153 "[Insertion du fichier en cours]"               /*"[Inserting file]"*/
#define TEXT154 "Ins\351r\351 "                                 /*"Inserted "*/
#define TEXT155 "Ouverture du fichier en \351criture impossible"        /*"Cannot open file for writing"*/
#define TEXT156 "Erreur \340 la fermeture du fichier"           /*"Error closing file"*/
#define TEXT157 "Erreur en \351criture E/S"                     /*"Write I/O error"*/
#define TEXT158 "Erreur en lecture du fichier"                  /*"File read error"*/
#define TEXT159 "%J'ai besoin d'un num\351ro de touche de fonction"     /*"%Need function key number"*/
#define TEXT160 "%Num\351ro de touche de fonction hors limite"          /*"%Function key number out of range"*/
#define TEXT161 "Contenu de l'\351tiquette: "                   /*"Label contents: "*/
#define TEXT162 " [o/n]? "                                      /*" [y/n]? "*/
#define TEXT163 "pas de valeur par d\351faut"                   /*"no default"*/
#define TEXT164 "[\351chec de la recherche]"                    /*"[search failed]"*/
#define TEXT165 "Recherche Incr\351mentale: "                      /*"ISearch: "*/
#define TEXT166 "? Cha\356ne de recherche trop longue"          /*"? Search string too long"*/
#define TEXT167 "? commande trop longue"                        /*"? command too long"*/
#define TEXT168 "%%Insertion de cha\356ne impossible"           /*"%%Can not insert string"*/
#define TEXT169 "Ins\351r\351(e)"                                       /*"Inserted"*/
#define TEXT170 "bogue: linsert"                                  /* same */
#define TEXT171 "Remplac\351(e)"                                        /*"Replaced"*/
#define TEXT172 "%%Plus de m\351moire en mode remplacement"             /*"%%Out of memory while overwriting"*/
#define TEXT173 "ERREUR VERROUILLAGE: table de verrouillage pleine"    /*"LOCK ERROR: Lock table full"*/
#define TEXT174 "Verrouillage impossible, plus de m\351moire"           /*"Cannot lock, out of memory"*/
#define TEXT175 "VERROUILL\311"                                         /*"LOCK"*/
#define TEXT176 "Fichier utilis\351 par "                                       /*"File in use by "*/
#define TEXT177 ", on passe outre ?"                                            /*", overide?"*/
#define TEXT178 "[Obtention d'un message d'erreur systeme impossible]"  /*"[can not get system error message]"*/
#define TEXT179 "  A-propos de MicroEmacs"                                      /*"  About MicroEmacs"*/
#define TEXT180 "%%R\351solution inexistante"                           /*"%%No such resolution"*/
#define TEXT181 "%%R\351solution interdite pour ce type d'\351cran"             /*"%%Resolution illegal for this monitor"*/
#define TEXT182 "Variable TERM non d\351finie"                          /*"Environment variable TERM not defined!"*/
#define TEXT183 "%s: type de terminal inconnu"                          /*"Unknown terminal type %s!"*/
#define TEXT184 "Description termcap incompl\350te (lignes)"            /*"termcap entry incomplete (lines)"*/
#define TEXT185 "Description termcap incompl\350te (colonnes)"          /*"Termcap entry incomplete (columns)"*/
#define TEXT186 "Description termcap incompl\350te"                     /*"Incomplete termcap entry\n"*/
#define TEXT187 "Description du terminal trop importante"               /*"Terminal description too big!\n"*/
#define TEXT188 "[Termin\351]"                                  /*"[End]"*/
#define TEXT189 "Description du terminal non trouv\351e\n"              /*"Cannot find entry for terminal type.\n"*/

/*              "Check terminal type with \"SHOW TERMINAL\" or\n"*/
#define TEXT190 "V\351rifiez le type de terminal avec \"SHOW TERMINAL\" ou\n"

/*              "try setting with \"SET TERMINAL/INQUIRE\"\n"*/
#define TEXT191 "essayez de l'initialiser avec \"SET TERMINAL/INQUIRE\"\n"

/*              "The terminal type does not have enough power to run\n"*/
#define TEXT192 "Ce terminal n'est pas assez performant pour \n"

/*              "MicroEMACS.  Try a different terminal or check\n"*/
#define TEXT193 "MicroEMACS. Essayez un autre terminal ou v\351rifiez\n"

/*              "type with \"SHOW TERMINAL\".\n"*/
#define TEXT194 "celui-ci avec \"SHOW TERMINAL\".\n"

/*              "Cannot open channel to terminal.\n"*/
#define TEXT195 "Ouverture du canal associ\351 au terminal impossible.\n"

/*              "Cannot obtain terminal settings.\n"*/
#define TEXT196 "Obtention des caract\351ristiques du terminal impossible.\n"

/*              "Cannot modify terminal settings.\n"*/
#define TEXT197 "Modification des caract\351ristiques du terminal impossible.\n"

#define TEXT198 "Erreur Entr\351e/Sortie (%d,%d)\n"             /*"I/O error (%d,%d)\n"*/
#define TEXT199 "[Lancement de DCL]\r\n"                        /*"[Starting DCL]\r\n"*/
#define TEXT200 "[Appel de DCL]\r\n"                            /*"[Calling DCL]\r\n"*/
#define TEXT201 "[Non encore disponible sous VMS]"              /*"[Not available yet under VMS]"*/
#define TEXT202 "Terminal non 'vt52' ou 'z19'!"                 /*"Terminal type not 'vt52'or 'z19' !"*/
#define TEXT203 "Num\351ro de fen\352tre hors limite"                   /*"Window number out of range"*/
#define TEXT204 "Destruction de cette fen\352tre impossible"    /*"Can not delete this window"*/
#define TEXT205 "D\351coupage impossible d'une fen\352tre de %d lignes" /*"Cannot split a %d line window"*/
#define TEXT206 "Une seule fen\352tre"                          /*"Only one window"*/
#define TEXT207 "Changement impossible"                         /*"Impossible change"*/
#define TEXT208 "[Fen\352tre inexistante]"                              /*"[No such window exists]"*/
#define TEXT209 "%%Taille d'\351cran hors limite"                       /*"%%Screen size out of range"*/
#define TEXT210 "%%Largeur d'\351cran hors limite"                      /*"%%Screen width out of range"*/
#define TEXT211 "Liste des Fonctions"                           /*"Function list"*/
#define TEXT212 "Affichage de la liste des fonctions impossible"/*"Can not display function list"*/
#define TEXT213 "[Cr\351ation de la liste des fonctions]"               /*"[Building function list]"*/
#define TEXT214 "%%Fichier %s inexistant"                       /*"%%No such file as %s"*/
#define TEXT215 ": macro-une-touche "                           /*": macro-to-key "*/
#define TEXT216 "Impossible de lire/\351crire des r\351pertoires !!!"        /*"Cannot read/write directories!!!"*/
#define TEXT217 "[Pas encore disponible sous AOS/VS]"           /*"[Not available yet under AOS/VS]"*/
#define TEXT218 "Compl\351ter le fichier "                       /*"Append file: "*/
#define TEXT219 "%%\311chec de la macro"                                /* "%%Macro Failed" */
#define TEXT220 "Ligne %D/%D Col %d/%d Car %D/%D (%d%%) car = 0x%x%x"
#define TEXT221 "Trop de groupes"                               /* "Too many groups" */
#define TEXT222 "Groupe non ferm\351"                           /* "Group not ended" */
#define TEXT223 "%%Colonne origine hors limite"         /* "%%Column origin out of range" */
#define TEXT224 "%%Ligne origine hors limite"           /* "%%Row origin out of range" */
#define TEXT225 "[Changement pour l'\351cran %s]"                  /* "[Switched to screen %s]" */
#define TEXT226 "%%Impossible de d\351truire un tampon en cours d'ex\351cution" /* "%%Can not kill an executing buffer" */
#define TEXT227 "\n--- Appuyer sur une touche quelconque pour continuer ---" /* "\n--- Press any key to Continue ---" */
#define TEXT228 "[Anneau des effacements vid\351]\n"
#define TEXT229 " dans <"                                       /*" in <"*/
#define TEXT230 "> ligne "                                      /*" > at line "*/

#define TEXT240 "[Aucun \351cran de ce nom]"                            /*"[No such screen]"*/
#define TEXT241 "%%Impossible de supprimer l'\351cran courant"     /*"%%Can't delete current screen"*/
#define TEXT242 "Chercher l'\351cran: "                           /*"Find Screen: "*/
#define TEXT243 "Supprimer l'\351cran: "                          /*"Delete Screen: "*/
#define TEXT244 "%%Fonction '%s' inexistante"                   /*"%%No such function as '%s'"*/
#define TEXT245 "%%Division par z\351ro interdite"                 /*"%%Division by Zero is illegal"*/
#define TEXT246 "%%Il faut un num\351ro de touche fonction"  /*"%%Need function key number"*/
#define TEXT247 "%%num\351ro de touche fonction hors limite"    /*"%%Function key number out of range"*/
#define	TEXT248 "Entrer une \202tiquette: "                    /*"Enter Label String: "*/

/* some of these are just used in the microsoft windows version */
#define TEXT300 "[Menu incorrect]"                              /*"[Incorrect menu]"*/
#define TEXT301 "[Trop de menus imbriqu\351s]"                     /*"[Too many nested popup menus]"*/
#define TEXT302 "[Manque de ressources]"                        /*"[Lack of resources]"*/

#define TEXT303 "Menu: "
#define TEXT304 "Function: "
#define TEXT305 "Macro: "
#define TEXT306 "Menu: "
#define TEXT307 "Help file: "
#define TEXT308 "Help key: "
#define TEXT310 "Alt+"
#define TEXT311 "Shift+"
#define TEXT312 "BkSp"
#define TEXT313 "Tab"
#define TEXT314 "Enter"
#define TEXT315 "Esc"
#define TEXT316 "Ctrl+"
#define TEXT317 "Home"
#define TEXT318 "DownArrow"
#define TEXT319 "UpArrow"
#define TEXT320 "LeftArrow"
#define TEXT321 "RightArrow"
#define TEXT322 "End"
#define TEXT323 "PageUp"
#define TEXT324 "PageDown"
#define TEXT325 "Ins"
#define TEXT326 "Del"
#define TEXT327 "F10"
#define CHAR328 'F'
#define TEXT329 "SpaceBar"
#define TEXT330 " - Historique des messages"                    /*" - Message history"*/
#define TEXT331 "Modes globaux"                                 /*"Global modes"*/
#define TEXT332 "Modes pour le tampon: "                       /*"Modes for buffer: "*/
#define TEXT333 "\311criture d'un fichier en cours. Quitter plus tard !"        /*"File write in progress. Quit later !"*/
#define TEXT334 "[R\351pertoire inexistant]"                       /*"[No such directory]"*/
#define TEXT335 "Changer le nom de l'\351cran en: "                    /*"Change screen name to: "*/
#define TEXT336 "[Nom d'\351cran d\351j\340 utilis\351]"                      /*"[Screen name already in use]"*/
#define TEXT337 "impossible de contr\364ler un programme externe"       /*"cannot monitor external program"*/

