/*	FRENCHDOS.H:	French language text strings for
                        MicroEMACS 3.12

			(C)opyright 1988 by Daniel Lawrence
			Translated by Bertrand DECOUTY, 10/88
			Modified 12/89 (accent...)
			decouty@caroline.irisa.fr, 
			{uunet,mcvax,inria}!irisa!decouty
			IRISA-INRIA, Campus de Beaulieu, F-35042 Rennes Cedex
			Modified 02/93 (code page 850, 8 bits chars)
			picheral@cicb.fr
*/

#define	TEXT1	"[Lancement d'un nouvel interpr\202teur]"		/*"[Starting new CLI]"*/
#define	TEXT2	"[\220criture du fichier filtre impossible]"	/*"[Cannot write filter file]"*/
#define	TEXT3	"[L'ex\202cution a \202chou\202]"			/*"[Execution failed]"*/
#define	TEXT4	"Variable TERM non d\202finie"			/*"Shell variable TERM not defined!"*/
#define	TEXT5	"Terminal non 'vt100'!"				/*"Terminal type not 'vt100'!"*/
#define	TEXT6	"\r\n\n[Termin\202]"				/*"\r\n\n[End]"*/
#define	TEXT7	"Aller \205 la ligne: "				/*"Line to GOTO: "*/
#define	TEXT8	"[Annul\202]"					/*"[Aborted]"*/
#define	TEXT9	"[Marque %d pos\202e]"				/*"[Mark set]"*/
#define	TEXT10	"[Marque %d enlev\202e]"				/*"[Mark removed]"*/
#define	TEXT11	"Pas de marque %d dans cette fen\210tre"		/*"No mark %d in this window"*/
#define	TEXT12	"[Fichier Help absent]"				/*"[Help file is not online]"*/
#define	TEXT13	": d\202crire-la-touche "				/*": describe-key "*/
#define	TEXT14	"Non li\202e"					/*"Not Bound"*/
#define	TEXT15	": lier-une-touche "				/*": bind-to-key "*/
#define	TEXT16	"[Fonction inexistante]"			/*"[No such function]"*/
#define	TEXT17	"La table des liens est PLEINE!"		/*"Binding table FULL!"*/
#define	TEXT18	": touche-\205-d\202lier "				/*": unbind-key "*/
#define	TEXT19	"[Touche non li\202e]"				/*"[Key not bound]"*/
#define	TEXT20	"Cha\214ne a-propos: "				/*"Apropos string: "*/
#define	TEXT21	"Liste des liens"				/*"Binding list"*/
#define	TEXT22	"Affichage de la liste des liens impossible"	/*"Can not display binding list"*/
#define	TEXT23	"[Cr\202ation de la liste des liens]"		/*"[Building binding list]"*/
#define	TEXT24	"Tampon \205 utiliser"				/*"Use buffer"*/
#define	TEXT25	"Tampon \205 utiliser: "				/*"Use buffer: "*/
#define	TEXT26	"Tampon \205 d\202truire"				/*"Kill buffer"*/
#define	TEXT27	"Tampon \205 d\202piler: "				/*"Pop buffer: "*/
#define	TEXT28	"Le tampon est affich\202"				/*"Buffer is being displayed"*/
#define	TEXT29	"Changer le nom du tampon en: "			/*"Change buffer name to: "*/

