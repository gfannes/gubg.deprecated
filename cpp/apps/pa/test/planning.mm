<map version="freeplane 1.2.0">
<!--To view this file, download free mind mapping software Freeplane from http://freeplane.sourceforge.net -->
<node TEXT="planning" ID="ID_1723255651" CREATED="1283093380553" MODIFIED="1368925253870"><hook NAME="MapStyle" zoom="1.5">
    <properties show_icon_for_attributes="true"/>

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
<hook NAME="AutomaticEdgeColor" COUNTER="3"/>
<node TEXT="task a" POSITION="right" ID="ID_500267427" CREATED="1368925157507" MODIFIED="1368925605178">
<edge COLOR="#ff0000"/>
<attribute NAME="estimate" VALUE="4" OBJECT="org.freeplane.features.format.FormattedNumber|4"/>
<node TEXT="do 1" ID="ID_312674642" CREATED="1368925172695" MODIFIED="1368925622169">
<attribute NAME="estimate" VALUE="12" OBJECT="org.freeplane.features.format.FormattedNumber|12"/>
</node>
<node TEXT="do 2" ID="ID_1271135418" CREATED="1368925176164" MODIFIED="1368925648550">
<attribute NAME="estimate" VALUE="23" OBJECT="org.freeplane.features.format.FormattedNumber|23"/>
<attribute NAME="oeu" VALUE="123" OBJECT="org.freeplane.features.format.FormattedNumber|123"/>
</node>
<node TEXT="do nothing" ID="ID_1017339220" CREATED="1368925179218" MODIFIED="1368925185647"/>
</node>
<node TEXT="task b" POSITION="right" ID="ID_1059129007" CREATED="1368925162726" MODIFIED="1368925304089">
<edge COLOR="#0000ff"/>
<attribute NAME="estimate" VALUE="2" OBJECT="org.freeplane.features.format.FormattedNumber|2"/>
</node>
</node>
</map>
