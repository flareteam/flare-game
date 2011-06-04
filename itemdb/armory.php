<?php

  include('inc_itemdb_config.php');

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
power_mod,
power_desc
from (items inner join base_items on items.base_type = base_items.id)
inner join item_mods on items.item_mod = item_mods.id
order by items.id
';

    $stmt = $db->prepare($sql);
    $stmt->execute();
    while ($row = $stmt->fetch()) {
	echo "<tr>\n";

      $name = "";
      if (!is_null($row["proper_name"]))
        $name = $row["proper_name"];
      else {
        if (!is_null($row["prefix"]))
          $name = $row["prefix"] . $row["base_name"];
        else
          $name = $row["base_name"] . $row["suffix"];
      }

	  // icon
	  echo "<td><img src=\"images/icons/" . $row["icon32"] . ".png\" /></td>\n";
	
	  // name link
	  echo "<td>";
	  echo "<a href=\"item.php?id=" . $row["id"] . "\">";
	  echo "<span class=\"" . $row["quality"] . "\">";
	  echo $name;
	  echo "</span>";
	  echo "</a>";
	  echo "</td>\n";
	
	  // level
	  echo "<td>" . $row["level"] . "</td>\n";
	
          // stats
	  if (!is_null($row["dmg_min"])) {
	    echo "<td>" . $row["dmg_min"] . "-" . $row["dmg_max"] . " damage</td>\n";
	  }
	  else if (!is_null($row["abs_min"])) {
	    if ($row["abs_min"] == $row["abs_max"])
	      echo "<td>" . $row["abs_min"] . " absorb</td>\n";
	    else
	      echo "<td>" . $row["abs_min"] . "-" . $row["abs_max"] . " absorb</td>\n";
	  }
	  else echo "<td>-</td>\n";
	
	  // bonus
	  echo "<td>";
	  if (!is_null($row["bonus_stat"])) {

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

	    echo " " . $row["bonus_stat"];
	  }
	  else echo "-";
          echo "</td>\n";
	 
          // power
          echo "<td>";
          if (!is_null($row["power_desc"]))
            echo " <span class=\"high\">" . $row["power_desc"] . "</span>";
          else echo "-";
          echo "</td>\n";
	  
          // requirement
	  echo "<td>";
	  if (!is_null($row["req_stat"])) {
	    if ($row["req_val"] > 1) {
	      if ($row["req_stat"] == "p") echo "Physical";
	      if ($row["req_stat"] == "m") echo "Mental";
	      if ($row["req_stat"] == "o") echo "Offense";
	      if ($row["req_stat"] == "d") echo "Defense";
		
		  echo " " . $row["req_val"];  
	    }
	    else echo "-";
	  }
	  else echo "-";	
          echo "</td>\n";
    
	  // value
	  echo "<td>";
	  $value = floor(($row["base_price"] * $row["price_mod"]) / 100);
	  echo $value;
	  echo " <span class=\"low\">(" . max(floor($value/4),1) . ")</span>";
	  echo "</td>\n";


	  echo "</tr>\n";
    }
    $stmt = null;
    $db = null;
  }
  
  include("inc_header.php");
?>
<h2>Armory</h2>

<table id="armory">
  <tr>
    <th>icon</th>
    <th>name</th>
    <th>level</th>
    <th>stats</th>
    <th>bonus</th>
    <th>power</th>
    <th>requirement</th>
    <th>price</th>
  </tr>
<?php list_items(); ?>
</table>

<?php
  include("inc_footer.php");
?>