/*		"ACTN   Modes      Size  Buffer          File"*/
#define	TEXT30	"ACTN   Modes    Taille  Tampon          Fichier"
#define	TEXT31	"         Modes Globaux"			/*"         Global Modes"*/
#define	TEXT32	"Annuler les modifications"			/*"Discard changes"*/
#define	TEXT33	"Cha\214ne d'encryptage: "				/*"Encryption String: "*/
#define	TEXT34	"Fichier: "					/*"File: "*/
#define	TEXT35	"autre utilisateur"				/*"another user"*/
#define	TEXT36	"ERREUR AU VERROUILLAGE"			/*"LOCK ERROR -- "*/
#define	TEXT37	"v\202rification de l'existence de %s\n"		/*"checking for existence of %s\n"*/
#define	TEXT38	"cr\202ation du r\202pertoire %s\n"			/*"making directory %s\n"*/
#define	TEXT39	"en cours de cr\202ation de %s\n"			/*"creating %s\n"*/
#define	TEXT40	"cr\202ation d'un fichier verrouill\202 impossible"	/*"could not create lock file"*/
#define	TEXT41	"pid: %ld\n"					/*"pid is %ld\n"*/
#define	TEXT42	"lecture du fichier verrouill\202 %s\n"		/*"reading lock file %s\n"*/
#define	TEXT43	"lecture du fichier verrouill\202 impossible"	/*"could not read lock file"*/
#define	TEXT44	"le pid de %s est %ld\n"			/*"pid in %s is %ld\n"*/
#define	TEXT45	"envoi d'un signal au processus %ld\n"		/*"signaling process %ld\n"*/
#define	TEXT46	"le processus existe"				/*"process exists"*/
#define	TEXT47	"kill a echou\202"					/*"kill was bad"*/
#define	TEXT48	"succ\212s du kill; le processus existe"		/*"kill was good; process exists"*/
#define	TEXT49	"unlink de %s en cours\n"				/*"attempting to unlink %s\n"*/
#define	TEXT50	"destruction impossible d'un fichier verrouill\202"	/*"could not remove lock file"*/
#define	TEXT51	"Variable \205 positionner: "				/*"Variable to set: "*/
#define	TEXT52	"%%La variable '%s' n'existe pas"		/*"%%No such variable as '%s'"*/
#define	TEXT53	"Valeur: "					/*"Value: "*/
#define	TEXT54	"[Macro termin\202e anormalement]"			/*"[Macro aborted]"*/
#define	TEXT55	"Affichage de la variable: "			/*"Variable to display: "*/
#define	TEXT56	"Liste des variables"				/*"Variable list"*/
#define	TEXT57	"Affichage impossible de la liste des variables"	/*"Can not display variable list"*/
#define	TEXT58	"[Construction de la liste des variables]"	/*"[Building variable list]"*/
#define	TEXT59	"[Marge droite colonne: %d]"			/*"[Fill column is %d]"*/

/*		"Line %d/%d Col %d/%d Char %D/%D (%d%%) char = 0x%x"*/
#define	TEXT60	"Ligne %D/%D Col %d/%d Car %D/%D (%d%%) car = 0x%x"

#define	TEXT61	"<NOT USED>"					/*"not used"*/
#define	TEXT62	"Mode global \205 "				/*"Global mode to "*/
#define	TEXT63	"Mode \205 "					/*"Mode to "*/
#define	TEXT64	"ajouter: "					/*"add: "*/
#define	TEXT65	"supprimer: "					/*"delete: "*/
#define	TEXT66	"Mode inexistant!"				/*"No such mode!"*/
#define	TEXT67	"Message \205 \202crire: "				/*"Message to write: "*/
#define	TEXT68	"Cha\214ne \205 ins\202rer: "			/*"String to insert<META>: "*/
#define	TEXT69	"Cha\214ne \205 substituer: "			/*"String to overwrite<META>: "*/
#define	TEXT70	"[R\202gion copi\202e]"				/*"[region copied]"*/
#define	TEXT71	"%%Ce tampon est d\202j\205 diminu\202"			/*"%%This buffer is already narrowed"*/

/*		"%%Must narrow at least 1 full line"*/
#define	TEXT72	"%%La diminution doit \210tre d'au moins une ligne compl\212te"

#define	TEXT73	"[Le tampon est diminu\202]"			/*"[Buffer is narrowed]"*/
#define	TEXT74	"%%Ce tampon n'est pas diminu\202"			/*"%%This buffer is not narrowed"*/
#define	TEXT75	"[Tampon \202largi]"				/*"[Buffer is widened]"*/
#define	TEXT76	"Pas de marque dans cette fen\210tre"		/*"No mark set in this window"*/
#define	TEXT77	"Bogue: marque perdue"				/*"Bug: lost mark"*/
#define	TEXT78	"Recherche avant "				/*"Search"*/
#define	TEXT79	"Pas trouv\202"					/*"Not found"*/
#define	TEXT80	"Cha\214ne non initialis\202e"			/*"No pattern set"*/
#define	TEXT81	"Recherche arri\212re "				/*"Reverse search"*/
#define	TEXT82	"Prenant en compte %d avec un saut de %d"	/*"Considering %d with jump %d"*/
#define	TEXT83	"Un autre cas est %d"				/*"Its other case is %d"*/
#define	TEXT84	"Remplacer "					/*"Replace"*/
#define	TEXT85	"Remplacer avec confirmation "			/*"Query replace"*/
#define	TEXT86	"par "						/*"with"*/
#define	TEXT87	"Remplacer '"					/*"Replace '"*/
#define	TEXT88	"' par '"					/*"' with '"*/
#define	TEXT89	"Annul\202!"					/*"Aborted!"*/

