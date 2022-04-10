<map version="freeplane 1.9.8">
<!--To view this file, download free mind mapping software Freeplane from https://www.freeplane.org -->
<node TEXT="Effective C++" FOLDED="false" ID="ID_781028109" CREATED="1315921353234" MODIFIED="1649558039158"><hook NAME="MapStyle">
    <properties edgeColorConfiguration="#808080ff,#ff0000ff,#0000ffff,#00ff00ff,#ff00ffff,#00ffffff,#7c0000ff,#00007cff,#007c00ff,#7c007cff,#007c7cff,#7c7c00ff" fit_to_viewport="false" show_note_icons="true"/>

<map_styles>
<stylenode LOCALIZED_TEXT="styles.root_node" STYLE="oval" UNIFORM_SHAPE="true" VGAP_QUANTITY="24 pt">
<font SIZE="24"/>
<stylenode LOCALIZED_TEXT="styles.predefined" POSITION="right" STYLE="bubble">
<stylenode LOCALIZED_TEXT="default" ID="ID_271890427" ICON_SIZE="12 pt" COLOR="#000000" STYLE="fork">
<arrowlink SHAPE="CUBIC_CURVE" COLOR="#000000" WIDTH="2" TRANSPARENCY="200" DASH="" FONT_SIZE="9" FONT_FAMILY="SansSerif" DESTINATION="ID_271890427" STARTARROW="DEFAULT" ENDARROW="NONE"/>
<font NAME="SansSerif" SIZE="10" BOLD="false" ITALIC="false"/>
<richcontent CONTENT-TYPE="plain/auto" TYPE="DETAILS"/>
<richcontent TYPE="NOTE" CONTENT-TYPE="plain/auto"/>
</stylenode>
<stylenode LOCALIZED_TEXT="defaultstyle.details"/>
<stylenode LOCALIZED_TEXT="defaultstyle.attributes">
<font SIZE="9"/>
</stylenode>
<stylenode LOCALIZED_TEXT="defaultstyle.note" COLOR="#000000" BACKGROUND_COLOR="#ffffff" TEXT_ALIGN="LEFT"/>
<stylenode LOCALIZED_TEXT="defaultstyle.floating">
<edge STYLE="hide_edge"/>
<cloud COLOR="#f0f0f0" SHAPE="ROUND_RECT"/>
</stylenode>
<stylenode LOCALIZED_TEXT="defaultstyle.selection" BACKGROUND_COLOR="#4e85f8" BORDER_COLOR_LIKE_EDGE="false" BORDER_COLOR="#4e85f8"/>
</stylenode>
<stylenode LOCALIZED_TEXT="styles.user-defined" POSITION="right" STYLE="bubble">
<stylenode LOCALIZED_TEXT="styles.topic" COLOR="#18898b" STYLE="fork">
<font NAME="Liberation Sans" SIZE="10" BOLD="true"/>
</stylenode>
<stylenode LOCALIZED_TEXT="styles.subtopic" COLOR="#cc3300" STYLE="fork">
<font NAME="Liberation Sans" SIZE="10" BOLD="true"/>
</stylenode>
<stylenode LOCALIZED_TEXT="styles.subsubtopic" COLOR="#669900">
<font NAME="Liberation Sans" SIZE="10" BOLD="true"/>
</stylenode>
<stylenode LOCALIZED_TEXT="styles.important" ID="ID_67550811">
<icon BUILTIN="yes"/>
<arrowlink COLOR="#003399" TRANSPARENCY="255" DESTINATION="ID_67550811"/>
</stylenode>
</stylenode>
<stylenode LOCALIZED_TEXT="styles.AutomaticLayout" POSITION="right" STYLE="bubble">
<stylenode LOCALIZED_TEXT="AutomaticLayout.level.root" COLOR="#000000" STYLE="oval" SHAPE_HORIZONTAL_MARGIN="10 pt" SHAPE_VERTICAL_MARGIN="10 pt">
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
<stylenode LOCALIZED_TEXT="AutomaticLayout.level,5"/>
<stylenode LOCALIZED_TEXT="AutomaticLayout.level,6"/>
<stylenode LOCALIZED_TEXT="AutomaticLayout.level,7"/>
<stylenode LOCALIZED_TEXT="AutomaticLayout.level,8"/>
<stylenode LOCALIZED_TEXT="AutomaticLayout.level,9"/>
<stylenode LOCALIZED_TEXT="AutomaticLayout.level,10"/>
<stylenode LOCALIZED_TEXT="AutomaticLayout.level,11"/>
</stylenode>
</stylenode>
</map_styles>
</hook>
<node TEXT="让自己习惯C++" POSITION="right" ID="ID_1974122051" CREATED="1315921674234" MODIFIED="1323180334718">
<icon BUILTIN="full-1"/>
<node TEXT="1. 视C++为一个语言联邦" ID="ID_1079760855" CREATED="1315921697187" MODIFIED="1649557635970"/>
<node TEXT="2. 尽量以const, enum, inline替换#define" ID="ID_290070448" CREATED="1315921745750" MODIFIED="1649557647138">
<font NAME="SansSerif" SIZE="12"/>
</node>
<node TEXT="3. 尽可能使用 const" ID="ID_1828912009" CREATED="1315921774718" MODIFIED="1649557659617"><richcontent TYPE="NOTE" CONTENT-TYPE="xml/">
<html>
  <head>
    
  </head>
  <body>
    <p>
      &#20851;&#38190;&#23383;const&#20986;&#29616;&#22312;&#26143;&#21495;&#24038;&#36793;&#65292;&#34920;&#31034;&#34987;&#25351;&#29289;&#26159;&#24120;&#37327;&#65292;&#22914;&#26524;&#20986;&#29616;&#22312;&#21491;&#36793;&#65292;&#34920;&#31034;&#25351;&#38024;&#33258;&#36523;&#26159;&#24120;&#37327;&#65292;&#22914;&#26524;&#20986;&#29616;&#22312;&#26143;&#21495;&#20004;&#36793;&#65292;&#34920;&#31034;&#34987;&#25351;&#29289;&#21644;&#25351;&#38024;&#20004;&#32773;&#26159;&#24120;&#37327;
    </p>
  </body>
