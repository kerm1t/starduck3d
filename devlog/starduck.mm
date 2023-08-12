<map version="1.1.0">
<!-- To view this file, download free mind mapping software FreeMind from http://freemind.sourceforge.net -->
<node CREATED="1641574178423" ID="ID_1162147495" MODIFIED="1641574197307" TEXT="starduck">
<node CREATED="1641575101856" ID="ID_349878192" MODIFIED="1641713314560" POSITION="right" TEXT="obj.hpp">
<node CREATED="1641575125437" ID="ID_243556471" MODIFIED="1641575150142" TEXT="obj_billboard.hpp"/>
<node CREATED="1641575138271" ID="ID_1484630121" MODIFIED="1641575157729" TEXT="obj_tunnel.hpp"/>
</node>
<node CREATED="1641574212162" ID="ID_1725651945" MODIFIED="1641714478864" POSITION="right" TEXT="rnd_shaderman.hpp">
<node CREATED="1641574276871" ID="ID_1670545685" MODIFIED="1641714484432" TEXT="initshader1()">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      standard
    </p>
  </body>
</html></richcontent>
<node CREATED="1641574234876" ID="ID_257320488" MODIFIED="1641574242838" TEXT="1 vertex shader"/>
<node CREATED="1641574249213" ID="ID_420001395" MODIFIED="1641574253667" TEXT="1 fragment shader"/>
</node>
<node CREATED="1641574302599" ID="ID_1772745187" MODIFIED="1641714487619" TEXT="initshader2()">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      help screen
    </p>
  </body>
</html></richcontent>
</node>
</node>
<node CREATED="1641577564369" ID="ID_127168499" MODIFIED="1641577568804" POSITION="right" TEXT="inc_render.h">
<node CREATED="1641577575595" ID="ID_1600404846" MODIFIED="1641582774160" TEXT="c_VAO">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      <font size="3">this is a storage for VAO </font>
    </p>
    <p>
      
    </p>
    <p>
      <font size="3">VAO OpenGL 3.0 reading prescription for VBO </font>
    </p>
    <p>
      <font size="3">- pos 3 float </font>
    </p>
    <p>
      <font size="3">- normal 3 float </font>
    </p>
    <p>
      <font size="3">- texture 2 float </font>
    </p>
    <p>
      <font size="3">- VBO OpenGL 1.5 (stored in VRAM of GPU) </font>
    </p>
    <p>
      <font size="3">&#160;&#160;&#160;&#160;flexibility of vertexarray </font>
    </p>
    <p>
      <font size="3">&#160;&#160;&#160;&#160;speed of displaylist (compiled, cannot be changed)&#160;&#160;- OpenGL 3.1 </font>
    </p>
    <p>
      
    </p>
    <p>
      <font size="3">----- </font>
    </p>
    <p>
      <font size="3">(1)&#160;&#160;&#160;&#160;&#160;&#160;&#160;&#160;&#160;&#160;&#160;&#160;&#160;glBegin() ... glEnd() </font>
    </p>
    <p>
      <font size="3">(2) vertexarray&#160;&#160;&#160;&#160;&#160;&#160;&#160;&#160;&#160;&#160;&#160;&#160;&#160;&#160;&#160;&#160;--&gt; glDrawArrays() </font>
    </p>
    <p>
      <font size="3">(3) displaylist </font>
    </p>
    <p>
      <font size="3">(4) VBO<br size="3" />(5) VAO </font>
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1641577802187" FOLDED="true" ID="ID_542585515" MODIFIED="1641583238499" TEXT="VAO">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      <font size="3">goes along with a shader program to define how to draw </font>
    </p>
    <p>
      
    </p>
    <p>
      <font size="3">https://antongerdelan.net/opengl/hellotriangle.html </font>
    </p>
  </body>
</html></richcontent>
<node CREATED="1641577832702" ID="ID_1248498792" MODIFIED="1641577834745" TEXT="VBO">
<node CREATED="1641577806750" ID="ID_349535865" MODIFIED="1641582925550" TEXT="pos 3 float"/>
<node CREATED="1641577812000" ID="ID_952486398" MODIFIED="1641577830914" TEXT="normal 3 float"/>
<node CREATED="1641577815937" ID="ID_440252432" MODIFIED="1641582921644" TEXT="texture 2 float"/>
</node>
</node>
</node>
<node CREATED="1641707780969" ID="ID_1897054439" MODIFIED="1641708142506" POSITION="left" TEXT="proj.cpp/.h">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      <font size="3">create objects </font>
    </p>
    <ul>
      <li>
        <font size="3">groundplane</font>
      </li>
      <li>
        <font size="3">scene</font>
      </li>
      <li>
        <font size="3">curbstone</font>
      </li>
      <li>
        <font size="3">guardrail</font>
      </li>
      <li>
        <font size="3">tunnel</font>
      </li>
      <li>
        <font size="3">traffic signs</font>
      </li>
      <li>
        <font size="3">red car </font>
      </li>
      <li>
        <font size="3">house </font>
      </li>
      <li>
        <font size="3">conference room </font>
      </li>
      <li>
        <font size="3">anton's village </font>
      </li>
      <li>
        <font size="3">scene objs (edited) </font>
      </li>
      <li>
        <font size="3">billboards </font>
      </li>
      <li>
        <font size="3">20 random holzstapel </font>
      </li>
      <li>
        <font size="3">conticar </font>
      </li>
      <li>
        <font size="3">black jeep </font>
      </li>
      <li>
        <font size="3">jeep </font>
      </li>
      <li>
        <font size="3">barriers </font>
      </li>
      <li>
        <font size="3">sponza</font>
      </li>
    </ul>
  </body>