/*		"(Y)es, (N)o, (!)Do rest, (U)ndo last, (^G)Abort, (.)Abort back, (?)Help: "*/
#define	TEXT90	"(O)Oui, (N)on, (!)Remp.tout, (U)ndo prec., (^G)Fin,(.)Fin debut, (?)Aide: "

#define	TEXT91	"Cha\214ne vide remplac\202e, arr\210t"			/*"Empty string replaced, stopping."*/
#define	TEXT92	"%d substitutions"				/* same in French */
#define	TEXT93	"%%ERREUR pendant la suppression"		/*"%%ERROR while deleting"*/
#define	TEXT94	"%%Plus de m\202moire disponible"			/*"%%Out of memory"*/
#define	TEXT95	"%%mceq: qu'est-ce que %d?"			/*"%%mceq: what is %d?"*/
#define	TEXT96	"%%Pas de caract\212res dans cette classe"		/*"%%No characters in character class"*/
#define	TEXT97	"%%Classe de caract\212res non termin\202e"		/*"%%Character class not ended"*/
#define	TEXT98	"Marge droite non initialis\202e"			/*"No fill column set"*/
#define	TEXT99	"[PLUS DE M\220MOIRE]"				/*"[OUT OF MEMORY]"*/

/*		"Words %D Chars %D Lines %d Avg chars/word %f"*/
#define TEXT100 "Mots %D Car %D Lignes %d Moyenne car/mots %f"
#define	TEXT101	"[Je ne peux pas chercher et me d\202placer en m\210me temps!]"	/*"[Can not search and goto at the same time!]"*/
#define	TEXT102	"[Aller-a: Param\212tre erron\202]"				/*"[Bogus goto argument]"*/
#define	TEXT103	"[Sauvegarde de %s]"					/*"[Saving %s]"*/
#define	TEXT104	"Des tampons ont \202t\202 modifi\202s. On quitte malgr\202 tout"	/*"Modified buffers exist. Leave anyway"*/
#define	TEXT105	"%%Macro d\202j\205 active"					/*"%%Macro already active"*/
#define	TEXT106	"[D\202but de la macro]"				/*"[Start macro]"*/
#define	TEXT107	"%%Macro non active"				/*"%%Macro not active"*/
#define	TEXT108	"[Fin de la macro]"				/*"[End macro]"*/
#define	TEXT109	"[Touche interdite dans le mode VIEW]"		/*"[Key illegal in VIEW mode]"*/
#define	TEXT110	"[Commande \205 usage RESTREINT]"			/*"[That command is RESTRICTED]"*/
#define	TEXT111	"Pas de macro indiqu\202e"				/*"No macro specified"*/
#define	TEXT112	"Num\202ro de macro hors limite"			/*"Macro number out of range"*/
#define	TEXT113	"Cr\202ation de macro impossible"			/*"Can not create macro"*/
#define	TEXT114	"Nom de la proc\202dure: "			/*"Procedure name: "*/
#define	TEXT115 "Ex\202cuter la proc\202dure"				/*"Execute procedure"*/
#define	TEXT116	"Proc\202dure inexistante: "				/*"No such procedure"*/
#define	TEXT117	"Ex\202cuter le tampon: "				/*"Execute buffer"*/
#define	TEXT118	"Tampon inexistant"				/*"No such buffer"*/
#define	TEXT119	"%%Plus de m\202moire pendant la recherche"	/*"%%Out of memory during while scan"*/

#define	TEXT120	"%%!BREAK en-dehors d'une boucle !WHILE"	/*"%%!BREAK outside of any !WHILE loop"*/
#define	TEXT121	"%%!ENDWHILE sans !WHILE"			/*"%%!ENDWHILE with no preceding !WHILE in '%s'"*/
#define	TEXT122	"%%!WHILE sans !ENDWHILE"			/*"%%!WHILE with no matching !ENDWHILE in '%s'"*/
#define	TEXT123	"%%Plus de m\202moire pendant l'ex\202cution d'une macro"	/*"%%Out of Memory during macro execution"*/
#define	TEXT124	"%%Directive inconnue"					/*"%%Unknown Directive"*/
#define	TEXT125	"Plus de m\202moire pour l'enregistrement d'une macro"	/*"Out of memory while storing macro"*/
#define	TEXT126	"%%Erreur interne boucle While"				/*"%%Internal While loop error"*/
#define	TEXT127	"%%Etiquette inexistante"			/*"%%No such label"*/