</html></richcontent>
</node>
<node TEXT="4. 确定对象被使用前已先被初始化" ID="ID_1534064865" CREATED="1315921785750" MODIFIED="1649557671861"/>
</node>
<node TEXT="构造/析构/赋值运算" POSITION="right" ID="ID_978473956" CREATED="1323180272218" MODIFIED="1323180338343">
<icon BUILTIN="full-2"/>
<node TEXT="5. 了解 C++ 默默编写并调用了哪些函数" ID="ID_327118147" CREATED="1649557570337" MODIFIED="1649557617789"/>
<node TEXT="6. 若不想使用编译器自动生成的函数，就该明确拒绝" ID="ID_1849799394" CREATED="1649557678418" MODIFIED="1649557710818"/>
<node TEXT="7. 为多态基类声明 virtual 析构函数" ID="ID_866709255" CREATED="1649557712728" MODIFIED="1649557730412"/>
<node TEXT="8. 别让异常逃离析构函数" ID="ID_1291742168" CREATED="1649557731126" MODIFIED="1649557752976"/>
<node TEXT="9. 绝不在构造和析构过程中调用 virtual 函数" ID="ID_160787174" CREATED="1649557754357" MODIFIED="1649557780785"/>
<node TEXT="10. 令 operator= 返回一个 reference to *this" ID="ID_1447532022" CREATED="1649557783880" MODIFIED="1649557804036"/>
<node TEXT="11. 在 operator= 中处理自我赋值" ID="ID_687499004" CREATED="1649557806537" MODIFIED="1649557834209"/>
<node TEXT="12. 复制对象时勿忘其中每一个成分" ID="ID_881151665" CREATED="1649557834921" MODIFIED="1649557872479"/>
</node>
<node TEXT="资源管理" POSITION="right" ID="ID_1530790534" CREATED="1323180345703" MODIFIED="1323180418484">
<icon BUILTIN="full-3"/>
<node TEXT="13. 以对象管理资源" ID="ID_688812424" CREATED="1649557877097" MODIFIED="1649557885884"/>
<node TEXT="14. 在资源管理类中小心 coping 行为" ID="ID_1348834964" CREATED="1649557886338" MODIFIED="1649557907665"/>
<node TEXT="15. 在资源管理类中提供对原始资源的访问" ID="ID_365743656" CREATED="1649557908171" MODIFIED="1649557927406"/>
<node TEXT="16. 成对使用 new 和 delete 时要采取相同形式" ID="ID_1063988889" CREATED="1649557927761" MODIFIED="1649557962735"/>
<node TEXT="17. 以独立语句将 newed 对象置入智能指针" ID="ID_414552758" CREATED="1649557963194" MODIFIED="1649557995022"/>
</node>
<node TEXT="设计与声明" POSITION="right" ID="ID_1700297359" CREATED="1315922660718" MODIFIED="1323180420515">
<icon BUILTIN="full-4"/>
<node TEXT="18. 让接口容易被正确使用, 不易被误用" ID="ID_899933598" CREATED="1323180817375" MODIFIED="1323180856343"/>
<node TEXT="19. 设计 class 犹如设计 type" ID="ID_977512627" CREATED="1323180872812" MODIFIED="1323180886718"/>
<node TEXT="20. 宁以pass-by-reference-to-const 替换 pass-by-value" ID="ID_1488217834" CREATED="1323180887218" MODIFIED="1323180918937"/>
<node TEXT="21. 必须返回对象时，别妄想返回其reference" ID="ID_1947074279" CREATED="1315922672296" MODIFIED="1323180865234"><richcontent TYPE="NOTE" CONTENT-TYPE="xml/">
<html>
  <head>
    
  </head>
  <body>
    <p>
      &#32477;&#23545;&#19981;&#35201;&#36820;&#22238;pointer &#25110; reference &#25351;&#21521;&#19968;&#20010; local stack &#23545;&#35937;&#65292; &#25110;&#36820;&#22238; reference &#25351;&#21521;&#19968;&#20010; heap-allocated&#23545;&#35937;&#65292; &#25110;&#36820;&#22238; pointer &#25110; reference &#25351;&#21521;&#19968;&#20010; local static &#23545;&#35937;&#32780;&#26377;&#21487;&#33021;&#21516;&#26102;&#38656;&#35201;&#22810;&#20010;&#36825;&#26679;&#30340;&#23545;&#35937;
    </p>
  </body>
