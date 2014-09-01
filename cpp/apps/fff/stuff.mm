<map version="freeplane 1.3.0">
<!--To view this file, download free mind mapping software Freeplane from http://freeplane.sourceforge.net -->
<node TEXT="flexible file factory" ID="ID_1723255651" CREATED="1283093380553" MODIFIED="1409294446742"><hook NAME="MapStyle" zoom="1.464">

<map_styles>
<stylenode LOCALIZED_TEXT="styles.root_node">
<stylenode LOCALIZED_TEXT="styles.predefined" POSITION="right">
<stylenode LOCALIZED_TEXT="default" MAX_WIDTH="600" COLOR="#000000" STYLE="as_parent">
<font NAME="SansSerif" SIZE="10" BOLD="false" ITALIC="false"/>
</stylenode>
<stylenode LOCALIZED_TEXT="defaultstyle.details"/>
<stylenode LOCALIZED_TEXT="defaultstyle.note"/>
<stylenode LOCALIZED_TEXT="defaultstyle.floating">
<edge STYLE="hide_edge"/>
<cloud COLOR="#f0f0f0" SHAPE="ROUND_RECT"/>
</stylenode>
</stylenode>
<stylenode LOCALIZED_TEXT="styles.user-defined" POSITION="right">
<stylenode LOCALIZED_TEXT="styles.topic" COLOR="#18898b" STYLE="fork">
<font NAME="Liberation Sans" SIZE="10" BOLD="true"/>
</stylenode>
<stylenode LOCALIZED_TEXT="styles.subtopic" COLOR="#cc3300" STYLE="fork">
<font NAME="Liberation Sans" SIZE="10" BOLD="true"/>
</stylenode>
<stylenode LOCALIZED_TEXT="styles.subsubtopic" COLOR="#669900">
<font NAME="Liberation Sans" SIZE="10" BOLD="true"/>
</stylenode>
<stylenode LOCALIZED_TEXT="styles.important">
<icon BUILTIN="yes"/>
</stylenode>
</stylenode>
<stylenode LOCALIZED_TEXT="styles.AutomaticLayout" POSITION="right">
<stylenode LOCALIZED_TEXT="AutomaticLayout.level.root" COLOR="#000000">
<font SIZE="18"/>
</stylenode>
<stylenode LOCALIZED_TEXT="AutomaticLayout.level,1" COLOR="#0033ff">
<font SIZE="16"/>
</stylenode>
<stylenode LOCALIZED_TEXT="AutomaticLayout.level,2" COLOR="#00b439">
<font SIZE="14"/>
</stylenode>
<stylenode LOCALIZED_TEXT="AutomaticLayout.level,3" COLOR="#990000">
<font SIZE="12"/>
</stylenode>
<stylenode LOCALIZED_TEXT="AutomaticLayout.level,4" COLOR="#111111">
<font SIZE="10"/>
</stylenode>
</stylenode>
</stylenode>
</map_styles>
</hook>
<hook NAME="AutomaticEdgeColor" COUNTER="11"/>
<node TEXT="collect" POSITION="right" ID="ID_1640078965" CREATED="1409228877970" MODIFIED="1409228880096">
<edge COLOR="#ff0000"/>
<node TEXT="input" ID="ID_656568316" CREATED="1409228913690" MODIFIED="1409228919527">
<node TEXT="one .cpp file" ID="ID_915063149" CREATED="1409228920234" MODIFIED="1409228925726"/>
<node TEXT="directory" ID="ID_1926968555" CREATED="1409228926049" MODIFIED="1409228929607"/>
<node TEXT=".lua config file" ID="ID_1305545276" CREATED="1409228930089" MODIFIED="1409228936583"/>
</node>
<node TEXT="add new build inputs" ID="ID_34621326" CREATED="1409229237672" MODIFIED="1409229255204">
<node TEXT=".hpp to .cpp translation" ID="ID_1805763584" CREATED="1409229255583" MODIFIED="1409229264877"/>
<node TEXT="parsing for headers" ID="ID_493487066" CREATED="1409229265255" MODIFIED="1409229275683"/>
<node TEXT="lists of files via configs" ID="ID_1410109754" CREATED="1409229276887" MODIFIED="1409229291491"/>
<node TEXT="add build settings" ID="ID_1072565244" CREATED="1409229292207" MODIFIED="1409229307764"/>
</node>
</node>
<node TEXT="build" POSITION="right" ID="ID_30545050" CREATED="1409228880458" MODIFIED="1409228986095">
<edge COLOR="#0000ff"/>
<node TEXT="cache with lookup based on hash of all required input of a build rule" ID="ID_830225536" CREATED="1409229062441" MODIFIED="1409229086789">
<node TEXT="recursive dependencies" ID="ID_850071995" CREATED="1409229654532" MODIFIED="1409229662650"/>
</node>
<node TEXT="fast mode that only checks file sizes, not the complete checksum" ID="ID_1004731322" CREATED="1409229087648" MODIFIED="1409229153181"/>
<node TEXT="flow" ID="ID_356685122" CREATED="1409229527021" MODIFIED="1409229533450">
<node TEXT="files and tags" ID="ID_368199419" CREATED="1409229534221" MODIFIED="1409229553770"/>
<node TEXT="command" ID="ID_1124992172" CREATED="1409229554141" MODIFIED="1409229559033"/>
<node TEXT="hash based on" ID="ID_1367041837" CREATED="1409229610916" MODIFIED="1409229703866">
<node TEXT="files" ID="ID_1037978672" CREATED="1409229686965" MODIFIED="1409229688786"/>
<node TEXT="tags" ID="ID_169729432" CREATED="1409229689244" MODIFIED="1409229690889"/>
<node TEXT="command" ID="ID_756676795" CREATED="1409229691292" MODIFIED="1409229692689"/>
<node TEXT="dependent files" ID="ID_1310935216" CREATED="1409229693035" MODIFIED="1409229697297"/>
</node>
<node TEXT="1 output file" ID="ID_1168721999" CREATED="1409229561589" MODIFIED="1409229573225"/>
</node>
</node>
<node TEXT="link" POSITION="right" ID="ID_606699131" CREATED="1409228883450" MODIFIED="1409228990567">
<edge COLOR="#00ff00"/>
</node>
<node TEXT="run" POSITION="right" ID="ID_1445278886" CREATED="1409228885595" MODIFIED="1409228886743">
<edge COLOR="#ff00ff"/>
<node TEXT="execute" ID="ID_1226716140" CREATED="1409229038050" MODIFIED="1409229040190"/>
<node TEXT="program" ID="ID_289633717" CREATED="1409229040553" MODIFIED="1409229042702"/>
<node TEXT="upload" ID="ID_1508216912" CREATED="1409229043073" MODIFIED="1409229050214"/>
</node>
<node TEXT="config" POSITION="left" ID="ID_1378818621" CREATED="1409228938994" MODIFIED="1409228942255">
<edge COLOR="#00ffff"/>
<node TEXT="multiple" ID="ID_793463059" CREATED="1409228943161" MODIFIED="1409228950663"/>
<node TEXT="changes the rules for collect/build/link/run" ID="ID_1449691230" CREATED="1409228951969" MODIFIED="1409228979718"/>
<node TEXT="defaults per file type/location" ID="ID_1585900536" CREATED="1409228996889" MODIFIED="1409229020718"/>
</node>
<node TEXT="settings" POSITION="left" ID="ID_844729933" CREATED="1409229322070" MODIFIED="1409229325276">
<edge COLOR="#ffff00"/>
<node TEXT="each file can have different setting types" ID="ID_381647759" CREATED="1409229326167" MODIFIED="1409229345691"/>
</node>
<node TEXT="file" POSITION="left" ID="ID_1602893697" CREATED="1409229359246" MODIFIED="1409229360339">
<edge COLOR="#7c0000"/>
<node TEXT="tags" ID="ID_1467901024" CREATED="1409229361302" MODIFIED="1409229362875"/>
</node>
<node TEXT="tag" POSITION="left" ID="ID_695276877" CREATED="1409229460302" MODIFIED="1409229462575">
<edge COLOR="#007c00"/>
<node TEXT="based on extension" ID="ID_905027736" CREATED="1409229465039" MODIFIED="1409229471090"/>
<node TEXT="based on location" ID="ID_157243485" CREATED="1409229471549" MODIFIED="1409229475667"/>
<node TEXT="based on config" ID="ID_324983489" CREATED="1409229476070" MODIFIED="1409229481098"/>
</node>
<node TEXT="scenarios" POSITION="right" ID="ID_1686650510" CREATED="1409230612251" MODIFIED="1409230617714">
<edge COLOR="#7c007c"/>
<node TEXT="build c++ executable" ID="ID_1907829194" CREATED="1409230619622" MODIFIED="1409230634930">
<node TEXT="find necessary .cpp files based on one file" ID="ID_642084043" CREATED="1409230635372" MODIFIED="1409230665951"/>
<node TEXT="find include paths" ID="ID_816925134" CREATED="1409230666610" MODIFIED="1409230673330"/>
<node TEXT="find settings and libs based on shallow parsing for include statements" ID="ID_1636018286" CREATED="1409230673695" MODIFIED="1409230701254"/>
<node TEXT="run or program into arduino" ID="ID_310992277" CREATED="1409230703322" MODIFIED="1409230711771"/>
</node>
<node TEXT="make static site" ID="ID_1739408032" CREATED="1409230713573" MODIFIED="1409230723648">
<node TEXT="asciidoctor to convert .adoc to .html" ID="ID_152725149" CREATED="1409230724427" MODIFIED="1409230748462"/>
<node TEXT="upload site" ID="ID_1239218826" CREATED="1409230748840" MODIFIED="1409230754296"/>
</node>
<node TEXT="run all unit tests" ID="ID_235528342" CREATED="1409230764363" MODIFIED="1409230768969"/>
<node TEXT="fix include guards" ID="ID_223767665" CREATED="1409230771452" MODIFIED="1409230777490"/>
<node TEXT="indexing" ID="ID_1655971912" CREATED="1409230786926" MODIFIED="1409230804774"/>
<node TEXT="fix newlines" ID="ID_380154476" CREATED="1409230976624" MODIFIED="1409230985439"/>
</node>
<node TEXT="list of processors" POSITION="left" ID="ID_811379018" CREATED="1409295309654" MODIFIED="1409295329450">
<edge COLOR="#007c7c"/>
<node TEXT="can generate new files" ID="ID_1641111387" CREATED="1409295330269" MODIFIED="1409295336691">
<node TEXT="file should always exist" ID="ID_477387074" CREATED="1409295351340" MODIFIED="1409295358097"/>
</node>
<node TEXT="can add new settings" ID="ID_1744789527" CREATED="1409295338292" MODIFIED="1409295344513"/>
<node TEXT="always start with start of processors again" ID="ID_1794640865" CREATED="1409295359764" MODIFIED="1409295374751"/>
<node TEXT="multi-thread" ID="ID_1023584103" CREATED="1409295376058" MODIFIED="1409296587652"/>
</node>
</node>
</map>
