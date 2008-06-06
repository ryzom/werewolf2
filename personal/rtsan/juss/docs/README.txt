*********************************************************************
                             README
*********************************************************************

Autor(id): Taimo Peelo

viimati muudetud: 29. aprill 2005

Kuidas Eclipse keskkonnas arenduseks JUSSi t��le ajada:

1) JUSSi serveri k�ivitamiseks Eclipses peavad olema installeeritud 
   a) MySQL andmebaasis�steem
		--> http://dev.mysql.com/downloads/index.html
		T�mmata suvaline 4.x versioon.
		installeerida soovitaks vaikev��rtustega ja niimoodi et andmebaas
		arvuti k�ivitamisel alati t��le t�mmatakse. Selleks et JUSS
		andmebaasi kasutada oskaks, tuleb MySQLi konsoolis sisestada veel
		j�rgnevad k�sud:
		CREATE DATABASE jussdb;
		
		GRANT ALL PRIVILEGES
		ON jussdb.*
		TO 'juss'@'localhost'
		IDENTIFIED BY 'goodpassword';
		
	b) Tomcati veebiserver.
		--> http://jakarta.apache.org/site/downloads/downloads_tomcat-5.cgi
		T�mmata suvaline 5.x versioon
		installatsioon ei tohiks raskusi valmistada. Arenduse jaoks pole
		soovitatav, et Tomcat installeeritaks s�steemiprotsessina - parem
		kui see arvuti k�ivitamisel automaatselt t��le ei l�heks. Tomcati
		k�ivitamiseks kasutame j�rgmist Eclipse komponenti
		
	c) Eclipse Tomcati plugin
		--> http://www.sysdeo.com/eclipse/tomcatPlugin.html
		t�mmata 3.1 beetaversioon - tomcatPluginV31beta.zip
		pakkida .zip arhiiv lahti Eclipse pluginate kataloogi - selle asukoht
		s�ltub endastm�istetavalt sellest, kuhu Eclipse installeeritud on,
		minu pluginate kataloog on /usr/lib/eclipse-3/plugins/.
		K�ivitada Eclipse - tekkinud peaks olema Tomcat men�� 3 elemendiga - 
		(Start, Stop, Restart) Tomcat.
		V�tta lahti Window->Preferences->Tomcat ja m��rata seal j�rgnevad
		suvandid:
			1) Tomcat version - 5.x
			2) Tomcat home, kataloog kuhu tomcat installeeriti
			   minul nt /opt/tomcat5
			3) Context declaration mode -> context files
			
	d) mingi cvs-i klient. 
		Eclipse bugi t�ttu > 1.12.9 CVS serveriga suhtlemisel ei ole v�imalik
		Eclipsega CVS checkouti ats.cs.ut.ee-st sooritada. Seet�ttu tuleb
		checkoutiks kasutada midagi muud.
		Standardse k�sureakliendi kasutamiseks tuleb teha j�rgmist.
		Seada keskkonnamuutuja: 
		CVSROOT=:pserver:kasutajanimi@ats.cs.ut.ee:2401/projects/home/tvp2005/cvsroot
		minna kataloogi kuhu soovitakse jussi development treed paigutada
		anda k�sk: 'cvs login' ja sisestada oma parool
		CVSist asjade �ratirimiseks anda k�sk: cvs co oop/juss
		Tekkis kataloog oop/juss, kus hakkabki toimuma arendust��
		
	e) V�tta Eclipses File->New Project-> Java Project -> Next -> create project at
		external location - valida kataloog JUSS kuhu CVSist checkout tehti, panna
		projektile nimeks JUSS -> Klikkida finish
		Teha paremklikk tekkinud projektil Navigatori aknas valida properties
		ja kontrollida et Tomcati suvandid oleksid j�rgmised:
			x is a Tomcat project
			context name /juss
			x mark this context as reloadable
			x redirect context logger to eclipse console (debug option)
			salvestada suvandid
		Teha uuesti paremklikk projektil, valida 
		Tomcat Project -> Update context definition
	f) ... siia tuleb jutt konfist ja sellest et server l�heb t��le
		validest Tomcat->Start
		