/*		"(e)val exp, (c/x)ommand, (t)rack exp, (^G)abort, <SP>exec, <META>stop debug"*/
#define	TEXT128	"(e)val exp, (c/x)ommand, (t)rack exp, (^G)avorte,<SP>exec, <META> stop debug"

#define	TEXT129	"Ex\202cuter le fichier: "			/*"File to execute: "*/
#define	TEXT130	"Macro non d\202finie"				/*"Macro not defined"*/
#define	TEXT131	"Lire le fichier "				/*"Read file: "*/
#define	TEXT132	"Ins\202rer le fichier "				/*"Insert file: "*/
#define	TEXT133	"Charger le fichier "				/*"Find file: "*/
#define	TEXT134	"Voir le fichier "				/*"View file: "*/
#define	TEXT135	"[Ancien tampon]"				/*"[Old buffer]"*/
#define	TEXT136	"Nom du tampon: "				/*"Buffer name: "*/
#define	TEXT137	"Cr\202ation du tampon impossible"			/*"Cannot create buffer"*/
#define	TEXT138	"[Nouveau fichier]"				/*"[New file]"*/
#define	TEXT139	"[Lecture du fichier en cours]"			/*"[Reading file]"*/
#define	TEXT140	"Lu "						/*"Read "*/
#define	TEXT141	"ERREUR ENTR\220E/SORTIE"				/*"I/O ERROR, "*/
#define	TEXT142	"PLUS DE M\220MOIRE"				/*"OUT OF MEMORY, "*/
#define	TEXT143	" ligne"					/*" line"*/
#define	TEXT144	"\220crire le fichier "				/*"Write file"*/
#define	TEXT145	"Pas de nom de fichier"				/*"No file name"*/
#define	TEXT146	"Fichier tronqu\202..\205 \202crire ailleurs"		/*"Truncated file..write it out"*/

