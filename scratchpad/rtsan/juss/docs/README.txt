*********************************************************************
                             README
*********************************************************************

Autor(id): Taimo Peelo

viimati muudetud: 29. aprill 2005

Kuidas Eclipse keskkonnas arenduseks JUSSi tööle ajada:

1) JUSSi serveri käivitamiseks Eclipses peavad olema installeeritud 
   a) MySQL andmebaasisüsteem
		--> http://dev.mysql.com/downloads/index.html
		Tõmmata suvaline 4.x versioon.
		installeerida soovitaks vaikeväärtustega ja niimoodi et andmebaas
		arvuti käivitamisel alati tööle tõmmatakse. Selleks et JUSS
		andmebaasi kasutada oskaks, tuleb MySQLi konsoolis sisestada veel
		järgnevad käsud:
		CREATE DATABASE jussdb;
		
		GRANT ALL PRIVILEGES
		ON jussdb.*
		TO 'juss'@'localhost'
		IDENTIFIED BY 'goodpassword';
		
	b) Tomcati veebiserver.
		--> http://jakarta.apache.org/site/downloads/downloads_tomcat-5.cgi
		Tõmmata suvaline 5.x versioon
		installatsioon ei tohiks raskusi valmistada. Arenduse jaoks pole
		soovitatav, et Tomcat installeeritaks süsteemiprotsessina - parem
		kui see arvuti käivitamisel automaatselt tööle ei läheks. Tomcati
		käivitamiseks kasutame järgmist Eclipse komponenti
		
	c) Eclipse Tomcati plugin
		--> http://www.sysdeo.com/eclipse/tomcatPlugin.html
		tõmmata 3.1 beetaversioon - tomcatPluginV31beta.zip
		pakkida .zip arhiiv lahti Eclipse pluginate kataloogi - selle asukoht
		sõltub endastmõistetavalt sellest, kuhu Eclipse installeeritud on,
		minu pluginate kataloog on /usr/lib/eclipse-3/plugins/.
		Käivitada Eclipse - tekkinud peaks olema Tomcat menüü 3 elemendiga - 
		(Start, Stop, Restart) Tomcat.
		Võtta lahti Window->Preferences->Tomcat ja määrata seal järgnevad
		suvandid:
			1) Tomcat version - 5.x
			2) Tomcat home, kataloog kuhu tomcat installeeriti
			   minul nt /opt/tomcat5
			3) Context declaration mode -> context files
			
	d) mingi cvs-i klient. 
		Eclipse bugi tõttu > 1.12.9 CVS serveriga suhtlemisel ei ole võimalik
		Eclipsega CVS checkouti ats.cs.ut.ee-st sooritada. Seetõttu tuleb
		checkoutiks kasutada midagi muud.
		Standardse käsureakliendi kasutamiseks tuleb teha järgmist.
		Seada keskkonnamuutuja: 
		CVSROOT=:pserver:kasutajanimi@ats.cs.ut.ee:2401/projects/home/tvp2005/cvsroot
		minna kataloogi kuhu soovitakse jussi development treed paigutada
		anda käsk: 'cvs login' ja sisestada oma parool
		CVSist asjade äratirimiseks anda käsk: cvs co oop/juss
		Tekkis kataloog oop/juss, kus hakkabki toimuma arendustöö
		
	e) Võtta Eclipses File->New Project-> Java Project -> Next -> create project at
		external location - valida kataloog JUSS kuhu CVSist checkout tehti, panna
		projektile nimeks JUSS -> Klikkida finish
		Teha paremklikk tekkinud projektil Navigatori aknas valida properties
		ja kontrollida et Tomcati suvandid oleksid järgmised:
			x is a Tomcat project
			context name /juss
			x mark this context as reloadable
			x redirect context logger to eclipse console (debug option)
			salvestada suvandid
		Teha uuesti paremklikk projektil, valida 
		Tomcat Project -> Update context definition
	f) ... siia tuleb jutt konfist ja sellest et server läheb tööle
		validest Tomcat->Start
		