</html>
</richcontent>
<node CREATED="1641712202793" ID="ID_413865472" MODIFIED="1641712240250" TEXT="init()">
<node CREATED="1641708143704" ID="ID_648650322" MODIFIED="1641708516867" TEXT="load_scene_and_objs_to_vbo()">
<node CREATED="1641708204703" ID="ID_311040591" MODIFIED="1641708208898" TEXT="goundplane"/>
<node CREATED="1641708197902" ID="ID_1625984879" MODIFIED="1641708200440" TEXT="scene"/>
<node CREATED="1641708231213" ID="ID_771708325" MODIFIED="1641712301459" TEXT="load_scene_objs()">
<icon BUILTIN="ksmiletris"/>
</node>
<node CREATED="1641708248384" ID="ID_130280245" MODIFIED="1641708251391" TEXT="billboards"/>
<node CREATED="1641708258270" ID="ID_1017133715" MODIFIED="1641708259902" TEXT="..."/>
</node>
<node CREATED="1641708170261" ID="ID_954906614" MODIFIED="1641708520818" TEXT="bind_vbos_to_vaos()"/>
<node CREATED="1641712241432" ID="ID_384104022" MODIFIED="1641712253595" TEXT="m_render_.init()"/>
</node>
<node CREATED="1641708369311" ID="ID_1759119007" MODIFIED="1641708747904" TEXT="doit()">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      collision check --&gt; move elsewhere
    </p>
    <p>
      autodrive --&gt; move elsewhere
    </p>
  </body>
</html>
</richcontent>
<node CREATED="1641708725431" ID="ID_1157614146" MODIFIED="1641708733710" TEXT="draw_VAOs_Neu"/>
<node CREATED="1641708312301" ID="ID_855409138" MODIFIED="1641708740284" TEXT="mouse_2d_to_3d()">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      move elsewhere
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1641708348106" ID="ID_1604579340" MODIFIED="1641708743314" TEXT="draw_imgui()">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      move elsewhere
    </p>
  </body>
</html>
</richcontent>
</node>
</node>
<node CREATED="1641709066953" ID="ID_1150428861" MODIFIED="1641712305178" TEXT="save_scene_objects()">
<icon BUILTIN="ksmiletris"/>
</node>
</node>
<node CREATED="1641707979438" ID="ID_1065142652" MODIFIED="1641707986229" POSITION="right" TEXT="buggyboy.h"/>
<node CREATED="1641713112917" ID="ID_556913952" MODIFIED="1641713289217" POSITION="left" TEXT="proj_m3uloader">
<icon BUILTIN="help"/>
<node CREATED="1641713171976" ID="ID_864872217" MODIFIED="1641713187123" TEXT="Load()">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      not referenced ??
    </p>
  </body>
</html>
</richcontent>
</node>
</node>
<node CREATED="1641713301147" ID="ID_388411909" MODIFIED="1641714599373" POSITION="left" TEXT="proj_render">
<node CREATED="1641713325722" ID="ID_1086312640" MODIFIED="1641713349633" TEXT="Render()">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      not much happen here
    </p>
  </body>
</html>
</richcontent>
</node>
<node CREATED="1641714430639" ID="ID_1807250028" MODIFIED="1641714434121" TEXT="Init()">
<node CREATED="1641714438777" ID="ID_386774388" MODIFIED="1641714543820" TEXT="InitShader1()">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      rnd_shaderman.hpp
    </p>
  </body>
</html>
</richcontent>
</node>
<node CREATED="1641714438777" ID="ID_247497395" MODIFIED="1641714451117" TEXT="InitShader2()"/>
</node>
<node CREATED="1641714567372" ID="ID_356057292" MODIFIED="1641714572388" TEXT="Overlays2D()"/>
<node CREATED="1641714583932" ID="ID_1893762074" MODIFIED="1641714587276" TEXT="Bind_NEW__VBOs_to_VAOs()"/>
<node CREATED="1641714599377" ID="ID_1809613134" MODIFIED="1641714603798" TEXT="Bind_VBOs_to_VAOs()"/>
</node>
</node>
</map>