#define	TEXT147	"Tampon raccourci.. \205 sauvegarder"		/*"Narrowed Buffer..write it out"*/
#define	TEXT148	"[\220criture en cours...]"			/*"[Writing...]"*/
#define	TEXT149	"[\220crit "					/*"[Wrote "*/
#define	TEXT150	", sauvegard\202 sous le nom "			/*", saved as "*/
#define	TEXT151	"Nom: "						/*"Name: "*/
#define	TEXT152	"[Fichier inexistant]"				/*"[No such file]"*/
#define	TEXT153	"[Insertion du fichier en cours]"		/*"[Inserting file]"*/
#define	TEXT154	"Ins\202r\202 "					/*"Inserted "*/
#define	TEXT155	"Ouverture du fichier en \202criture impossible"	/*"Cannot open file for writing"*/
#define	TEXT156	"Erreur \205 la fermeture du fichier"		/*"Error closing file"*/
#define	TEXT157	"Erreur en \202criture E/S"			/*"Write I/O error"*/
#define	TEXT158	"Erreur en lecture du fichier"			/*"File read error"*/
#define	TEXT159	"%J'ai besoin d'un num\202ro de touche de fonction"	/*"%Need function key number"*/
#define	TEXT160	"%Num\202ro de touche de fonction hors limite"		/*"%Function key number out of range"*/
#define	TEXT161	"Contenu de l'\202tiquette: "			/*"Label contents: "*/
#define	TEXT162	" [o/n]? "					/*" [y/n]? "*/
#define	TEXT163	"pas de valeur par d\202faut"			/*"no default"*/
#define	TEXT164	"[\202chec de la recherche]"			/*"[search failed]"*/
#define	TEXT165	"Recherche Incr\202mentale: "			/*"ISearch: "*/
#define	TEXT166	"? Cha\214ne de recherche trop longue"		/*"? Search string too long"*/
#define	TEXT167	"? commande trop longue"			/*"? command too long"*/
#define	TEXT168	"%%Insertion de cha\214ne impossible"		/*"%%Can not insert string"*/
#define	TEXT169	"Ins\202r\202(e)"					/*"Inserted"*/
#define	TEXT170	"bogue: linsert"					/* same */
#define	TEXT171	"Remplac\202(e)"					/*"Replaced"*/
#define	TEXT172	"%%Plus de m\202moire en mode remplacement"		/*"%%Out of memory while overwriting"*/
#define	TEXT173	"ERREUR VERROUILLAGE: table de verrouillage pleine"	/*"LOCK ERROR: Lock table full"*/
#define	TEXT174	"Verrouillage impossible, plus de m\202moire"		/*"Cannot lock, out of memory"*/
#define	TEXT175	"VERROUILL\220"						/*"LOCK"*/
#define	TEXT176	"Fichier utilis\202 par "					/*"File in use by "*/
#define	TEXT177	", on passe outre ?"						/*", overide?"*/
#define	TEXT178	"[Obtention d'un message d'erreur systeme impossible]"	/*"[can not get system error message]"*/
#define	TEXT179	"  A-propos de MicroEmacs"					/*"  About MicroEmacs"*/
#define	TEXT180	"%%R\202solution inexistante"				/*"%%No such resolution"*/
#define	TEXT181	"%%R\202solution interdite pour ce type d'\202cran"		/*"%%Resolution illegal for this monitor"*/
#define	TEXT182	"Variable TERM non d\202finie"				/*"Environment variable TERM not defined!"*/
#define	TEXT183	"%s: type de terminal inconnu"				/*"Unknown terminal type %s!"*/
#define	TEXT184	"Description termcap incompl\212te (lignes)"		/*"termcap entry incomplete (lines)"*/
#define	TEXT185	"Description termcap incompl\212te (colonnes)"		/*"Termcap entry incomplete (columns)"*/
#define	TEXT186	"Description termcap incompl\212te"			/*"Incomplete termcap entry\n"*/
#define	TEXT187	"Description du terminal trop importante"		/*"Terminal description too big!\n"*/
#define	TEXT188	"[Termin\202]"					/*"[End]"*/
#define	TEXT189	"Description du terminal non trouv\202e\n"		/*"Cannot find entry for terminal type.\n"*/

/*		"Check terminal type with \"SHOW TERMINAL\" or\n"*/
#define	TEXT190	"V\202rifiez le type de terminal avec \"SHOW TERMINAL\" ou\n"

/*		"try setting with \"SET TERMINAL/INQUIRE\"\n"*/
#define	TEXT191	"essayez de l'initialiser avec \"SET TERMINAL/INQUIRE\"\n"

/*		"The terminal type does not have enough power to run\n"*/
#define	TEXT192	"Ce terminal n'est pas assez performant pour \n"

/*		"MicroEMACS.  Try a different terminal or check\n"*/
#define	TEXT193	"MicroEMACS. Essayez un autre terminal ou v\202rifiez\n"

/*		"type with \"SHOW TERMINAL\".\n"*/
#define	TEXT194	"celui-ci avec \"SHOW TERMINAL\".\n"

/*		"Cannot open channel to terminal.\n"*/
#define	TEXT195	"Ouverture du canal associ\202 au terminal impossible.\n"

/*		"Cannot obtain terminal settings.\n"*/
#define	TEXT196	"Obtention des caract\202ristiques du terminal impossible.\n"

/*		"Cannot modify terminal settings.\n"*/
#define	TEXT197	"Modification des caract\202ristiques du terminal impossible.\n"

