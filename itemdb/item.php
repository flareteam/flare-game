<?php

  include('inc_itemdb_config.php');

  function display_item() {
  
    if (!isset($_GET["id"])) {
        echo "<p>Item id not specified.</p>\n";
        return;
    }
    
    $item_id = $_GET["id"];
  
    $db = new PDO(DBCONNECT, RW_USER, RW_PASS);
    $sql = '
select
items.id,
items.name proper_name,
prefix,
base_items.name base_name,
suffix,
(base_items.level + item_mods.level) level,
quality,
type,
icon32,
icon64,
(base_items.dmg_min + item_mods.dmg_min) dmg_min,
(base_items.dmg_max + item_mods.dmg_max) dmg_max,
(base_items.abs_min + item_mods.abs_min) abs_min,
(base_items.abs_max + item_mods.abs_max) abs_max,
effect,
req_stat,
req_val,
bonus_stat,
bonus_pval,
bonus_mval,
bonus_oval,
bonus_dval,
bonus_art,
sfx,
gfx,
loot,
power_desc
from (items inner join base_items on items.base_type = base_items.id)
inner join item_mods on items.item_mod = item_mods.id
where items.id = ?
';

    $stmt = $db->prepare($sql);
    $stmt->bindParam(1,$item_id);
    $stmt->execute();
    while ($row = $stmt->fetch()) {

      // icon
      echo "<td><img src=\"images/icons/" . $row["icon64"] . ".png\" style=\"float:left;\" /></td>\n";
      
      echo "<div id=\"tooltip\">\n";
    
      // determine name
      $name = "";
      if (!is_null($row["proper_name"]))
        $name = $row["proper_name"];
      else {
        if (!is_null($row["prefix"]))
          $name = $row["prefix"] . $row["base_name"];
        else
          $name = $row["base_name"] . $row["suffix"];
      }

    
      // name
      echo "<span class=\"" . $row["quality"] . "\">";
      echo $name;
      echo "</span><br />\n";
    
      // level
      echo "Level " . $row["level"] . "<br />\n";
    
      // stats
      if (!is_null($row["dmg_min"])) {
        echo $row["dmg_min"] . "-" . $row["dmg_max"] . " damage<br />\n";
      }
      else if (!is_null($row["abs_min"])) {
        if ($row["abs_min"] == $row["abs_max"]) {
          if ($row["abs_min"] != 0)
            echo $row["abs_min"] . " absorb<br />\n";
        }
        else
          echo $row["abs_min"] . "-" . $row["abs_max"] . " absorb<br />\n";
      }
    
      // bonus
      if (!is_null($row["bonus_stat"])) {
        $val = 0;
        if ($row["req_stat"] == "p")
              $val = $row["bonus_pval"];
        else if ($row["req_stat"] == "m")
          $val = $row["bonus_mval"];
        else if ($row["req_stat"] == "o")
          $val = $row["bonus_oval"];
        else if ($row["req_stat"] == "d")
              $val = $row["bonus_dval"];
        else
          $val = $row["bonus_art"];

        if ($val < 0) {
              echo "Decreases ";
              $val = $val * -1;
        }
        else echo "Increases ";
        echo $row["bonus_stat"];
        echo " by ";
        echo $val;
        echo "<br />\n";
      }

      // power
      if (!is_null($row["power_desc"])) {
         echo "<span class=\"high\">";
         echo $row["power_desc"];
         echo "</span>";
         echo "<br />\n";
      }

      // requirement
      if (!is_null($row["req_stat"])) {
        if ($row["req_val"] > 1) {
          echo "Requires ";
          
          if ($row["req_stat"] == "p") echo "Physical";
          else if ($row["req_stat"] == "m") echo "Mental";
          else if ($row["req_stat"] == "o") echo "Offense";
          else if ($row["req_stat"] == "d") echo "Defense";
        
          echo " " . $row["req_val"];  
        }
      }
      
      echo "</div>\n";
    }
    $stmt = null;
    $db = null;
  }
  
  include("inc_header.php");
?>
<h2>Item</h2>

<?php display_item(); ?>
</table>

<?php
  include("inc_footer.php");
?>
