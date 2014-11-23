<div class="ui top menu" id="id_topMenu">
  <div class="top wrapper">
    <a class="item" id="id_logo_div" href="/index.php">
      <img id="id_top_logo"    class="top logo"    src="/images/blueOccaLogo.png"/>
      <img id="id_bottom_logo" class="bottom logo" src="/images/blackOccaLogo.png"/>
    </a>

    <div class="right menu">
      <a class="light topMenu black item <?php addSelected('getStarted') ?>"
         href="/getStarted.php">
        Get Started
      </a>

      <a class="light topMenu black item <?php addSelected('downloads') ?>"
         href="/downloads.php">
        Downloads
      </a>

      <a class="light topMenu black item <?php addSelected('tutorials') ?>"
         href="/tutorials.php">
        Tutorials
      </a>

	    <div class="ui light topMenu black simple dropdown item <?php addSelected('documentation') ?>">
        <a class="topMenu" href="/documentation.php">
          Documentation
        </a>
        <div class="menu">
          <a class="item" href="/documentation/hostAPI.php">Host API</a>
          <a class="item" href="/documentation/devAPI.php">Device API</a>
        </div>
      </div>

	    <a class="light topMenu black item <?php addSelected('aboutUs') ?>"
         href="/aboutUs.php">
        About Us
      </a>
    </div> <!--[ right menu ]-->
  </div> <!--[ top wrapper ]-->
</div> <!--[ id_topMenu ]-->