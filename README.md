### Cream Library for <a title="Pure Data" href="http://msp.ucsd.edu/" target="_blank">Pure Data </a>

<p>A set of  PD externals for those who like vanilla... but also want some chocolate, coffee, caramel or cinnamon.</p> <p>The Cream Library uses <a title="CICM Wrapper" href="https://github.com/CICM/CicmWrapper" target="_blank">CICM Wrapper</a> developed to facilitate the creation of signal objects and GUI and to improve user interactions.</p> <p>Copyright (C) 2013-2015 Pierre Guillot, CICM - Université Paris 8</p>

##### Version :

Alpha 0.4 for Pure Data Vanilla and Pure Data Extended on Mac Os, Windows and Linux plateforms.

![capture decran 2014-07-24 a 13 11 29](https://cloud.githubusercontent.com/assets/1409918/3687190/623be588-1323-11e4-9818-8b463e353e71.png)

##### Compilation :

<p>Mac OsX : Use the XCode project (cream.xcodeproj). </p>
<p>Windows : Use the Visual Studio project (c.library.vcxproj).</p>
<p>Linux   : Use the Code Block project (c.library.cbp).</p>
<p>All     : Use the makefile.</p>

##### Installation :

<p>In the "CreamLibrary" folder, go to the "externals" folder then copy the external that matchs with your OS and your Pure Data distribution and past the external at the root of the "CreamLibrary" folder. You can also skip this part and use directly the path of that match with your OS and your Pure Data distribution in the startup options.</P>

<p>Pure Data Vanilla : Copy the "CreamLibrary" folder in your Pure Data <a title="package folder" href="http://puredata.info/docs/faq/how-do-i-install-externals-and-help-files" target="_blank">package folder</a> and add "c.library" in startup options. With another folder than the default ones, add the folder in the path preferences (~/CreamLibrary/Package) and add "CreamLibrary/cream" in the startup options.</P>

<p>Pure Data Extended : Copy the "CreamLibrary" folder in your Pure Data <a title="package folder" href="http://puredata.info/docs/faq/how-do-i-install-externals-and-help-files" target="_blank">package folder</a> and add "-lib c.library" in startup options. With another folder than the default ones, add the folder in the path preferences (~/CreamLibrary/Package) and add "-lib CreamLibrary/cream" in the startup options.</P>

<p>Exemple of startup options for Pure Data Extended : "-lib CreamLibrary/cream" (if your external is at the root of the CreamLibrary folder), "-lib CreamLibrary/externals/MacOs/pd-extended/cream" (if you didn't move the external from the external folder on MacOs).</p>

<p>Uninstall : Remove the "CreamLibrary" folder and remove the startup options.</p>

##### Author :

Pierre Guillot

##### Licence : 

The Cream Library in under the <a title="GNU" href="http://www.gnu.org/copyleft/gpl.html" target="_blank">GNU Public License</a>. If you'd like to avoid the restrictions of the GPL and use the Cream Library for a closed-source product, you contact the <a title="CICM" href="http://cicm.mshparisnord.org/" target="_blank">CICM</a>.


