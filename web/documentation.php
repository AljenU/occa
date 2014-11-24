<?php include($_SERVER['DOCUMENT_ROOT'] . '/main.php'); ?>
<?php addHeader('OCCA: Documentation') ?>

<?php absInclude("/menu.php"); ?>

<?php absInclude("/sidebarStart.php"); ?>

<div class="entry"><a href="#Introduction">1. Introduction</a></div>
<div class="entry"><a href="#Host-API"    >2. Host-API    </a></div>
<div class="entry"><a href="#Device-API"  >3. Device-API  </a></div>

<?php absInclude("/sidebarEnd.php"); ?>

<div id="id_body" class="documentation fixed body">

  <h2 id="Introduction" class="ui dividing header"> Quick Introduction </h2>

  <h2 id="Host-API" class="ui dividing header">
    <a href="/documentation/hostAPI.php" class="dsm5 link"> Host API </a>
  </h2>

  We have A B C

  <h2 id="Device-API" class="ui dividing header">
    <a href="/documentation/deviceAPI.php" class="dsm5 link"> Device API </a>
  </h2>

</div> <!--[ id_body ]-->

<?php include($_SERVER['DOCUMENT_ROOT'] . '/footer.php'); ?>