#define	TEXT198	"Erreur Entr\202e/Sortie (%d,%d)\n"		/*"I/O error (%d,%d)\n"*/
#define	TEXT199	"[Lancement de DCL]\r\n"			/*"[Starting DCL]\r\n"*/
#define	TEXT200	"[Appel de DCL]\r\n"				/*"[Calling DCL]\r\n"*/
#define	TEXT201	"[Non encore disponible sous VMS]"		/*"[Not available yet under VMS]"*/
#define	TEXT202	"Terminal non 'vt52' ou 'z19'!"			/*"Terminal type not 'vt52'or 'z19' !"*/
#define	TEXT203	"Num\202ro de fen\210tre hors limite"			/*"Window number out of range"*/
#define	TEXT204	"Destruction de cette fen\210tre impossible"	/*"Can not delete this window"*/
#define	TEXT205	"D\202coupage impossible d'une fen\210tre de %d lignes"	/*"Cannot split a %d line window"*/
#define	TEXT206	"Une seule fen\210tre"				/*"Only one window"*/
#define	TEXT207	"Changement impossible"				/*"Impossible change"*/
#define	TEXT208	"[Fen\210tre inexistante]"				/*"[No such window exists]"*/
#define	TEXT209	"%%Taille d'\202cran hors limite"			/*"%%Screen size out of range"*/
#define	TEXT210	"%%Largeur d'\202cran hors limite"			/*"%%Screen width out of range"*/
#define	TEXT211	"Liste des Fonctions"				/*"Function list"*/
#define	TEXT212	"Affichage de la liste des fonctions impossible"/*"Can not display function list"*/
#define	TEXT213	"[Cr\202ation de la liste des fonctions]"		/*"[Building function list]"*/
#define	TEXT214 "%%Fichier %s inexistant"			/*"%%No such file as %s"*/
#define TEXT215 ": macro-une-touche "                           /*": macro-to-key "*/
#define TEXT216 "Impossible de  lire/\202crire des r\202pertoires !!!"        /*"Cannot read/write directories!!!"*/
#define TEXT217 "[Pas encore disponible sous AOS/VS]"           /*"[Not available yet under AOS/VS]"*/
#define TEXT218 "Compl\202 le fichier "				/* "Append file"  */
#define TEXT219 "%%\220chec de la macro"				/* "%%Macro Failed" */
#define	TEXT220	"Ligne %D/%D Col %d/%d Car %D/%D (%d%%) car = 0x%x%x"
#define TEXT221 "Trop de groupes"				/* "Too many groups" */
#define TEXT222 "Groupe non ferm\202"				/* "Group not ended" */
#define	TEXT223 "%%Colonne origine hors limite"		/* "%%Column origin out of range" */
#define	TEXT224 "%%Ligne origine hors limite"		/* "%%Row origin out of range" */
#define	TEXT225 "[Changement pour l'\202cran %s]"			/* "[Switched to screen %s]" */
#define	TEXT226	"%%Impossible de d\202truire un tampon en cours d'ex\202cution" /* "%%Can not kill an executing buffer" */
#define	TEXT227 "\n--- Appuyer sur une touche quelconque pour continuer ---" /* "\n--- Press any key to Continue ---" */
#define TEXT228 "[Anneau des effacements vid\202]\n"
#define TEXT229 " dans <"					/*" in <"*/
#define TEXT230 "> ligne "					/*" > at line "*/

#define TEXT240 "[Aucun \202cran de ce nom]"                            /*"[No such screen]"*/
#define TEXT241 "%%Impossible de supprimer l'\202cran courant"     /*"%%Can't delete current screen"*/
#define TEXT242 "Chercher l'\202cran: "                           /*"Find Screen: "*/
#define TEXT243 "Supprimer l'\202cran: "                          /*"Delete Screen: "*/
#define TEXT244 "%%Fonction '%s' inexistante"                   /*"%%No such function as '%s'"*/
#define TEXT245 "%%Division par z\202ro interdite"                 /*"%%Division by Zero is illegal"*/
#define TEXT246 "%%Il faut un num\202ro de touche fonction"	/*"%%Need function key number"*/
#define TEXT247 "%%num\202ro de touche fonction hors limite"	/*"%%Function key number out of range"*/*/
#define	TEXT248 "Entrer une \202tiquette: "			/*"Enter Label String: "*/

/* some of these are just used in the microsoft windows version */
#define TEXT300 "[Menu incorrect]"                              /*"[Incorrect menu]"*/
#define TEXT301 "[Trop de menus imbriqu\202s]"                     /*"[Too many nested popup menus]"*/
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
#define TEXT333 "\220criture d'un fichier en cours. Quitter plus tard !"	/*"File write in progress. Quit later !"*/
#define TEXT334 "[R\202pertoire inexistant]"                       /*"[No such directory]"*/
#define TEXT335 "Changer le nom de l'\202cran en: "             	/*"Change screen name to: "*/
#define TEXT336 "[Nom d'\202cran d\202j\205 utilis\202]"                   	/*"[Screen name already in use]"*/
#define TEXT337 "impossible de contr\223ler un programme externe" 	/*"cannot monitor external program"*/
