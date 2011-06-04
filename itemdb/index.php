<?php

  header('Content-type: text/plain');

  include('inc_config.php');

  function list_items() {
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
base_price,
price_mod,
base_mod,
power_mod,
power_desc
from (items inner join base_items on items.base_type = base_items.id)
inner join item_mods on items.item_mod = item_mods.id
order by items.id
';

    $stmt = $db->prepare($sql);
    $stmt->execute();
    while ($row = $stmt->fetch()) {

      echo "[item]\n";  
      echo "id=" . $row["id"] . "\n";
      $name = "";
      if (!is_null($row["proper_name"]))
        $name = $row["proper_name"];
      else {
        if (!is_null($row["prefix"]))
          $name = $row["prefix"] . $row["base_name"];
        else
          $name = $row["base_name"] . $row["suffix"];
      }

      echo "name=" . $name . "\n";
      echo "level=" . $row["level"] . "\n";
      echo "quality=" . $row["quality"] . "\n";
      echo "type=" . $row["type"] . "\n";
      if ($row["icon64"] != NULL) {
        echo "icon=" . $row["icon32"] . "," . $row["icon64"] . "\n";
      }
      else {
        echo "icon=" . $row["icon32"] . "\n";
      }
      
      if (!is_null($row["dmg_min"])) {
        echo "dmg=" . $row["dmg_min"] . "," . $row["dmg_max"] . "\n";
      }
      if (!is_null($row["abs_max"])) {
        if ($row["abs_min"] < $row["abs_max"])
          echo "abs=" . $row["abs_min"] . "," . $row["abs_max"] . "\n";
        else if ($row["abs_min"] == $row["abs_max"] && $row["abs_max"] > 0)
          echo "abs=" . $row["abs_min"] . "\n";
      }

      // display bonus
      if (!is_null($row["bonus_stat"])) {
        echo "bonus=" . $row["bonus_stat"] . ",";

        if ($row["req_stat"] == "p")
          echo $row["bonus_pval"];
        else if ($row["req_stat"] == "m")
          echo $row["bonus_mval"];
        else if ($row["req_stat"] == "o")
          echo $row["bonus_oval"];
        else if ($row["req_stat"] == "d")
          echo $row["bonus_dval"];
        else
          echo $row["bonus_art"];
        echo "\n";
      }

      // display power
      if (!is_null($row["power_mod"]))
        echo "power_mod=" . $row["power_mod"] . "\n";
      else if (!is_null($row["base_mod"]))
        echo "power_mod=" . $row["base_mod"] . "\n";       

      if (!is_null($row["power_desc"])) 
        echo "power_desc=" . $row["power_desc"] . "\n";


      // display requirement
      if (!is_null($row["req_val"]) && $row["req_val"] > 1)
        echo "req=" . $row["req_stat"] . "," . $row["req_val"] . "\n";

      if (!is_null($row["sfx"]))
        echo "sfx=" . $row["sfx"] . "\n";

      if (!is_null($row["gfx"]))
        echo "gfx=" . $row["gfx"] . "\n";

      if (!is_null($row["loot"]))
        echo "loot=" . $row["loot"] . "\n";

      if (!is_null($row["base_price"]) && !is_null($row["price_mod"]))
        echo "price=" . floor(($row["base_price"] * $row["price_mod"]) / 100) . "\n";


      echo "\n";
    }
    $stmt = null;
    $db = null;
  }
  
  list_items();
?>
