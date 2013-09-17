function FindProxyForURL(url, host)
{
if ((shExpMatch(url,"http://proxy.pac.edf.fr")) ||
	(shExpMatch(url,"http://proxypac.edf.fr")) ||
	(shExpMatch(url,"http://proxypacsc.edf.fr")) ||
	(shExpMatch(url,"http://www-config.der.edf.fr")))
		return "PROXY noevipncp2n.edf.fr:3128; PROXY pcyvipncp2n.edf.fr:3128";
else
if ((shExpMatch(url, "https://authpcy.edf.fr/favicon.ico")) ||
	(shExpMatch(url, "https://authcla.edf.fr/favicon.ico")) ||
	(shExpMatch(url, "https://authnoe.edf.fr/favicon.ico")) ||
	(shExpMatch(url, "http://redirauth.edfgdf.fr/favicon.ico")))
		return "PROXY 10.200.53.18:3128";
else
if ((shExpMatch(host,"redirauth.edfgdf.fr")) || (shExpMatch(host,"redirauth2.edfgdf.fr")))
		return "PROXY noevipncp2n.edf.fr:3128; PROXY pcyvipncp2n.edf.fr:3128";
else
if ((shExpMatch(host,"authcla.edf.fr")) ||
	(shExpMatch(host,"authpcy.edf.fr")) ||
	(shExpMatch(host,"authnoe.edf.fr")))
		return "DIRECT";
else
if (	shExpMatch(host, "91.121.25.82") ||
	shExpMatch(url, "http://erdfradio.ice.infomaniak.ch/erdfradio-32.mp3") ||
                             shExpMatch(url, "http://erdfradio.ice.infomaniak.ch/erdfradio-32.mp3?*") ||
		  shExpMatch(url, "http://edf.ice.infomaniak.ch/edf.mp3") ||
                                                             shExpMatch(url, "http://edf.ice.infomaniak.ch/edf.mp3?*")
	)
	return "PROXY proxytv.edf.fr:3128";
else
        	if (shExpMatch(host, "*.wank.prod.linky.distribution.edf.fr"))
                	return "PROXY wank-proxy.prod.linky.distribution.edf.fr:3128";
else
if (	shExpMatch(host, "192.168.170.71") ||
	shExpMatch(host, "nedstat.edf.fr") ||
	isInNet(host, "192.168.67.0","255.255.255.0") ||
	isInNet(host, "192.168.68.0","255.255.255.0") ||
	isInNet(host, "192.168.69.0","255.255.255.0") ||
	isInNet(host, "192.168.70.0","255.255.255.0") ||
	isInNet(host, "192.168.71.0","255.255.255.0") ||
	shExpMatch(host, "intranet.edfenergy.com") ||
	dnsDomainIs(host, ".edfenergy.com") ||
	shExpMatch(host, "www.inpo.org") ||
	shExpMatch(host, "www.wano.org"))
	return "PROXY pcyvipncp2n.edf.fr:3128";
else
if (	shExpMatch(host, "192.168.110.251")||
	shExpMatch(host, "edisign.visionitgroup.fr"))
	return "PROXY clavipncp2n.edf.fr:3128";
else 	
if (	shExpMatch(host, "c2c1-phoenix.edf.fr")||
	shExpMatch(host, "c2c2-phoenix.edf.fr"))
	return "DIRECT";
else
if( !isResolvable( host ))
	return "DIRECT";
else
	if (shExpMatch(host, "horizon.gdfsuez.net") ||
	shExpMatch(host, "directory.gdfsuez.net") ||
	shExpMatch(host, "gdfsuez.net") ||
	shExpMatch(host, "watch.gdfsuez.net") ||
	shExpMatch(host, "directory.group.suez") ||
	shExpMatch(host, "dirsearch.gdfsuez.net") ||
	shExpMatch(host, "was1.gdfsuez.net") ||
	shExpMatch(host, "infinity.group.suez") ||
	shExpMatch(host, "recruiters.more.suez") ||
	shExpMatch(host, "horizon.group.suez") ||
	shExpMatch(host, "login.group.suez") ||
	shExpMatch(host, "*.mc2.gdfsuez.net") ||
	shExpMatch(host, "loginssc.gdfsuez.net") ||
	shExpMatch(host, "integration-anatole.grdf.gdfsuez.net") ||
	shExpMatch(host, "erapsodie.gdfsuez.net") ||
	shExpMatch(host, "anatole.grdf.net") ||
	shExpMatch(host, "anatole.gazdefrance.com") ||
	shExpMatch(host, "distri.gazdefrance.com") ||
	shExpMatch(host, "tech.grdf.net") ||
	shExpMatch(host, "anatole-ntlm.gdfsuez.net") ||
	shExpMatch(host, "acceptance-anatole.grdf.net") ||
	shExpMatch(host, "acceptance-tech.grdf.net") ||
	shExpMatch(host, "acceptance-anatole-ntlm.grdf.net") ||
	shExpMatch(host, "training-anatole-ntlm.gdfsuez.net") ||
	shExpMatch(host, "training-tech.grdf.net") ||
	shExpMatch(host, "training-anatole.grdf.net") ||
	shExpMatch(host, "reseauxcom.grdf.gdfsuez.net") ||
	shExpMatch(host, "athena.gdfsuez.net") ||
	shExpMatch(host, "athena-acc.gdfsuez.net") ||
	shExpMatch(host, "collaboration.gdfsuez.net") ||
   	shExpMatch(host, "smile.group.suez"))
   	return "PROXY 10.249.20.10:3128";
else
	if (shExpMatch(host, "acceptance-anatole.grdf.net") ||
   	shExpMatch(host, "acceptance-tech.grdf.net") ||
	shExpMatch(host, "acceptance-anatole-ntlm.grdf.net") ||
	shExpMatch(host, "acceptance-training-anatole.grdf.net") ||
	shExpMatch(host, "acceptance-training-anatole-ntlm.grdf.net"))
	return "PROXY 10.249.20.37:3128";
else
	if (shExpMatch(host, "192.196.121.143"))
		return "PROXY 192.196.121.143:3128";
else
	if (dnsDomainIs(host, ".whenu.com") ||
	dnsDomainIs(host, ".eorezo.com") ||
	shExpMatch(host, "eorezo.com") ||
	shExpMatch(host, "www.jraun.com") ||
	shExpMatch(host, "pluto.jraun.com") ||
	shExpMatch(host, "www.commonname.com") ||
	shExpMatch(host, "akapp.whenu.com") ||
	shExpMatch(host, "stats.virtuaguy.com") ||
	shExpMatch(host, "stats.virtuagirl.com") ||
	shExpMatch(host, "gatorcme.gator.com") ||
	shExpMatch(host, "www.internet-optimizer.com") ||
	shExpMatch(host, "update.thunderdownloads.com") ||
	shExpMatch(host, "ping.180solutions.com") ||
	shExpMatch(host, "bis.180solutions.com") ||
	shExpMatch(host, "www.openwares.org") ||
	shExpMatch(host, "www.hooping.org") ||
	shExpMatch(host, "www.imogenheap.co.uk") ||
	shExpMatch(host, "www.newgenerationcomics.net") ||
	shExpMatch(host, "www.aartanridge.org.uk") ||
	shExpMatch(host, "utils.winantivirus.com") ||
	shExpMatch(host, "www.eastcoastchoons.co.uk") ||
	shExpMatch(host, "www.foxalpha.com") ||
	shExpMatch(host, "www.sundayriders.co.uk") ||
	shExpMatch(host, "conyc.com") ||
	shExpMatch(url, "http://swupmf.adobe.com/manifest/50/win/AdobeUpdater.upd") ||
	shExpMatch(url, "http://swupmf.adobe.com/manifest/50/win/reader8rdr-fr_FR.upd") ||
	shExpMatch(host, "sb.google.com") ||
	shExpMatch(host, "dm.mlstat.com") ||
	shExpMatch(host, "www.ribaforada.net"))
	return "PROXY 127.0.0.1:3128";
else
	if (shExpMatch(host, "w3.gazdefrance.com:100") ||
	shExpMatch(host, "gaznet*.gazdefrance.com") ||
	shExpMatch(host, "w3.gazdefrance.com") ||
	shExpMatch(host, "w3.gazdefrance.com:8080") ||
	shExpMatch(host, "da.gaznet.gazdefrance.com") ||
	shExpMatch(host, "wp.gaznet.gazdefrance.com") ||
	shExpMatch(host, "interne.gaznet.gazdefrance.com") ||
	shExpMatch(host, "modulation.gazdefrance.com") ||
	shExpMatch(host, "www.giignl.org") ||
	shExpMatch(host, "e.gazdefrance.com") ||
	shExpMatch(host, "count.gazdefrance.com") ||
	shExpMatch(host, "travauxponctuels.com") ||
	shExpMatch(host, "eprogrammes.gazdefrance.com") ||
	shExpMatch(host, "admin.gazdefrance.com") ||
	shExpMatch(host, "www.climogaz.com") ||
	shExpMatch(host, "www.giga-radar.info") ||
	shExpMatch(host, "www.ladg-france.com") ||
	shExpMatch(host, "innovation.gazdefrance.com") ||
	shExpMatch(host, "mediatheque.gazdefrance.com") ||
	shExpMatch(host, "ep.gazdefrance.com") ||
	shExpMatch(host, "progres-spp.gazdefrance.com") ||
	shExpMatch(host, "entraide-handicap.asso.fr") ||
	shExpMatch(host, "negoce.gazdefrance.com") ||
	shExpMatch(host, "international-distrigaz.gazdefrance.com") ||
	shExpMatch(host, "international-distribution.gazdefrance.com") ||
	shExpMatch(host, "gazcom.gazdefrance.com") ||
	shExpMatch(host, "international.gazdefrance.com") ||
   	shExpMatch(host, "https://riis.gdfsuez.net") ||
	shExpMatch(host, "projet-pmv.gazdegrance.com") ||
	shExpMatch(host, "travauxponctuels.com"))
	return "PROXY 148.143.101.36:3128";
else
        if (
        shExpMatch(host, "panthere.c-s.fr") ||
        shExpMatch(host, "panthere.edf.fr") ||
        shExpMatch(host, "integration-transport.gazdefrance.com") ||
        shExpMatch(host, "administration-transport.gazdefrance.com") ||
        shExpMatch(host, "integration-distribution.gazdefrance.com") ||
        shExpMatch(host, "integration-administration.gazdefrance.com") ||
        shExpMatch(host, "administration.gazdefrance.com") ||
        shExpMatch(host, "www.gazdefrance-distribution.com") ||
        shExpMatch(host, "gazdefrance-distribution.com") ||
        shExpMatch(host, "distribution.gazdefrance.com") ||
        shExpMatch(host, "transport.gazdefrance.com") ||
        shExpMatch(host, "www.transport.gazdefrance.com") ||
        shExpMatch(host, "81.28.99.197") ||
        shExpMatch(host, "intervenants.grandlyon.org") ||
        shExpMatch(host, "sv3.eu.ipsos.com") ||
        shExpMatch(host, "colt3.audientia.net") ||
        shExpMatch(host, "ademe.gazdefrance.com") ||
        shExpMatch(host, "www.edfparticuliers.fr") ||
        dnsDomainIs(host, "particuliers.edf.fr") ||
        shExpMatch(host, "service.meteorage.com") ||
        shExpMatch(host, "www.meteorage.fr") ||
        shExpMatch(host, "catphoto.edfgdf.hubwoo.com") ||
        shExpMatch(host, "catphoto.hubwoo.com") ||
        shExpMatch(host, "cflols.int.hubwoo.com") ||
        shExpMatch(host, "printperso.hubwoo.net") ||
        shExpMatch(host, "demo.hwo.hubwoo.com") ||
        shExpMatch(host, "p102x1.hwo.hubwoo.com") ||
        shExpMatch(host, "hubxml.hwo.hubwoo.com") ||
        shExpMatch(host, "hubxml.hubwoo.com") ||
        shExpMatch(host, "hubxml.int.hubwoo.com") ||
        shExpMatch(host, "demo.hwo.hubwoo.com") ||
        shExpMatch(host, "demo.hubwoo.com") ||
        shExpMatch(host, "www.edf.fr") ||
        shExpMatch(host, "entreprises.edf.fr") ||
        shExpMatch(host, "www.edf.com") ||
        shExpMatch(host, "www.edfpro.fr") ||
        shExpMatch(host, "www.edfpro.com") ||
        shExpMatch(host, "www.edf-pro.fr") ||
        shExpMatch(host, "www.edf-pro.com") ||
        shExpMatch(host, "www.edf-pros.fr") ||
        shExpMatch(host, "www.edf-pros.com") ||
        shExpMatch(host, "www.pro.edf.fr") ||
        shExpMatch(host, "www.electricitedefrance.fr") ||
        shExpMatch(host, "www.electricite-de-france.fr") ||
        shExpMatch(host, "capacityauctions.edf.com") ||
        shExpMatch(host, "encherescapacite.edf.com") ||
        shExpMatch(host, "pilotimmo.edf.fr") ||
        shExpMatch(host, "idfn-pilotimmo.edf.fr") ||
        shExpMatch(host, "*-pilotimmo.edf.fr") ||
        shExpMatch(host, "convention2003*.edf.fr") ||
        shExpMatch(host, "espace-elec.edf.fr") ||
        shExpMatch(host, "direct-residentiel.edf.fr") ||
        shExpMatch(host, "diirect-riobravo2.edf.fr") ||
        shExpMatch(host, "asiapacific.edf.fr") ||
        shExpMatch(host, "page.edf.fr") ||
        shExpMatch(host, "finance.edf.fr") ||
        shExpMatch(host, "corporateuniversity-elearning.edf.fr") ||
        shExpMatch(host, "ard.edf.fr") ||
        shExpMatch(host, "capacityauctions.edf.fr") ||
        shExpMatch(host, "cl.edf.fr") ||
        shExpMatch(host, "www.edf-collectivites.com") ||
        shExpMatch(host, "collectivite.edf.fr") ||
        shExpMatch(host, "collectivites.edf.fr") ||
        shExpMatch(host, "collectivites-locales.edf.fr") ||
        shExpMatch(host, "developpement-durable.edf.fr") ||
        shExpMatch(host, "encherescapacite.edf.fr") ||
        shExpMatch(host, "grandesentreprises.edf.fr") ||
        shExpMatch(host, "backoffice.edf.com") ||
        shExpMatch(host, "etrebienchezsoi.edf.fr") ||
        shExpMatch(host, "nucleaire.edf.fr") ||
        shExpMatch(host, "presse.edf.fr") ||
        shExpMatch(host, "rdsoft.edf.fr") ||
        shExpMatch(host, "retd.edf.fr") ||
        shExpMatch(host, "www.rd.edf.fr") ||
        shExpMatch(host, "crl.edf.fr") ||
        shExpMatch(host, "fournisseurs.edf.fr") ||
        shExpMatch(host, "serveurdetestpki.edf.fr") ||
        dnsDomainIs(host, ".exp.edf.fr") ||
        shExpMatch(host, "extranet.edfgdf.hubwoo.com") ||
        shExpMatch(host, "punchout.edfgdf.int.hubwoo.com") ||
        shExpMatch(host, "punchout.edfgdf.hubwoo.com") ||
        shExpMatch(host, "dauphin.edfgdf.int.hubwoo.com") ||
        shExpMatch(host, "dauphin.edfgdf.hubwoo.com") ||
        shExpMatch(host, "dauphin-dwh.edfgdf.int.hubwoo.com") ||
        shExpMatch(host, "dauphin-dwh.edfgdf.hubwoo.com") ||
        shExpMatch(host, "hubxml.edfgdf.dev.hubwoo.com") ||
        shExpMatch(host, "hubxml.edfgdf.int.hubwoo.com") ||
        shExpMatch(host, "hubxml.edfgdf.hubwoo.com") ||
        shExpMatch(host, "www.wano.org") ||
        shExpMatch(host, "www.inpo.org") ||
        dnsDomainIs(host, "opengazdefrance.com") ||
        dnsDomainIs(host, "opengazdefrance.fr") ||
        shExpMatch(host, "tennisgazdefrance.fr") ||
        shExpMatch(host, "www.dolcevita.gazdefrance.fr") ||
        shExpMatch(host, "tennisgazdefrance.com") ||
        dnsDomainIs(host, "tennis.gazdefrance.fr") ||
        dnsDomainIs(host, "tennis.gazdefrance.com") ||
        dnsDomainIs(host, "meetinggazdefrance.fr") ||
        dnsDomainIs(host, "meetinggazdefrance.com") ||
        dnsDomainIs(host, "meeting.gazdefrance.com") ||
        dnsDomainIs(host, "meeting.gazdefrance.fr") ||
        dnsDomainIs(host, "athle.gazdefrance.com") ||
        dnsDomainIs(host, "athle.gazdefrance.fr") ||
        dnsDomainIs(host, "athletisme.gazdefrance.fr") ||
        dnsDomainIs(host, "athletismegazdefrance.fr") ||
        dnsDomainIs(host, "athletismegazdefrance.com") ||
        shExpMatch(host, "www.iuk-isis.co.uk") ||
        shExpMatch(host, "www.micropatent.com") ||
        shExpMatch(host, "gemini-ix-prod-citrix.transco.co.uk") ||
        shExpMatch(host, "gemini-ship-citrix.transco.co.uk") ||
        shExpMatch(host, "gemini-ix-prod-citrix-dr.transco.co.uk") ||
        shExpMatch(host, "gemini-ix-prod-sg.transco.co.uk") ||
        shExpMatch(host, "gemini-ix-prod.transco.co.uk") ||
        shExpMatch(host, "gemini-ix-trial.transco.co.uk") ||
        shExpMatch(host, "gemini-ship.transco.co.uk") ||
        shExpMatch(host, "ors-reporting.edf.fr") ||
        shExpMatch(host, "victoiresducommerce.preprod.proximity.fr") ||
        shExpMatch(host, "victoiresducommerce.edf.fr") ||
        shExpMatch(host, "www.edftravaux.bravosolution.fr") ||
        shExpMatch(host, "www.bravobuild.fr") ||
        shExpMatch(host, "www.test.portail.achats.edf.bravosolution.fr") ||
        shExpMatch(host, "www.edfdistribution.fr") ||
        shExpMatch(host, "edfdistribution.fr") ||
        shExpMatch(host, "sei.edf.fr") ||
        shExpMatch(host, "prestataires-nucleaire.edf.fr") ||
        shExpMatch(host, "prestataires-nucleaire.edf.com") ||
        shExpMatch(host, "deveniractionnaire.edf.com") ||
        shExpMatch(host, "devenir-actionnaire.edf.com") ||
        shExpMatch(host, "actionnaires.edf.com") ||
        shExpMatch(host, "www.actionnaires.edf.com") ||
        shExpMatch(host, "www.clubactionnaires.edf.com") ||
        shExpMatch(host, "clubactionnaires.edf.com") ||
        shExpMatch(host, "clubactionnaire.edf.com") ||
        shExpMatch(host, "esourds.edf.fr") ||
        shExpMatch(host, "esourdscontact.edf.fr") ||
        shExpMatch(host, "esourdsconseiller.edf.fr") ||
        shExpMatch(host, "geo.edf.com") ||
        shExpMatch(host, "photos.cnpe-stlaurent.com") ||
        shExpMatch(host, "extranetclient.profils.net") ||
        shExpMatch(host, "meeting-gdf.e-styler.com") ||
        shExpMatch(host, "inactifs-ors.edf.fr") ||
        shExpMatch(host, "www.rte-france.com")||
        shExpMatch(host, "www4.commerce.reuters.com")||
        shExpMatch(host, "www.gva.rapid.reuters.com")||
        shExpMatch(host, "www.edfentreprises.net")||
        shExpMatch(host, "planetcommercial-blog.edf.fr")||
        shExpMatch(host, "rencontres2006.gazdefrance.com")||
        shExpMatch(host, "enquete.gazdefrance.com")||
        shExpMatch(host, "213.215.157.56") ||
        shExpMatch(host, "213.215.157.57") ||
        shExpMatch(host, "213.215.157.58") ||
        shExpMatch(host, "prp.edf.com") ||
        dnsDomainIs(host, ".edf.bravosolution.fr") ||
        dnsDomainIs(host, ".rte.gmessaging.net") ||
        dnsDomainIs(host, ".edfentreprises.net") ||
        shExpMatch(host, "transports.edf.fr")||
        shExpMatch(host, "lespacephoto.gazdefrance.com")||
        shExpMatch(host, "laboutique.gazdefrance.com")||
        shExpMatch(host, "panorama.edf.com")||
        shExpMatch(host, "panorama-electricite.edf.com")||
        shExpMatch(host, "cga.edf.com")||
        shExpMatch(host, "tempo.edf.fr")||
        shExpMatch(host, "www.tempo.tm.fr")||
        shExpMatch(host, "test.tempo.tm.fr")||
        shExpMatch(host, "www.trophees2007.edf-group.net")||
        shExpMatch(host, "www.2007trophies.edf-group.net")||
        shExpMatch(host, "progresser-ensemble.edf-group.net")||
        shExpMatch(host, "excellence-operationnelle.edf-group.net")||
        shExpMatch(host, "planetcommercial-tournedpp.edf.fr")||
        shExpMatch(host, "ina.edf.com")||
        shExpMatch(host, "lanouvelledonne2007.edf.fr")||
        shExpMatch(host, "edf.orphea.com")||
        shExpMatch(host, "mediatheque.edf.fr")||
        shExpMatch(host, "mediatheque.edf.com")||
        shExpMatch(host, "partenaires.edf.fr")||
        shExpMatch(host, "fondation.edf.com")||
        shExpMatch(host, "sport.edf.com")||
        shExpMatch(host, "www.sport.edf.com")||
        shExpMatch(host, "sports.edf.com")||
        shExpMatch(host, "www.sports.edf.com")||
        shExpMatch(host, "recrutement-thermique.edf.com")||
        shExpMatch(host, "apprentis.edf.fr")||
        shExpMatch(host, "www.edf-eboutique.com")||
        shExpMatch(host, "investisseurs.edf.com")||
        shExpMatch(host, "www.5-minutes-avec-edf.edf.fr")||
        shExpMatch(host, "recruit-optimisationandtrading.edf.com")||
        shExpMatch(host, "recrutement-informatiqueettelecom.edf.com")||
        shExpMatch(host, "preprod-suiviconso.edf.fr")||
        shExpMatch(host, "suiviconso.edf.fr")||
        shExpMatch(host, "suiviconso.edf.com")||
        shExpMatch(host, "suivi-conso.edf.fr")||
        shExpMatch(host, "suivi-conso.edf.com")||
        shExpMatch(host, "www.suiviconso.edf.fr")||
        shExpMatch(host, "www.suiviconso.edf.com")||
        shExpMatch(host, "www.suivi-conso.edf.fr")||
        shExpMatch(host, "www.suivi-conso.edf.com")||
        shExpMatch(host, "jeufacture.edf.fr")||
        shExpMatch(host, "rd.edf.com")||
        dnsDomainIs(host, "masecondemaison.fr")||
        dnsDomainIs(host, "masecondemaison.com")||
        shExpMatch(host, "www.ma-seconde-maison.fr")||
        shExpMatch(host, "www.ma-seconde-maison.com")||
        shExpMatch(host, "www.ma2ndemaison.fr")||
        shExpMatch(host, "www.ma-2nde-maison.fr")||
        shExpMatch(host, "www.ma2ndemaison.com")||
        shExpMatch(host, "www.ma-2nde-maison.com")||
        shExpMatch(host, "m2m.edf.fr")||
        shExpMatch(host, "ma2maison.edf.fr")||
        shExpMatch(host, "masecondemaison.edf.fr")||
        shExpMatch(host, "convention-act2007.edf.com")||
        shExpMatch(host, "www.convention-act2007.edf.com")||
        shExpMatch(host, "eegalmoinsdeco2.be")||
        shExpMatch(host, "eegalmoinsdeco2.biz")||
        shExpMatch(host, "eegalmoinsdeco2.com")||
        shExpMatch(host, "eegalmoinsdeco2.eu")||
        shExpMatch(host, "eegalmoinsdeco2.fr")||
        shExpMatch(host, "eegalmoinsdeco2.info")||
        shExpMatch(host, "eegalmoinsdeco2.name")||
        shExpMatch(host, "eegalmoinsdeco2.net")||
        shExpMatch(host, "eegalmoinsdeco2.org")||
        shExpMatch(host, "www.eegalmoinsdeco2.be")||
        shExpMatch(host, "www.eegalmoinsdeco2.biz")||
        shExpMatch(host, "www.eegalmoinsdeco2.com")||
        shExpMatch(host, "www.eegalmoinsdeco2.eu")||
        shExpMatch(host, "www.eegalmoinsdeco2.fr")||
        shExpMatch(host, "www.eegalmoinsdeco2.info")||
        shExpMatch(host, "www.eegalmoinsdeco2.name")||
        shExpMatch(host, "www.eegalmoinsdeco2.net")||
        shExpMatch(host, "www.eegalmoinsdeco2.org")||
        shExpMatch(host, "emoinsdeco2.be")||
        shExpMatch(host, "emoinsdeco2.biz")||
        shExpMatch(host, "emoinsdeco2.com")||
        shExpMatch(host, "emoinsdeco2.eu")||
        shExpMatch(host, "emoinsdeco2.fr")||
        shExpMatch(host, "emoinsdeco2.name")||
        shExpMatch(host, "emoinsdeco2.net")||
        shExpMatch(host, "www.emoinsdeco2.be")||
        shExpMatch(host, "www.emoinsdeco2.biz")||
        shExpMatch(host, "www.emoinsdeco2.com")||
        shExpMatch(host, "www.emoinsdeco2.eu")||
        shExpMatch(host, "www.emoinsdeco2.fr")||
        shExpMatch(host, "www.emoinsdeco2.name")||
        shExpMatch(host, "www.emoinsdeco2.net")||
        shExpMatch(host, "moinsdeco2.be")||
        shExpMatch(host, "moinsdeco2.biz")||
        shExpMatch(host, "moinsdeco2.com")||
        shExpMatch(host, "moinsdeco2.eu")||
        shExpMatch(host, "moinsdeco2.fr")||
        shExpMatch(host, "moinsdeco2.info")||
        shExpMatch(host, "moinsdeco2.name")||
        shExpMatch(host, "moinsdeco2.net")||
        shExpMatch(host, "moinsdeco2.org")||
        shExpMatch(host, "www.moinsdeco2.be")||
        shExpMatch(host, "www.moinsdeco2.biz")||
        shExpMatch(host, "www.moinsdeco2.com")||
        shExpMatch(host, "www.moinsdeco2.eu")||
        shExpMatch(host, "www.moinsdeco2.fr")||
        shExpMatch(host, "www.moinsdeco2.info")||
        shExpMatch(host, "www.moinsdeco2.name")||
        shExpMatch(host, "www.moinsdeco2.net")||
        shExpMatch(host, "www.moinsdeco2.org")||
        shExpMatch(host, "energies.edf.com")||
        shExpMatch(host, "edf-editions.staci.com")||
        shExpMatch(host, "webcat.staci.com")||
        shExpMatch(host, "photovideo.edf.com")||
        shExpMatch(host, "www.edf-bleuciel.fr")||
        shExpMatch(host, "www.edfbleuciel.fr")||
        shExpMatch(host, "edfbleuciel.fr")||
        shExpMatch(host, "wap.edfbleuciel.fr")||
        shExpMatch(host, "developpement-durable.edf.com")||
        shExpMatch(host, "jeunes.edf.com")||
        shExpMatch(host, "enseignants.edf.com")||
        shExpMatch(host, "design.edf.com")||
        shExpMatch(host, "demo-reno.edf.fr")||
        shExpMatch(host, "demo-immo.edf.fr")||
        shExpMatch(host, "identite-groupe.edf.com")||
        dnsDomainIs(host, "hubdoaat.fr")||
        dnsDomainIs(host, "hubdoaat.net")||
        dnsDomainIs(host, "hubdoaat.eu")||
        shExpMatch(host, "planetcommercial-animation.edf.fr")||
        shExpMatch(host, "www.gazdefrance.com")||
        shExpMatch(host, "www.preprod.gazdefrance.com")||
        shExpMatch(host, "org-www.gazdefrance.com")||
        shExpMatch(host, "org-www.preprod.gazdefrance.com")||
        shExpMatch(host, "www.erdfdistribution.fr")||
        shExpMatch(host, "ors2008-conventions.edf-group.net")||
        shExpMatch(host, "i-nova.gazdefrance.com")||
        shExpMatch(host, "statistiques.edf.com")||
        shExpMatch(host, "service-public.edf.com")||
        shExpMatch(host, "presse.edf.com")||
        shExpMatch(host, "groupe.edf.com")||
        shExpMatch(host, "international.edf.com")||
        shExpMatch(host, "rh.edf.com")||
        shExpMatch(host, "regions.edf.com")||
        shExpMatch(host, "multimedia.edf.com")||
        shExpMatch(host, "magazine.edf.com")||
        shExpMatch(host, "refxdev.edf.fr")||
        shExpMatch(host, "www.erdf.fr")||
        shExpMatch(host, "www.eboutiqueedf.com")||
        shExpMatch(host, "www.eboutiqueedf.fr")||
        shExpMatch(host, "edfasie.com")||
        shExpMatch(host, "www.edfasie.com")||
        shExpMatch(host, "edf-asie.com")||
        shExpMatch(host, "www.edf-asie.com")||
        shExpMatch(host, "edf-asia.com")||
        shExpMatch(host, "www.edf-asia.com")||
        shExpMatch(host, "edfgroup.asia")||
        shExpMatch(host, "www.edfgroup.asia")||
        shExpMatch(host, "edf.asia")||
        shExpMatch(host, "www.edf.asia")||
        shExpMatch(host, "ors-retraites.edf.com")||
        shExpMatch(host, "retraites-ors.edf.com")||
        shExpMatch(host, "achat-photovoltaique.edf.fr")||
        shExpMatch(host, "nedstat.edf.fr")||
        shExpMatch(host, "gazdefrance.com")||
        shExpMatch(host, "fondation.gazdefrance.com")||
        shExpMatch(host, "developpementdurable.gazdefrance.com")||
        shExpMatch(host, "presse.gazdefrance.com")||
        shExpMatch(host, "fournisseurs.gazdefrance.com")||
        shExpMatch(host, "recrutement.gazdefrance.com")||
        shExpMatch(host, "erecrutement.gazdefrance.com")||
        shExpMatch(host, "www.fondation.gazdefrance.com")||
        shExpMatch(host, "www.developpementdurable.gazdefrance.com")||
        shExpMatch(host, "www.presse.gazdefrance.com")||
        shExpMatch(host, "www.fournisseurs.gazdefrance.com")||
        shExpMatch(host, "www.recrutement.gazdefrance.com")||
        shExpMatch(host, "www.erecrutement.gazdefrance.com")||
        shExpMatch(host, "enqueteenligne.bleuciel.edf.fr")||
        shExpMatch(host, "enqueteenligne.edfpro.edf.fr")||
        shExpMatch(host, "enqueteenligne.monespaceclient.edf.fr")||
        shExpMatch(host, "enqueteenligne.agenceedfpro.edf.fr")||
        dnsDomainIs(host, "refxdev-vpn.edf.fr")||
        dnsDomainIs(host, "refxrec.edf.fr")||
        dnsDomainIs(host, "refxpreprod.edf.fr")||
        dnsDomainIs(host, "agil.edf.fr")||
        shExpMatch(host, "www.grdf.fr")||
        shExpMatch(host, "edf.transatvideo.com")||
        shExpMatch(host, "www.archi-co2.fr")||
        shExpMatch(host, "archi-co2.fr")||
        shExpMatch(host, "www.archi-co2.com")||
        shExpMatch(host, "archi-co2.com")||
        shExpMatch(host, "enqueteenligne.bleuciel.edf.fr")||
        shExpMatch(host, "enqueteenligne.edfpro.edf.fr")||
        shExpMatch(host, "enqueteenligne.monespaceclient.edf.fr")||
        shExpMatch(host, "enqueteenligne.agenceedfpro.edf.fr")||
        shExpMatch(host, "www.edfenr.com")||
        shExpMatch(host, "edfenr.com")||
        shExpMatch(host, "www.energiesdedemain.com")||
        shExpMatch(host, "energiesdedemain.com")||
        shExpMatch(host, "www.energiesdedemain.eu")||
        shExpMatch(host, "energiesdedemain.eu")||
        shExpMatch(host, "www.energiesdedemain.fr")||
        shExpMatch(host, "energiesdedemain.fr")||
        shExpMatch(host, "www.tomorrowsenergies.fr")||
        shExpMatch(host, "tomorrowsenergies.fr")||
        shExpMatch(host, "www.tomorrowsenergies.eu")||
        shExpMatch(host, "tomorrowsenergies.eu")||
        shExpMatch(host, "www.fepledd.com")||
        shExpMatch(host, "fepledd.com")||
        shExpMatch(host, "www.fepledd.fr")||
        shExpMatch(host, "fepledd.fr")||
        shExpMatch(host, "www.fepledd.eu")||
        shExpMatch(host, "fepledd.eu")||
        shExpMatch(host, "www.formationingenieur.com")||
        shExpMatch(host, "formationingenieur.com")||
        shExpMatch(host, "www.formationnucleaire.com")||
        shExpMatch(host, "formationnucleaire.com")||
        shExpMatch(host, "www.formationnucleaire.eu")||
        shExpMatch(host, "formationnucleaire.eu")||
        shExpMatch(host, "www.formationnucleaire.fr")||
        shExpMatch(host, "formationnucleaire.fr")||
        shExpMatch(host, "www.formationenergie.com")||
        shExpMatch(host, "formationenergie.com")||
        shExpMatch(host, "www.formationenergie.fr")||
        shExpMatch(host, "formationenergie.fr")||
        shExpMatch(host, "publicite.edf.com")||
        shExpMatch(host, "advertising.edf.com")||
        shExpMatch(host, "www.lenergiecreative.fr")||
        shExpMatch(host, "planetcommercial-album.edf.fr")||
        shExpMatch(host, "enquete.edf.fr")||
        dnsDomainIs(host, "mamaisonbleucieledf.com")||
        dnsDomainIs(host, "mamaisonbleucieledf.fr")||
        shExpMatch(host, "intranet.edfenergy.com")||
        shExpMatch(host, "galsn1.edfbleuciel.phonevalley.com")||
        shExpMatch(host, "217.19.57.216")||
        shExpMatch(host, "ws-simm.prod.edelia.fr")||
        shExpMatch(host, "192.196.77.110")||
        shExpMatch(host, "192.196.77.109")||
        shExpMatch(host, "192.168.110.251")||
        shExpMatch(host, "monagenceroutage.edf.fr")||
        shExpMatch(host, "monagence.edf.fr")||
        shExpMatch(host, "exposition.edf.com")||
        shExpMatch(host, "mediacenter.gdfsuez.com")||
        shExpMatch(host, "gdfsuez.com")||
        shExpMatch(host, "*forumdesvaleurs.gdfsuez.com")||
        shExpMatch(host, "testemark.gdfsuez.com")||
        shExpMatch(host, "martinique.edf.fr")||
        shExpMatch(host, "guadeloupe.edf.fr")||
        shExpMatch(host, "la-reunion.edf.fr")||
        shExpMatch(host, "reunion.edf.fr")||
        shExpMatch(host, "guyane.edf.fr")||
        shExpMatch(host, "st-pierre-miquelon.edf.fr")||
        shExpMatch(host, "saint-pierre-et-miquelon.edf.fr")||
        shExpMatch(host, "corse.edf.fr")||
        shExpMatch(host, "www.gdfsuez.com")||
        shExpMatch(host, "www.monespace-edfenr.com")||
        dnsDomainIs(host, "edf-actionnariat-salarie.com")||
        shExpMatch(host, "minibadge.edfentreprises.net")||
        shExpMatch(host, "loys.edfentreprises.net")||
        shExpMatch(host, "essai.edfentreprises.net")||
        shExpMatch(host, "achacunsongeste.edfentreprises.net")||
        shExpMatch(host, "energiesphere.edf.com")||
        shExpMatch(host, "tests.formergaz.gdfsuez.com")||
        shExpMatch(host, "formergaz.gdfsuez.com")||
        shExpMatch(host, "grand-jeu.edf-bleuciel.fr")||
        shExpMatch(host, "www.code-saturne.org")||
        shExpMatch(host, "www.code-saturne.fr")||
        shExpMatch(host, "www.code-saturne.com")||
        shExpMatch(host, "www.code-saturne.net")||
        shExpMatch(host, "www.codesaturne.org")||
        shExpMatch(host, "www.codesaturne.fr")||
        shExpMatch(host, "www.codesaturne.com")||
        shExpMatch(host, "www.codesaturne.net")||
        shExpMatch(host, "cacheenergiesphere.edf.com")||
        shExpMatch(host, "diagnostic-edf-bleuciel.fr")||
        shExpMatch(host, "europeancustomers.edf-group.eu")||
        shExpMatch(host, "blogsports.edf.com")||
        shExpMatch(host, "iwfm-int.edf.fr")||
        shExpMatch(host, "ras.silicomp.fr")||
        shExpMatch(host, "exercicecrise.edf.com")||
        shExpMatch(host, "exercice-crise.edf.com")||
        shExpMatch(host, "catalogue-ws.edfentreprises.net")||
        shExpMatch(host, "catalogue_ws.edfentreprises.net")||
        shExpMatch(host, "voeux.edf.com")||
        shExpMatch(host, "widget.edfentreprises.net")||
        shExpMatch(host, "meter.edfentreprises.net")||
        shExpMatch(host, "foundation.edf.com")||
        shExpMatch(host, "suppliers.edf.com")||
        shExpMatch(host, "public-service.edf.com")||
        shExpMatch(host, "press.edf.com")||
        shExpMatch(host, "group.edf.com")||
        shExpMatch(host, "int.edf.com")||
        shExpMatch(host, "hr.edf.com")||
        shExpMatch(host, "area.edf.com")||
        shExpMatch(host, "gallery.edf.com")||
        shExpMatch(host, "investors.edf.com")||
        shExpMatch(host, "research.edf.com")||
        shExpMatch(host, "encherescapacites.edf.com")||
        shExpMatch(host, "energy.edf.com")||
        shExpMatch(host, "sport.edf.com")||
        shExpMatch(host, "belgium.edf.com")||
        shExpMatch(host, "iberique.edf.com")||
        shExpMatch(host, "italie.edf.com")||
        shExpMatch(host, "ameriquedunord.edf.com")||
        shExpMatch(host, "asie.edf.com")||
        shExpMatch(host, "asia.edf.com")||
        shExpMatch(host, "yazhou.edf.com")||
        shExpMatch(host, "magasine.edf.com")||
        shExpMatch(host, "www.trophees2009.edf-group.net")||
        shExpMatch(host, "www.2009trophies.edf-group.net")||
        shExpMatch(host, "cachejeunes.edf.com")||
        shExpMatch(host, "edfrecrute.com")||
        shExpMatch(host, "recherche.edf-bleuciel.fr")||
        shExpMatch(host, "recherche.edfpro.fr")||
        shExpMatch(host, "recherche-entreprises.edf.fr")||
        shExpMatch(host, "recherche-collectivite.edf.fr")||
        shExpMatch(host, "recherche.edf.com")||
        shExpMatch(host, "search.edf.com")||
        shExpMatch(host, "rh-clicrh.edf.fr")||
        shExpMatch(host, "voeux.erdfdistribution.fr")||
        shExpMatch(host, "be.edf.com") ||
        shExpMatch(host, "tropheesfondation.edf.com") ||
        shExpMatch(host, "www.edf-italia.com") ||
        shExpMatch(host, "search-press.edf.com") ||
        shExpMatch(host, "recherche-presse.edf.com") ||
        shExpMatch(host, "etoile-covoiturage.edf.fr") ||
        shExpMatch(host, "covoiturage.edf.fr") ||
        shExpMatch(host, "192.168.170.71") ||
        shExpMatch(host, "monagencepart.edf.fr") ||
        shExpMatch(host, "admin-aelsimm.edf.fr") ||
        shExpMatch(host, "admin-aelsimm2.edf.fr") ||
        shExpMatch(host, "www.partenaires.edf.fr") ||
        shExpMatch(host, "shareholdersandinvestors.edf.com") ||
        shExpMatch(host, "shareholders-and-investors.edf.com") ||
        shExpMatch(host, "diffusion-editions.edf.com") ||
        shExpMatch(host, "interessement.edf.fr") ||
        shExpMatch(host, "rdsoft.edf.fr") ||
        shExpMatch(host, "planetcommercial-act.edf.fr") ||
        shExpMatch(host, "remisedesvictoires.edf.fr") ||
        shExpMatch(host, "www.challenge-et-partage-dpih.edf.fr") ||
        shExpMatch(host, "challenge-et-partage-dpih.edf.fr") ||
        shExpMatch(host, "gmail.edf.fr") ||
        shExpMatch(host, "nosenergies.edf.fr") ||
        shExpMatch(host, "www.transports.edf.fr") ||
        shExpMatch(host, "bopki.edf.fr") ||
        shExpMatch(host, "dilsenergie-be.edf.com") ||
        shExpMatch(host, "nestenergie-be.edf.com") ||
        shExpMatch(host, "plateforme-editoriale.edf.com") ||
        shExpMatch(host, "bleuciel.edf.com") ||
        shExpMatch(host, "bleu-ciel.edf.com") ||
        shExpMatch(host, "facture-bleuciel.edf.com") ||
        shExpMatch(host, "residential.edf.com") ||
        shExpMatch(host, "edfpro.edf.com") ||
        shExpMatch(host, "pro.edf.com") ||
        shExpMatch(host, "small-business.edf.com") ||
        shExpMatch(host, "entreprises.edf.com") ||
        shExpMatch(host, "collectivites.edf.com") ||
        shExpMatch(host, "sei.edf.com") ||
        shExpMatch(host, "corse.edf.com") ||
        shExpMatch(host, "guyane.edf.com") ||
        shExpMatch(host, "reunion.edf.com") ||
        shExpMatch(host, "la-reunion.edf.com") ||
        shExpMatch(host, "st-pierre-miquelon.edf.com") ||
        shExpMatch(host, "saint-pierre-et-miquelon.edf.com") ||
        shExpMatch(host, "martinique.edf.com") ||
        shExpMatch(host, "guadeloupe.edf.com") ||
        shExpMatch(host, "energie.edf.com") ||
        shExpMatch(host, "centrale.edf.com") ||
        shExpMatch(host, "centrales.edf.com") ||
        shExpMatch(host, "power-plants.edf.com") ||
        shExpMatch(host, "powerplants.edf.com") ||
        shExpMatch(host, "nucleaire.edf.com") ||
        shExpMatch(host, "hydraulique.edf.com") ||
        shExpMatch(host, "thermique.edf.com") ||
        shExpMatch(host, "nuclear.edf.com") ||
        shExpMatch(host, "hydropower.edf.com") ||
        shExpMatch(host, "fossil-fired.edf.com") ||
        shExpMatch(host, "solar.edf.com") ||
        shExpMatch(host, "biomass.edf.com") ||
        shExpMatch(host, "geothermal.edf.com") ||
        shExpMatch(host, "windpower.edf.com") ||
        shExpMatch(host, "paimpol-brehat.edf.com") ||
        shExpMatch(host, "la-rance.edf.com") ||
        shExpMatch(host, "france.edf.com") ||
        shExpMatch(host, "fr.edf.com") ||
        shExpMatch(host, "fact-bleuciel.edf.com") ||
        shExpMatch(host, "facture-electronique.edf.com") ||
        shExpMatch(host, "raccordement-bleuciel.edf.com") ||
        shExpMatch(host, "contrat-raccordement-bleuciel.edf.com") ||
        shExpMatch(host, "residential-bleuciel.edf.com") ||
        shExpMatch(host, "facture-edfpro.edf.com") ||
        shExpMatch(host, "small-business-edfpro.edf.com") ||
        shExpMatch(host, "paimpol-brehat-en.edf.com") ||
        shExpMatch(host, "la-rance-en.edf.com") ||
        shExpMatch(host, "chooz-en.edf.com") ||
        shExpMatch(host, "civaux-en.edf.com") ||
        shExpMatch(host, "cruas-meysse-en.edf.com") ||
        shExpMatch(host, "belleville-en.edf.com") ||
        shExpMatch(host, "blayais-en.edf.com") ||
        shExpMatch(host, "bugey-en.edf.com") ||
        shExpMatch(host, "cattenom-en.edf.com") ||
        shExpMatch(host, "chinon-en.edf.com") ||
        shExpMatch(host, "dampierre-en.edf.com") ||
        shExpMatch(host, "flamanville-en.edf.com") ||
        shExpMatch(host, "fessenheim-en.edf.com") ||
        shExpMatch(host, "golfech-en.edf.com") ||
        shExpMatch(host, "gravelines-en.edf.com") ||
        shExpMatch(host, "nogent-en.edf.com") ||
        shExpMatch(host, "paluel-en.edf.com") ||
        shExpMatch(host, "penly-en.edf.com") ||
        shExpMatch(host, "saint-alban-en.edf.com") ||
        shExpMatch(host, "saint-laurent-en.edf.com") ||
        shExpMatch(host, "tricastin-en.edf.com") ||
        shExpMatch(host, "epr-flamanville-en.edf.com") ||
        shExpMatch(host, "brennilis-en.edf.com") ||
        shExpMatch(host, "bugey1-en.edf.com") ||
        shExpMatch(host, "chinon-a-en.edf.com") ||
        shExpMatch(host, "chooz-a-en.edf.com") ||
        shExpMatch(host, "creys-malville-en.edf.com") ||
        shExpMatch(host, "saint-laurent-a-en.edf.com") ||
        shExpMatch(host, "aramon.edf-en.com") ||
        shExpMatch(host, "aramon.edf-en.com") ||
        shExpMatch(host, "arrighi-en.edf.com") ||
        shExpMatch(host, "blenod-en.edf.com") ||
        shExpMatch(host, "bouchain-en.edf.com") ||
        shExpMatch(host, "brennilis-tac-en.edf.com") ||
        shExpMatch(host, "cordemais-en.edf.com") ||
        shExpMatch(host, "dirinon-en.edf.com") ||
        shExpMatch(host, "gennevilliers-en.edf.com") ||
        shExpMatch(host, "la-maxe-en.edf.com") ||
        shExpMatch(host, "le-havre-en.edf.com") ||
        shExpMatch(host, "montereau-en.edf.com") ||
        shExpMatch(host, "martigues-en.edf.com") ||
        shExpMatch(host, "porcheville-en.edf.com") ||
        shExpMatch(host, "richemont-en.edf.com") ||
        shExpMatch(host, "vaires-en.edf.com") ||
        shExpMatch(host, "vitry-en.edf.com") ||
        shExpMatch(host, "hydro-alpes-en.edf.com") ||
        shExpMatch(host, "hydro-centre-en.edf.com") ||
        shExpMatch(host, "hydro-est-en.edf.com") ||
        shExpMatch(host, "hydro-mediterranee-en.edf.com") ||
        shExpMatch(host, "hydro-sud-ouest-en.edf.com") ||
        shExpMatch(host, "engagements.edf.com") ||
        shExpMatch(host, "obligations-achats.edf.com") ||
        shExpMatch(host, "region.edf.com") ||
        shExpMatch(host, "openx.edf.com") ||
        shExpMatch(host, "back-openx.edf.com") ||
        shExpMatch(host, "conseil-bleuciel.edf.com") ||
        shExpMatch(host, "linky-bleuciel.edf.com") ||
        shExpMatch(host, "linky-edfpro.edf.com") ||
        shExpMatch(host, "france-en.edf.com") ||
        shExpMatch(host, "fr-en.edf.com") ||
        shExpMatch(host, "transports.edf.com") ||
        shExpMatch(host, "partenaires.edf.com") ||
        shExpMatch(host, "belgique-btob.edf.com") ||
        shExpMatch(host, "belgie-btob.edf.com") ||
        shExpMatch(host, "bazacle.edf.com") ||
        shExpMatch(host, "pprbackoffice.edf.com") ||
        shExpMatch(host, "orp-boutique.edf.fr") ||
        shExpMatch(host, "e-toile-covoiturage.edf.fr") ||
        shExpMatch(host, "netachat.edf.fr") ||
        shExpMatch(host, "hrcn.edf.fr") ||
        shExpMatch(host, "163.116.9.8") ||
        shExpMatch(host, "163.116.9.9") ||
        shExpMatch(host, "163.116.9.10") ||
        shExpMatch(host, "163.116.9.11") ||
        shExpMatch(host, "163.116.9.12") ||
        shExpMatch(host, "planetcommercial-iwfm-int.edf.fr") ||
        shExpMatch(host, "si-i-nov-it.edf.fr") ||
        shExpMatch(host, "omega.grdf.fr") ||
        shExpMatch(host, "gaiaform.gazdefrance.com") ||
        shExpMatch(host, "gaia.gazdefrance.com") ||	
        shExpMatch(host, "planetcommercial-planetwork.edf.fr") ||
        shExpMatch(host, "planetcommercial-trombinoscopes.edf.fr") ||
        shExpMatch(host, "concours-identitesonore.grdf.fr") ||
        shExpMatch(host, "192.168.170.77") ||
        shExpMatch(host, "192.168.170.72") ||
        shExpMatch(host, "192.168.50.1") ||
        shExpMatch(host, "flipbooks.edf.fr") ||
        shExpMatch(host, "www.dsp-peps.edf.fr") ||
        shExpMatch(host, "agence.edfpro.fr") ||
        shExpMatch(host, "portal.uat.hubwoo.com") ||
        shExpMatch(host, "portal.hubwoo.com") ||
        shExpMatch(host, "dauphin.int.cc-hubwoo.com") ||
        shExpMatch(host, "dauphin.cc-hubwoo.com") ||
        shExpMatch(host, "econnect.int.hubwoo.com") ||
        shExpMatch(host, "econnect.hubwoo.com")||
        shExpMatch(host, "rh-interpro.edf.fr") ||
        shExpMatch(host, "sei-aladin.edf.fr")||
        shExpMatch(host, "sei-aladin-recette.edf.fr")||
        dnsDomainIs(host, ".portail-achats.edf.com")||
        isInNet(host, "192.168.51.48","255.255.255.248") ||
        shExpMatch(host, "portail-achats.edf.com")||
        shExpMatch(host, "www.portail-achats.edf.com")||
        shExpMatch(host, "www1.portail-achats.edf.com")||
        shExpMatch(host, "www2.portail-achats.edf.com")||
        shExpMatch(host, "2days2gether.edf.com"))
        return "PROXY noevipncp2n.edf.fr:3128; PROXY pcyvipncp2n.edf.fr:3128";
    else
	if (isPlainHostName(host) ||
	dnsDomainIs(host, ".edf.fr") ||
	dnsDomainIs(host, ".gdf.fr") ||
	dnsDomainIs(host, ".edfgdf.fr") ||
	isInNet(host, "192.196.0.0","255.255.0.0") ||
	isInNet(host, "10.200.0.0","255.255.0.0") ||
	shExpMatch(host, "omega.grdf.fr")||
	shExpMatch(host, "omega.gazdefrance-distribution.com")||
	shExpMatch(host, "connect-racco.erdfdistribution.fr")||
	shExpMatch(host, "herakles-iq.erdfdistribution.fr") ||
	shExpMatch(host, "herakles-pp.erdfdistribution.fr") ||
	shExpMatch(host, "herakles.erdfdistribution.fr") ||
	shExpMatch(host, "www.curde.edfdistribution.fr") ||
	shExpMatch(host, "www.jeunes.gazdefrance.com") ||
	shExpMatch(host, "www-pays.gazdefrance.com") ||
	shExpMatch(host, "jeunes.gazdefrance.com") ||
	shExpMatch(host, "www2.gazdefrance.com") ||
	shExpMatch(host, "diagnet.gazdefrance.com") ||
	shExpMatch(host, "olimp.gazdefrance.com") ||
	shExpMatch(host, "solidarite.gazdefrance.com") ||
	shExpMatch(host, "azur.gazdefrance.com") ||
	shExpMatch(host, "cnm.gazdefrance.com") ||
	shExpMatch(host, "capmanager.gazdefrance.com") ||
	shExpMatch(host, "arche.gazdefrance.com") ||
	shExpMatch(host, "prodis.gazdefrance.com") ||
	shExpMatch(host, "coopernet.gazdefrance.com") ||
	shExpMatch(host, "trucsetastuces.gazdefrance.com") ||
	shExpMatch(host, "annusec1.gazdefrance.com") ||
	shExpMatch(host, "kitcc.gazdefrance.com") ||
	shExpMatch(host, "sesame.gazdefrance.com") ||
	shExpMatch(host, "www.gaz-de-france.*") ||
	shExpMatch(host, "centres.gazdefrance.com") ||
	shExpMatch(host, "industrie.gazdefrance.com") ||
	shExpMatch(host, "gazdefranceenergy.gazdefrance.com") ||
	shExpMatch(host, "annuaire-statistique.gazdefrance.com") ||
	shExpMatch(host, "codevgaz.gazdefrance.com") ||
	shExpMatch(host, "anous.gazdefrance.com") ||
	shExpMatch(host, "monagence.gazdefrance.com") ||
	shExpMatch(host, "sicm.gazdefrance.com") ||
	shExpMatch(host, "expertgaz.gazdefrance.com") ||
	shExpMatch(host, "particulie*.gazdefrance.com") ||
	shExpMatch(host, "212.30.118.203") ||
	shExpMatch(host, "www.echoesfromearth.com") ||
	shExpMatch(host, "www.lesvoixdelaterre.com") ||
	shExpMatch(host, "e-projets.gazdefrance.com") ||
	shExpMatch(host, "operations-transport.gazdefrance.com") ||
	shExpMatch(host, "cliegdfg.gazdefrance.com") ||
	shExpMatch(host, "contrat-etat.gazdefrance.com") ||
	shExpMatch(host, "plancom.gazdefrance.com") ||
	shExpMatch(host, "prjpays.gazdefrance.com") ||
	shExpMatch(host, "pmv.gazdefrance.com") ||
	shExpMatch(host, "pmv2.gazdefrance.com") ||
	shExpMatch(host, "edo.gazdefrance.com") ||
	shExpMatch(host, "gps.gazdefrance.com") ||
	shExpMatch(host, "go.gazdefrance.com") ||
	shExpMatch(host, "www.jeunesflammes.com") ||
	shExpMatch(host, "www.jeunesflammes.gazdefrance.com") ||
	shExpMatch(host, "gestesdinterieur.gazdefrance.com") ||
	shExpMatch(host, "www.developpementdurable.gazdefrance.com") ||
	shExpMatch(host, "www.fondation.gazdefrance.com") ||
	shExpMatch(host, "repmetiers.gazdefrance.com") ||
	shExpMatch(host, "bourse-emploi.gazdefrance.com") ||
	shExpMatch(host, "reptransverse.gazdefrance.com") ||
	shExpMatch(host, "intra-perigord.gazdefrance.com") ||
	shExpMatch(host, "telephon-e-mobile.gazdefrance.com") ||
	shExpMatch(host, "commandespubliques.gazdefrance.com") ||
	shExpMatch(host, "actigaz.gazdefrance.com") ||
	shExpMatch(host, "brancheexpertise.gazdefrance.com") ||
	shExpMatch(host, "epdm.gazdefrance.com") ||
	shExpMatch(host, "www.code-aster.org") ||
	shExpMatch(host, "www.codeaster.org") ||
	shExpMatch(host, "www.vivrelec.fr") ||
	shExpMatch(host, "www.vivrelec.com") ||
	shExpMatch(host, "edf-collectivites.fr") ||
	shExpMatch(host, "groupe-en-marche.gazdefrance.com") ||
	shExpMatch(host, "www.edf-group.net") ||
	shExpMatch(host, "wwws0.edf-group.net") ||
	shExpMatch(host, "wwws1.edf-group.net") ||
	shExpMatch(host, "www.temoignages.vivrelec.fr") ||
	shExpMatch(host, "distri-qualite.gazdefrance.com") ||
	shExpMatch(host, "distri-environnement.gazdefrance.com") ||
	shExpMatch(host, "distri-collectivites.gazdefrance.com") ||
	shExpMatch(host, "www.identitevisuelle.gazdefrance.com") ||
	shExpMatch(host, "identitevisuelle.gazdefrance.com") ||
	shExpMatch(host, "formulairesrh.gazdefrance.com") ||
	shExpMatch(host, "distri-caraibe.gazdefrance.com") ||
	shExpMatch(host, "distri-caraibe-admin.gazdefrance.com") ||
	shExpMatch(host, "ventimmo.gazdefrance.com") ||
	shExpMatch(host, "e-rdv-diagnostic.gazdefrance.com") ||
	shExpMatch(host, "macarriere.gazdefrance.com") ||
	shExpMatch(host, "procadre.gazdefrance.com") ||
	shExpMatch(host, "azur-stats.gazdefrance.com") ||
	shExpMatch(host, "base-baqm.gazdefrance.com") ||
	shExpMatch(host, "telereglement-taxeprofessionnelle.gazdefrance.com") ||
	shExpMatch(host, "reseaucommunes.gazdefrance.com") ||
	shExpMatch(host, "www.disnet.gazdefrance.com") ||
	shExpMatch(host, "dishab.disnet.gazdefrance.com") ||
	shExpMatch(host, "www.face-infos.com") ||
	shExpMatch(host, "www.fpe-infos.com") ||
	shExpMatch(host, "stations-test.anfr.fr") ||
	shExpMatch(host, "www.wattnews.com") ||
	shExpMatch(host, "www.edf-oasolaire.fr") ||
	shExpMatch(host, "grc.edfentreprises.fr") ||
	shExpMatch(host, "*edfentreprises.*") ||
	shExpMatch(host, "*edf-entreprises.*") ||
	shExpMatch(host, "*.emea.extranet.reuters.biz") ||
	shExpMatch(host, "*.session.rservices.com") ||
	shExpMatch(host, "www.interactivmanager.net") ||
	shExpMatch(host, "agents.interactivmanager.net") ||
	shExpMatch(host, "agent.interactivmanager.net") ||
	shExpMatch(host, "hypervisors.interactivmanager.net") ||
	shExpMatch(host, "supervisors.interactivmanager.net") ||
	shExpMatch(host, "live304.interactivmanager.net") ||
	shExpMatch(host, "live305.interactivmanager.net") ||
	shExpMatch(host, "w304.interactivmanager.net") ||
	shExpMatch(host, "w305.interactivmanager.net") ||
	shExpMatch(host, "dt.interact-iv.com") ||
	shExpMatch(host, "dtedf.interact-iv.com") ||
	shExpMatch(host, "dt304.interactivmanager.net") ||
	shExpMatch(host, "phone-qualif.interact-iv.net") ||
	shExpMatch(host, "pepitocmdb.erdfdistribution.fr") ||
	shExpMatch(host, "pepitocmdb-pprd.erdfdistribution.fr") ||
	shExpMatch(host, "pepitocmdb-qualif.erdfdistribution.fr") ||
	shExpMatch(host, "localhost") ||
	isInNet(host, "127.0.0.0","255.0.0.0") ||
	shExpMatch(host, "xmpp.edf-labs.net") ||
	shExpMatch(host, "192.196.77.110")||
	shExpMatch(host, "www.edfcollectivite.fr")||
	shExpMatch(host, "www.edfcollectivites.fr")||
	shExpMatch(host, "citrix-ext1.axa-corporatesolutions.intraxa")||
	shExpMatch(host, "citrix-ext2.axa-corporatesolutions.intraxa")||
	shExpMatch(host, "stats.edfsolidarite.interactivmanager.net")||
	shExpMatch(host, "agence-testhd.edfpro.fr")||
	shExpMatch(host, "mediateur.edf.com") ||
	isInNet(host, "193.252.4.0","255.255.255.0") ||
	isInNet(host, "192.168.0.0","255.255.0.0") ||
	isInNet(host, "172.16.0.0","255.240.0.0") ||
	isInNet(host, "192.54.192.0","255.255.255.0") ||
	isInNet(host, "192.54.193.0","255.255.255.0") ||
	isInNet(host, "163.11.0.0","255.255.0.0") ||
	isInNet(host, "163.9.0.0","255.255.0.0") ||
	isInNet(host, "163.8.0.0","255.255.0.0") ||
	isInNet(host, "163.7.0.0","255.255.0.0") ||
	isInNet(host, "163.6.0.0","255.255.0.0") ||
	isInNet(host, "163.62.0.0","255.255.0.0") ||
	isInNet(host, "163.63.0.0","255.255.0.0") ||
	isInNet(host, "163.64.0.0","255.255.0.0") ||
	isInNet(host, "163.65.0.0","255.255.0.0") ||
	isInNet(host, "163.66.0.0","255.255.0.0") ||
	isInNet(host, "163.67.0.0","255.255.0.0") ||
	isInNet(host, "163.68.0.0","255.255.0.0") ||
	isInNet(host, "163.69.0.0","255.255.0.0") ||
	isInNet(host, "163.70.0.0","255.255.0.0") ||
	isInNet(host, "163.71.0.0","255.255.0.0") ||
	isInNet(host, "163.72.0.0","255.255.0.0") ||
	isInNet(host, "163.73.0.0","255.255.0.0") ||
	isInNet(host, "163.74.0.0","255.255.0.0") ||
	isInNet(host, "163.75.0.0","255.255.0.0") ||
	isInNet(host, "163.76.0.0","255.255.0.0") ||
	isInNet(host, "163.77.0.0","255.255.0.0") ||
	isInNet(host, "163.78.0.0","255.255.0.0") ||
	isInNet(host, "163.79.0.0","255.255.0.0") ||
	isInNet(host, "163.80.0.0","255.255.0.0") ||
	isInNet(host, "163.81.0.0","255.255.0.0") ||
	isInNet(host, "163.82.0.0","255.255.0.0") ||
	isInNet(host, "163.83.0.0","255.255.0.0") ||
	isInNet(host, "163.84.0.0","255.255.0.0") ||
	isInNet(host, "163.85.0.0","255.255.0.0") ||
	isInNet(host, "163.86.0.0","255.255.0.0") ||
	isInNet(host, "163.87.0.0","255.255.0.0") ||
	isInNet(host, "163.88.0.0","255.255.0.0") ||
	isInNet(host, "163.89.0.0","255.255.0.0") ||
	isInNet(host, "163.90.0.0","255.255.0.0") ||
	isInNet(host, "163.91.0.0","255.255.0.0") ||
	isInNet(host, "163.92.0.0","255.255.0.0") ||
	isInNet(host, "163.93.0.0","255.255.0.0") ||
	isInNet(host, "163.94.0.0","255.255.0.0") ||
	isInNet(host, "163.95.0.0","255.255.0.0") ||
	isInNet(host, "163.96.0.0","255.255.0.0") ||
	isInNet(host, "163.97.0.0","255.255.0.0") ||
	isInNet(host, "163.98.0.0","255.255.0.0") ||
	isInNet(host, "163.99.0.0","255.255.0.0") ||
	isInNet(host, "163.100.0.0","255.255.0.0") ||
	isInNet(host, "163.101.0.0","255.255.0.0") ||
	isInNet(host, "163.102.0.0","255.255.0.0") ||
	isInNet(host, "163.103.0.0","255.255.0.0") ||
	isInNet(host, "163.104.0.0","255.255.0.0") ||
	isInNet(host, "163.105.0.0","255.255.0.0") ||
	isInNet(host, "163.106.0.0","255.255.0.0") ||
	isInNet(host, "163.107.0.0","255.255.0.0") ||
	isInNet(host, "163.108.0.0","255.255.0.0") ||
	isInNet(host, "163.109.0.0","255.255.0.0") ||
	isInNet(host, "163.110.0.0","255.255.0.0") ||
	isInNet(host, "163.111.0.0","255.255.0.0") ||
	isInNet(host, "163.112.0.0","255.255.0.0") ||
	isInNet(host, "163.113.0.0","255.255.0.0") ||
	isInNet(host, "163.114.0.0","255.255.0.0") ||
	isInNet(host, "163.115.0.0","255.255.0.0") ||
	isInNet(host, "163.116.0.0","255.255.0.0") ||
	isInNet(host, "163.128.163.0","255.255.255.128") ||
	isInNet(host, "169.254.0.0","255.255.0.0") ||
	isInNet(host, "149.251.0.0","255.255.0.0") ||
	isInNet(host, "144.165.0.0","255.255.0.0") ||
	isInNet(host, "158.231.130.0","255.255.255.0") ||
	isInNet(host, "158.231.128.0","255.255.255.0") ||
	isInNet(host, "10.203.48.0","255.255.255.0") ||
	isInNet(host, "10.203.0.0","255.255.0.0") ||
	isInNet(host, "10.151.0.0","255.255.0.0") ||
	isInNet(host, "10.114.124.0","255.255.252.0") ||
	isInNet(host, "10.114.128.0","255.255.128.0") ||
	isInNet(host, "10.115.0.0","255.255.0.0") ||
	isInNet(host, "10.116.0.0","255.252.0.0") ||
	isInNet(host, "10.120.0.0","255.252.0.0") ||
	isInNet(host, "10.0.0.0", "255.0.0.0") ||
	isInNet(host, "192.54.195.0","255.255.255.0") ||
	isInNet(host, "192.70.95.0","255.255.255.0") ||
	isInNet(host, "130.98.0.0","255.255.0.0") ||
	shExpMatch(host, "144.165.2.32") ||
	shExpMatch(host, "10.50.52.11") ||
	shExpMatch(host, "10.11.11.3") ||
	isInNet(host, "158.231.132.0","255.255.254.0") ||
	isInNet(host, "192.93.151.0","255.255.255.0") ||
	isInNet(host, "148.143.0.0","255.255.0.0"))
	return "DIRECT";
else
    return "PROXY noevipncp2n.edf.fr:3128; PROXY pcyvipncp2n.edf.fr:3128";
}