</html></richcontent>
</node>
<node TEXT="22. 将成员变量声明为 private" ID="ID_1399808663" CREATED="1323180928703" MODIFIED="1323180945156"/>
<node TEXT="23. 宁以non-member, non-friend 替换 member 函数" ID="ID_166183079" CREATED="1323180945937" MODIFIED="1323180979593"/>
<node TEXT="24. 若所有参数皆需类型转换，请为此采用 non-member 函数" ID="ID_493998565" CREATED="1649558049647" MODIFIED="1649558079139"/>
<node TEXT="25. 考虑写出一个不抛异常的 swap 函数" ID="ID_780677835" CREATED="1649558092002" MODIFIED="1649558109016"/>
</node>
<node TEXT="实现" POSITION="right" ID="ID_45021932" CREATED="1323180363046" MODIFIED="1323180422484">
<icon BUILTIN="full-5"/>
<node TEXT="26. 尽可能延后变量定义式的出现时间" ID="ID_316757652" CREATED="1649558113860" MODIFIED="1649558133553"/>
<node TEXT="27. 尽量少做转型动作" ID="ID_532142528" CREATED="1649558140243" MODIFIED="1649558157763"/>
<node TEXT="28. 避免返回 handlers 指向对象内部成分" ID="ID_160692944" CREATED="1649558158166" MODIFIED="1649558176544"/>
<node TEXT="29. 为异常安全而努力是值得的" ID="ID_1079388430" CREATED="1649558182170" MODIFIED="1649558202027"/>
<node TEXT="30. 透彻了解 inlining 的里里外外" ID="ID_1483009815" CREATED="1649558202410" MODIFIED="1649558223705"/>
<node TEXT="31. 将文件的编译依关系降到最低" ID="ID_304702527" CREATED="1649558224157" MODIFIED="1649558250196"/>
</node>
<node TEXT="继承与面向对象设计" POSITION="right" ID="ID_991189251" CREATED="1323180367250" MODIFIED="1323180424468">
<icon BUILTIN="full-6"/>
<node TEXT="32. 确定你的 public 继承塑模出 is-a 关系 Make sure public inheritance models &quot;is-a&quot;" ID="ID_780744658" CREATED="1323180454218" MODIFIED="1323180508890"/>
<node TEXT="33. 避免遮掩继承而来的名称" ID="ID_978691900" CREATED="1323180528343" MODIFIED="1323180545890"/>
<node TEXT="34.区分接口继承和实现继承" ID="ID_1068705639" CREATED="1323180546734" MODIFIED="1323180560984"/>
<node TEXT="35. 考虑 virtual 函数以外的其他选择" ID="ID_1285280247" CREATED="1323180561531" MODIFIED="1323180595468"/>
<node TEXT="36. 绝不重新定义继承而来的 non-virtual 函数" ID="ID_784064902" CREATED="1323180603296" MODIFIED="1323180627109"/>
<node TEXT="37. 绝不重新定义继承而来的缺省参数值" ID="ID_1920671151" CREATED="1323180636406" MODIFIED="1323180662703"/>
<node TEXT="38. 通过复合塑模出 has-a 或 根据某物实现出" ID="ID_1912828530" CREATED="1323180671562" MODIFIED="1323180708625"/>
<node TEXT="39. 明智而审慎地使用 private 继承" ID="ID_355209600" CREATED="1323180711828" MODIFIED="1323180743203"/>
<node TEXT="40. 明智而审慎地使用多重继承" ID="ID_1203903066" CREATED="1323180743765" MODIFIED="1323180761296"/>
</node>
<node TEXT="模板与泛型编程" POSITION="right" ID="ID_1655747914" CREATED="1323180382625" MODIFIED="1323180426500">
<icon BUILTIN="full-7"/>
<node TEXT="41. 了解隐式接口和编译期多态" ID="ID_554955588" CREATED="1649558279314" MODIFIED="1649558298337"/>
<node TEXT="42. 了解 typename 的双重定义" ID="ID_1191807878" CREATED="1649558298593" MODIFIED="1649558309624"/>
<node TEXT="43. 学习处理模板化基类内的名称" ID="ID_1935174501" CREATED="1649558310985" MODIFIED="1649558336037"/>
<node TEXT="44. 将与参数无关的代码抽离 templates" ID="ID_464285731" CREATED="1649558336409" MODIFIED="1649558351907"/>
<node TEXT="45. 运行成员函数模板接受所有兼容类型" ID="ID_1716158933" CREATED="1649558352942" MODIFIED="1649558380882"/>
<node TEXT="46. 需要类型转换时请模板定义非成员函数" ID="ID_1819383407" CREATED="1649558381320" MODIFIED="1649558406270"/>
<node TEXT="47. 请使用 traits classes 表现类型信息" ID="ID_1172197488" CREATED="1649558407202" MODIFIED="1649558426312"/>
<node TEXT="48. 认识 template 元编程" ID="ID_437498318" CREATED="1649558431206" MODIFIED="1649558443134"/>
</node>
<node TEXT="定制 new 和 delete" POSITION="right" ID="ID_756249470" CREATED="1323180394421" MODIFIED="1323180428343">
<icon BUILTIN="full-8"/>
<node TEXT="49. 了解 new-handler 的行为" ID="ID_1037111151" CREATED="1649558450550" MODIFIED="1649558467631"/>
<node TEXT="50. 了解 new 和 delete 的合理替换时机" ID="ID_1761561508" CREATED="1649558467839" MODIFIED="1649558486502"/>
<node TEXT="51. 编写 new 和 delete 时需要固守常规" ID="ID_1375190585" CREATED="1649558486730" MODIFIED="1649558543738"/>
<node TEXT="52. 写了 placement new 也要写 placement delete" ID="ID_1447230281" CREATED="1649558544020" MODIFIED="1649558564518"/>
</node>
<node TEXT="杂项讨论" POSITION="right" ID="ID_1663073761" CREATED="1323180405203" MODIFIED="1323180430578">
<icon BUILTIN="full-9"/>
<node TEXT="53. 不要轻忽编译器的警告" ID="ID_1939508346" CREATED="1649558572128" MODIFIED="1649558590914"/>
<node TEXT="54. 让自己熟悉新的标准程序库" ID="ID_1399084790" CREATED="1649558591062" MODIFIED="1649558615075"/>
<node TEXT="55. 让自己熟悉 Boost" ID="ID_686559721" CREATED="1649558615300" MODIFIED="1649558631469"/>
</node>
</node>
</map>
