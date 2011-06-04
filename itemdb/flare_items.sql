-- MySQL dump 10.11
--
-- Host: localhost    Database: osare
-- ------------------------------------------------------
-- Server version	5.0.45

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `base_items`
--

DROP TABLE IF EXISTS `base_items`;
CREATE TABLE `base_items` (
  `id` int(11) NOT NULL,
  `name` varchar(32) default NULL,
  `level` int(11) default NULL,
  `type` varchar(16) default NULL,
  `icon32` int(11) default NULL,
  `icon64` int(11) default NULL,
  `dmg_min` int(11) default NULL,
  `dmg_max` int(11) default NULL,
  `abs_min` int(11) default NULL,
  `abs_max` int(11) default NULL,
  `req_stat` varchar(16) default NULL,
  `req_val` int(11) default NULL,
  `sfx` varchar(32) default NULL,
  `gfx` varchar(32) default NULL,
  `loot` varchar(32) default NULL,
  `base_price` int(11) default NULL,
  `base_mod` int(11) default NULL,
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Dumping data for table `base_items`
--

LOCK TABLES `base_items` WRITE;
/*!40000 ALTER TABLE `base_items` DISABLE KEYS */;
INSERT INTO `base_items` VALUES (1,'Dagger',2,'main',96,0,2,6,NULL,NULL,'p',2,'metal','dagger','dagger',10,NULL),(2,'Shortsword',4,'main',97,1,3,8,NULL,NULL,'p',3,'metal','shortsword','shortsword',20,NULL),(3,'Longsword',6,'main',98,2,4,10,NULL,NULL,'p',4,'metal','longsword','longsword',50,NULL),(4,'Greatsword',8,'main',99,3,5,12,NULL,NULL,'p',5,'metal','greatsword','greatsword',100,NULL),(5,'Wand',2,'main',100,4,2,6,NULL,NULL,'m',2,'wood','wand','wand',14,NULL),(6,'Rod',4,'main',101,5,3,8,NULL,NULL,'m',3,'wood','rod','rod',26,NULL),(7,'Staff',6,'main',102,6,4,10,NULL,NULL,'m',4,'wood','staff','staff',65,NULL),(8,'Greatstaff',8,'main',103,7,5,12,NULL,NULL,'m',5,'wood','greatstaff','greatstaff',115,NULL),(9,'Slingshot',2,'off',104,8,2,6,NULL,NULL,'o',2,'wood','slingshot','slingshot',8,120),(10,'Shortbow',4,'off',105,9,3,8,NULL,NULL,'o',3,'wood','shortbow','shortbow',24,121),(11,'Longbow',6,'off',106,10,4,10,NULL,NULL,'o',4,'wood','longbow','longbow',60,121),(12,'Greatbow',8,'off',107,11,5,12,NULL,NULL,'o',5,'wood','greatbow','greatbow',110,121),(14,'Leather Armor',2,'body',108,12,NULL,NULL,1,1,'d',2,'leather','leather_armor','leather_armor',17,NULL),(15,'Buckler',4,'off',109,13,NULL,NULL,0,2,'d',3,'wood','buckler','buckler',30,NULL),(16,'Steel Armor',6,'body',110,14,NULL,NULL,2,2,'d',4,'maille','steel_armor','steel_armor',75,NULL),(17,'Shield',8,'off',111,15,NULL,NULL,1,3,'d',5,'heavy','shield','shield',90,NULL),(18,'Belt',3,'artifact',115,19,NULL,NULL,NULL,NULL,NULL,NULL,'leather',NULL,'belt',15,NULL),(19,'Boots',4,'artifact',113,17,NULL,NULL,NULL,NULL,NULL,NULL,'leather',NULL,'boots',25,NULL),(20,'Ring',7,'artifact',114,18,NULL,NULL,NULL,NULL,NULL,NULL,'gem',NULL,'ring',80,NULL),(13,'Clothes',1,'body',112,16,NULL,NULL,0,0,'d',1,'cloth','clothes','clothes',7,NULL);
/*!40000 ALTER TABLE `base_items` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `item_mods`
--

DROP TABLE IF EXISTS `item_mods`;
CREATE TABLE `item_mods` (
  `id` int(11) NOT NULL,
  `prefix` varchar(32) default NULL,
  `suffix` varchar(32) default NULL,
  `quality` varchar(16) default NULL,
  `level` int(11) default NULL,
  `dmg_min` int(11) default NULL,
  `dmg_max` int(11) default NULL,
  `abs_min` int(11) default NULL,
  `abs_max` int(11) default NULL,
  `bonus_stat` varchar(16) default NULL,
  `bonus_pval` int(11) default NULL,
  `bonus_mval` int(11) default NULL,
  `bonus_oval` int(11) default NULL,
  `bonus_dval` int(11) default NULL,
  `effect` varchar(32) default NULL,
  `bonus_art` int(11) default NULL,
  `price_mod` int(11) default NULL,
  `power_mod` int(11) default NULL,
  `power_desc` varchar(256) default NULL,
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Dumping data for table `item_mods`
--

LOCK TABLES `item_mods` WRITE;
/*!40000 ALTER TABLE `item_mods` DISABLE KEYS */;
INSERT INTO `item_mods` VALUES (1,'Rusted ',NULL,'low',-1,-1,-1,-1,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,30,NULL,NULL),(2,'Cracked ',NULL,'low',-1,-1,-1,-1,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,30,NULL,NULL),(3,'Cursed ',NULL,'low',-1,0,0,0,0,'accuracy',-3,-3,-4,-2,NULL,-5,70,NULL,NULL),(4,'Cursed ',NULL,'low',-1,0,0,0,0,'avoidance',-3,-3,-2,-4,NULL,-5,70,NULL,NULL),(5,'Dull ',NULL,'low',-1,0,0,0,0,'crit',-2,-2,-2,-2,NULL,-2,50,NULL,NULL),(6,'Depleted ',NULL,'low',-1,0,0,0,0,'crit',-2,-2,-2,-2,NULL,-2,50,NULL,NULL),(7,'Warped ',NULL,'low',-1,0,0,0,0,'crit',-2,-2,-2,-2,NULL,-2,50,NULL,NULL),(8,'Dwarven ',NULL,'high',1,0,0,0,0,'HP',4,2,3,3,NULL,5,200,NULL,NULL),(9,NULL,' of Restoration','high',2,0,0,0,0,'HP regen',2,2,2,2,NULL,4,240,NULL,NULL),(10,'Elven ',NULL,'high',1,0,0,0,0,'MP',2,4,3,3,NULL,5,220,NULL,NULL),(11,NULL,' of Meditation','high',2,0,0,0,0,'MP regen',2,2,2,2,NULL,4,260,NULL,NULL),(12,'Battle ',NULL,'high',2,1,1,0,1,NULL,NULL,NULL,NULL,NULL,NULL,NULL,280,NULL,NULL),(13,NULL,' of Accuracy','high',1,0,0,0,0,'accuracy',3,3,4,2,NULL,5,160,NULL,NULL),(14,NULL,' of Avoidance','high',1,0,0,0,0,'avoidance',3,3,2,4,NULL,5,180,NULL,NULL),(15,'Summer ',NULL,'high',1,0,0,0,0,'fire resist',8,8,6,10,NULL,20,140,NULL,NULL),(16,'Winter ',NULL,'high',1,0,0,0,0,'ice resist',8,8,6,10,NULL,20,120,NULL,NULL),(17,NULL,' of Slaying','high',2,0,0,0,0,'crit',2,2,2,2,NULL,4,300,NULL,NULL),(18,NULL,' of Speed','high',2,0,0,0,0,'speed',1,1,1,1,NULL,3,320,NULL,NULL),(19,'Warlord ',NULL,'high',3,0,1,0,1,'HP',7,5,6,6,NULL,10,500,NULL,NULL),(20,NULL,' of Victory','high',4,0,1,0,1,'HP regen',4,4,4,4,NULL,8,560,NULL,NULL),(21,'Archmage ',NULL,'high',3,0,1,0,1,'MP',5,7,6,6,NULL,10,530,NULL,NULL),(22,NULL,' of Wizardry','high',4,0,1,0,1,'MP regen',4,4,4,4,NULL,8,590,NULL,NULL),(23,'Ancient ',NULL,'high',4,2,2,1,1,NULL,NULL,NULL,NULL,NULL,NULL,NULL,620,NULL,NULL),(24,NULL,' of Precision','high',3,0,1,0,1,'accuracy',6,6,7,5,NULL,10,440,NULL,NULL),(25,NULL,' of Protection','high',3,0,1,0,1,'avoidance',6,6,5,7,NULL,10,470,NULL,NULL),(26,'Fiery ',NULL,'high',3,0,1,0,1,'fire resist',11,11,9,15,NULL,25,410,NULL,NULL),(27,'Frozen ',NULL,'high',3,0,1,0,1,'ice resist',11,11,9,15,NULL,25,380,NULL,NULL),(28,NULL,' of Destruction','high',4,0,1,0,1,'crit',4,4,4,4,NULL,8,650,NULL,NULL),(29,NULL,' of Travel','high',4,0,1,0,1,'speed',2,2,2,2,NULL,6,680,NULL,NULL),(30,'Epic Health ',NULL,'epic',5,2,2,1,1,'HP',10,8,9,9,NULL,15,1000,NULL,NULL),(31,'Epic Health Regen ',NULL,'epic',6,2,2,1,1,'HP regen',6,6,6,6,NULL,12,1100,NULL,NULL),(32,'Epic Mana ',NULL,'epic',5,2,2,1,1,'MP',8,10,9,9,NULL,15,1050,NULL,NULL),(33,'Epic Mana Regen ',NULL,'epic',6,2,2,1,1,'MP regen',6,6,6,6,NULL,12,1150,NULL,NULL),(34,'Epic Accuracy ',NULL,'epic',5,2,2,1,1,'accuracy',9,9,10,8,NULL,15,900,NULL,NULL),(35,'Epic Avoidance ',NULL,'epic',5,2,2,1,1,'avoidance',9,9,8,10,NULL,15,950,NULL,NULL),(36,'Epic Fire Resist ',NULL,'epic',5,2,2,1,1,'fire resist',14,14,12,20,NULL,30,850,NULL,NULL),(37,'Epic Frost Resist ',NULL,'epic',5,2,2,1,1,'ice resist',14,14,12,20,NULL,30,800,NULL,NULL),(38,'Epic Crit ',NULL,'epic',6,2,2,1,1,'crit',6,6,6,6,NULL,12,1200,NULL,NULL),(39,'Epic Speed ',NULL,'epic',6,2,2,1,1,'speed',3,3,3,3,NULL,9,1250,NULL,NULL),(0,NULL,NULL,'normal',0,0,0,0,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,100,NULL,NULL),(40,'Cracked ',NULL,'low',0,0,0,0,-1,NULL,NULL,NULL,NULL,NULL,NULL,NULL,30,NULL,NULL),(41,'Tattered ',NULL,'low',0,0,0,0,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,50,NULL,NULL),(42,'Tarnished ',NULL,'low',0,0,0,0,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,50,NULL,NULL),(43,'Summer ',NULL,'high',1,0,0,0,0,'resist ice',NULL,NULL,6,NULL,NULL,NULL,140,122,'Fire Ammo'),(44,'Winter ',NULL,'high',1,0,0,0,0,'resist fire',NULL,NULL,6,NULL,NULL,NULL,120,123,'Ice Ammo'),(45,'Fiery ',NULL,'high',3,0,1,0,0,'resist ice',NULL,NULL,9,NULL,NULL,NULL,410,122,'Fire Ammo'),(46,'Frozen ',NULL,'high',3,0,1,0,0,'resist fire',NULL,NULL,9,NULL,NULL,NULL,380,123,'Ice Ammo'),(47,'Epic Fire Resist ',NULL,'epic',5,2,2,0,0,'resist ice',NULL,NULL,12,NULL,NULL,NULL,850,122,'Fire Ammo'),(48,'Epic Ice Resist ',NULL,'epic',5,2,2,0,0,'resist fire',NULL,NULL,12,NULL,NULL,NULL,800,123,'Ice Ammo');
/*!40000 ALTER TABLE `item_mods` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `items`
--

DROP TABLE IF EXISTS `items`;
CREATE TABLE `items` (
  `id` int(11) NOT NULL,
  `base_type` int(11) default NULL,
  `item_mod` int(11) default NULL,
  `name` varchar(32) default NULL,
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Dumping data for table `items`
--

LOCK TABLES `items` WRITE;
/*!40000 ALTER TABLE `items` DISABLE KEYS */;
INSERT INTO `items` VALUES (100,1,0,NULL),(101,1,1,NULL),(102,1,3,NULL),(103,1,5,NULL),(105,1,8,NULL),(106,1,9,NULL),(107,1,10,NULL),(108,1,11,NULL),(109,1,12,NULL),(110,1,13,NULL),(111,1,14,NULL),(112,1,15,NULL),(113,1,16,NULL),(114,1,17,NULL),(120,1,19,NULL),(121,1,20,NULL),(122,1,21,NULL),(123,1,22,NULL),(124,1,23,NULL),(125,1,24,NULL),(126,1,25,NULL),(127,1,26,NULL),(128,1,27,NULL),(129,1,28,NULL),(135,1,30,'Sacrificial Knife'),(136,1,31,'Thorn'),(137,1,32,'Mind Shard'),(138,1,33,'Ether Slice'),(139,1,34,'Pinpoint'),(140,1,35,'Dancing Knife'),(141,1,36,'Fire Shard'),(142,1,37,'Ice Shard'),(143,1,38,'Goblin Carver'),(150,2,0,NULL),(151,2,1,NULL),(152,2,3,NULL),(153,2,5,NULL),(155,2,8,NULL),(156,2,9,NULL),(157,2,10,NULL),(158,2,11,NULL),(159,2,12,NULL),(160,2,13,NULL),(161,2,14,NULL),(162,2,15,NULL),(163,2,16,NULL),(164,2,17,NULL),(170,2,19,NULL),(171,2,20,NULL),(172,2,21,NULL),(173,2,22,NULL),(174,2,23,NULL),(175,2,24,NULL),(176,2,25,NULL),(177,2,26,NULL),(178,2,27,NULL),(179,2,28,NULL),(185,2,30,'Gladius'),(186,2,31,'Mending Blade'),(187,2,32,'Mana Edge'),(188,2,33,'Demon Pact'),(189,2,34,'Watchman\'s Blade'),(190,2,35,'Parry Blade'),(191,2,36,'Summersword'),(192,2,37,'Wintersword'),(193,2,38,'Sting'),(200,3,0,NULL),(201,3,1,NULL),(202,3,3,NULL),(203,3,5,NULL),(205,3,8,NULL),(206,3,9,NULL),(207,3,10,NULL),(208,3,11,NULL),(209,3,12,NULL),(210,3,13,NULL),(211,3,14,NULL),(212,3,15,NULL),(213,3,16,NULL),(214,3,17,NULL),(220,3,19,NULL),(221,3,20,NULL),(222,3,21,NULL),(223,3,22,NULL),(224,3,23,NULL),(225,3,24,NULL),(226,3,25,NULL),(227,3,26,NULL),(228,3,27,NULL),(229,3,28,NULL),(235,3,30,'Excalibur'),(236,3,31,'Vampiric Sword'),(237,3,32,'Glamdring'),(238,3,33,'Durendal'),(239,3,34,'Rapier'),(240,3,35,'Dueling Sword'),(241,3,36,'Flame Edge'),(242,3,37,'Frost Edge'),(243,3,38,'Vorpal Sword'),(250,4,0,NULL),(251,4,1,NULL),(252,4,3,NULL),(253,4,5,NULL),(255,4,8,NULL),(256,4,9,NULL),(257,4,10,NULL),(258,4,11,NULL),(259,4,12,NULL),(260,4,13,NULL),(261,4,14,NULL),(262,4,15,NULL),(263,4,16,NULL),(264,4,17,NULL),(270,4,19,NULL),(271,4,20,NULL),(272,4,21,NULL),(273,4,22,NULL),(274,4,23,NULL),(275,4,24,NULL),(276,4,25,NULL),(277,4,26,NULL),(278,4,27,NULL),(279,4,28,NULL),(285,4,30,'Ragnarok'),(286,4,31,'Earth Rend'),(287,4,32,'Joyeuse'),(288,4,33,'Soul Rend'),(289,4,34,'Hrunting'),(290,4,35,'Naegling'),(291,4,36,'Sunblade'),(292,4,37,'Moonblade'),(293,4,38,'Armageddon'),(300,5,0,NULL),(301,5,2,NULL),(302,5,3,NULL),(303,5,6,NULL),(305,5,8,NULL),(306,5,9,NULL),(307,5,10,NULL),(308,5,11,NULL),(309,5,12,NULL),(310,5,13,NULL),(311,5,14,NULL),(312,5,15,NULL),(313,5,16,NULL),(314,5,17,NULL),(320,5,19,NULL),(321,5,20,NULL),(322,5,21,NULL),(323,5,22,NULL),(324,5,23,NULL),(325,5,24,NULL),(326,5,25,NULL),(327,5,26,NULL),(328,5,27,NULL),(329,5,28,NULL),(335,5,30,'Tongue Depressor'),(336,5,31,'Living Branch'),(337,5,32,'Glow Stick'),(338,5,33,'Mana Catcher'),(339,5,34,'Conductor\'s Baton'),(340,5,35,'Blink Wand'),(341,5,36,'Enchanted Torch'),(342,5,37,'Snowy Branch'),(343,5,38,'Lich Finger'),(350,6,0,NULL),(351,6,2,NULL),(352,6,3,NULL),(353,6,6,NULL),(355,6,8,NULL),(356,6,9,NULL),(357,6,10,NULL),(358,6,11,NULL),(359,6,12,NULL),(360,6,13,NULL),(361,6,14,NULL),(362,6,15,NULL),(363,6,16,NULL),(364,6,17,NULL),(370,6,19,NULL),(371,6,20,NULL),(372,6,21,NULL),(373,6,22,NULL),(374,6,23,NULL),(375,6,24,NULL),(376,6,25,NULL),(377,6,26,NULL),(378,6,27,NULL),(379,6,28,NULL),(385,6,30,'Scepter of Kings'),(386,6,31,'Medic Rod'),(387,6,32,'Sage Scepter'),(388,6,33,'Oracle Rod'),(389,6,34,'Rod of Alignment'),(390,6,35,'Warding Rod'),(391,6,36,'Ruby-Tipped Cane'),(392,6,37,'Diamond-Tipped Cane'),(393,6,38,'Unwraveller'),(400,7,0,NULL),(401,7,2,NULL),(402,7,3,NULL),(403,7,6,NULL),(405,7,8,NULL),(406,7,9,NULL),(407,7,10,NULL),(408,7,11,NULL),(409,7,12,NULL),(410,7,13,NULL),(411,7,14,NULL),(412,7,15,NULL),(413,7,16,NULL),(414,7,17,NULL),(420,7,19,NULL),(421,7,20,NULL),(422,7,21,NULL),(423,7,22,NULL),(424,7,23,NULL),(425,7,24,NULL),(426,7,25,NULL),(427,7,26,NULL),(428,7,27,NULL),(429,7,28,NULL),(435,7,30,'Walking Stick'),(436,7,31,'Totem of Life'),(437,7,32,'Cosmic Staff'),(438,7,33,'Totem of Dreams'),(439,7,34,'Staff of True Sight'),(440,7,35,'Staff of Trials'),(441,7,36,'Magma Flow'),(442,7,37,'Glacial Wind'),(443,7,38,'Wyvern Spine'),(450,8,0,NULL),(451,8,2,NULL),(452,8,3,NULL),(453,8,6,NULL),(455,8,8,NULL),(456,8,9,NULL),(457,8,10,NULL),(458,8,11,NULL),(459,8,12,NULL),(460,8,13,NULL),(461,8,14,NULL),(462,8,15,NULL),(463,8,16,NULL),(464,8,17,NULL),(470,8,19,NULL),(471,8,20,NULL),(472,8,21,NULL),(473,8,22,NULL),(474,8,23,NULL),(475,8,24,NULL),(476,8,25,NULL),(477,8,26,NULL),(478,8,27,NULL),(479,8,28,NULL),(485,8,30,'Lifegiver'),(486,8,31,'Preserver'),(487,8,32,'Enlightenment'),(488,8,33,'Defiler'),(489,8,34,'Precognition'),(490,8,35,'Singularity'),(491,8,36,'Fission'),(492,8,37,'Fusion'),(493,8,38,'Cataclysm'),(500,9,0,NULL),(585,10,30,'Orc Chieftan'),(579,10,28,NULL),(578,10,46,NULL),(577,10,45,NULL),(576,10,25,NULL),(575,10,24,NULL),(574,10,23,NULL),(573,10,22,NULL),(572,10,21,NULL),(571,10,20,NULL),(570,10,19,NULL),(564,10,17,NULL),(563,10,44,NULL),(562,10,43,NULL),(561,10,14,NULL),(560,10,13,NULL),(559,10,12,NULL),(558,10,11,NULL),(557,10,10,NULL),(556,10,9,NULL),(555,10,8,NULL),(553,10,7,NULL),(552,10,3,NULL),(551,10,2,NULL),(550,10,0,NULL),(543,9,38,'Giant Slayer'),(542,9,48,'Snowball Slinger'),(541,9,47,'Ember Slinger'),(540,9,35,'Swift Sling'),(539,9,34,'Urchin Sling'),(538,9,33,'Wind Sling'),(537,9,32,'Willow Branch'),(536,9,31,'Rock Sling'),(535,9,30,'Ash Branch'),(529,9,28,NULL),(528,9,46,NULL),(527,9,45,NULL),(526,9,25,NULL),(525,9,24,NULL),(524,9,23,NULL),(523,9,22,NULL),(522,9,21,NULL),(521,9,20,NULL),(520,9,19,NULL),(514,9,17,NULL),(513,9,44,NULL),(512,9,43,NULL),(511,9,14,NULL),(510,9,13,NULL),(627,11,45,NULL),(626,11,25,NULL),(625,11,24,NULL),(624,11,23,NULL),(623,11,22,NULL),(622,11,21,NULL),(621,11,20,NULL),(620,11,19,NULL),(614,11,17,NULL),(613,11,44,NULL),(612,11,43,NULL),(611,11,14,NULL),(610,11,13,NULL),(609,11,12,NULL),(608,11,11,NULL),(607,11,10,NULL),(606,11,9,NULL),(605,11,8,NULL),(603,11,7,NULL),(602,11,3,NULL),(601,11,2,NULL),(600,11,0,NULL),(593,10,38,'Assassin'),(592,10,48,'Ice Striker'),(591,10,47,'Fire Striker'),(590,10,35,'Mithril Bow'),(589,10,34,'Bandit'),(588,10,33,'Feywild Bow'),(587,10,32,'Witch Doctor'),(586,10,31,'Crag Bow'),(509,9,12,NULL),(508,9,11,NULL),(507,9,10,NULL),(506,9,9,NULL),(505,9,8,NULL),(503,9,7,NULL),(502,9,3,NULL),(501,9,2,NULL),(628,11,46,NULL),(629,11,28,NULL),(635,11,30,'Thunder'),(636,11,31,'Bow of the World Tree'),(637,11,32,'Lightning'),(638,11,33,'Pharaoh'),(639,11,34,'Ranger\'s Bow'),(640,11,35,'Siege Bow'),(641,11,47,'Ruby Strand'),(642,11,48,'Diamond Strand'),(643,11,38,'Death Mark'),(650,12,0,NULL),(651,12,2,NULL),(652,12,3,NULL),(653,12,7,NULL),(655,12,8,NULL),(656,12,9,NULL),(657,12,10,NULL),(658,12,11,NULL),(659,12,12,NULL),(660,12,13,NULL),(661,12,14,NULL),(662,12,43,NULL),(663,12,44,NULL),(664,12,17,NULL),(670,12,19,NULL),(671,12,20,NULL),(672,12,21,NULL),(673,12,22,NULL),(674,12,23,NULL),(675,12,24,NULL),(676,12,25,NULL),(677,12,45,NULL),(678,12,46,NULL),(679,12,28,NULL),(685,12,30,'Dragonslayer'),(686,12,31,'Vampire Hunter'),(687,12,32,'Dimensional Rift'),(688,12,33,'Vortex'),(689,12,34,'Gladys'),(690,12,35,'Ballista'),(691,12,47,'Starfire'),(692,12,48,'Voidfire'),(693,12,38,'Chaos'),(50,13,0,NULL),(51,13,41,NULL),(55,13,8,NULL),(56,13,9,NULL),(57,13,10,NULL),(58,13,11,NULL),(59,13,12,NULL),(60,13,13,NULL),(61,13,14,NULL),(62,13,15,NULL),(63,13,16,NULL),(64,13,17,NULL),(70,13,19,NULL),(71,13,20,NULL),(72,13,21,NULL),(73,13,22,NULL),(74,13,23,NULL),(75,13,24,NULL),(76,13,25,NULL),(77,13,26,NULL),(78,13,27,NULL),(79,13,28,NULL),(85,13,30,'Butcher\'s Apron'),(86,13,31,'Medic Wrap'),(87,13,32,'Wizard Tunic'),(88,13,33,'Spellwoven Clothes'),(89,13,34,'Hunter\'s Garb'),(90,13,35,'Night Watch'),(91,13,36,'Nomad Rags'),(92,13,37,'Fur-Lined Tunic'),(93,13,38,'Vigilante'),(700,14,0,NULL),(701,14,2,NULL),(702,14,4,NULL),(705,14,8,NULL),(706,14,9,NULL),(707,14,10,NULL),(708,14,11,NULL),(709,14,12,NULL),(710,14,13,NULL),(711,14,14,NULL),(712,14,15,NULL),(713,14,16,NULL),(714,14,17,NULL),(720,14,19,NULL),(721,14,20,NULL),(722,14,21,NULL),(723,14,22,NULL),(724,14,23,NULL),(725,14,24,NULL),(726,14,25,NULL),(727,14,26,NULL),(728,14,27,NULL),(729,14,28,NULL),(735,14,30,'Barbarian Wrap'),(736,14,31,'Bugbear Tunic'),(737,14,32,'Runic Leather'),(738,14,33,'Battlemage Tunic'),(739,14,34,'Predator Hide'),(740,14,35,'Chimera Hide'),(741,14,36,'Red Dragonhide'),(742,14,37,'White Dragonhide'),(743,14,38,'Ninja Gi'),(750,15,0,NULL),(751,15,40,NULL),(752,15,4,NULL),(755,15,8,NULL),(756,15,9,NULL),(757,15,10,NULL),(758,15,11,NULL),(759,15,12,NULL),(760,15,13,NULL),(761,15,14,NULL),(762,15,15,NULL),(763,15,16,NULL),(764,15,17,NULL),(770,15,19,NULL),(771,15,20,NULL),(772,15,21,NULL),(773,15,22,NULL),(774,15,23,NULL),(775,15,24,NULL),(776,15,25,NULL),(777,15,26,NULL),(778,15,27,NULL),(779,15,28,NULL),(785,15,30,'Gladiator Buckler'),(786,15,31,'Holy Targe'),(787,15,32,'Averguard Disc'),(788,15,33,'Mage Targe'),(789,15,34,'Focus Buckler'),(790,15,35,'Guardian Targe'),(791,15,36,'Heat Barrier'),(792,15,37,'Cold Barrier'),(793,15,38,'Spiked Buckler'),(800,16,0,NULL),(801,16,2,NULL),(802,16,4,NULL),(805,16,8,NULL),(806,16,9,NULL),(807,16,10,NULL),(808,16,11,NULL),(809,16,12,NULL),(810,16,13,NULL),(811,16,14,NULL),(812,16,15,NULL),(813,16,16,NULL),(814,16,17,NULL),(820,16,19,NULL),(821,16,20,NULL),(822,16,21,NULL),(823,16,22,NULL),(824,16,23,NULL),(825,16,24,NULL),(826,16,25,NULL),(827,16,26,NULL),(828,16,27,NULL),(829,16,28,NULL),(835,16,30,'Knight\'s Cuirass'),(836,16,31,'Wyrmrider Maille'),(837,16,32,'Paladin\'s Oath'),(838,16,33,'Cleric Maille'),(839,16,34,'Jousting Armor'),(840,16,35,'Titan Plate'),(841,16,36,'Ruby Scale Armor'),(842,16,37,'Diamond Scale Armor'),(843,16,38,'Marauder Maille'),(850,17,0,NULL),(851,17,2,NULL),(852,17,4,NULL),(855,17,8,NULL),(856,17,9,NULL),(857,17,10,NULL),(858,17,11,NULL),(859,17,12,NULL),(860,17,13,NULL),(861,17,14,NULL),(862,17,15,NULL),(863,17,16,NULL),(864,17,17,NULL),(870,17,19,NULL),(871,17,20,NULL),(872,17,21,NULL),(873,17,22,NULL),(874,17,23,NULL),(875,17,24,NULL),(876,17,25,NULL),(877,17,26,NULL),(878,17,27,NULL),(879,17,28,NULL),(885,17,30,'Knight\'s Defense'),(886,17,31,'Aegis of Life'),(887,17,32,'Paladin\'s Honor'),(888,17,33,'Aegis of Ether'),(889,17,34,'Steel Ward'),(890,17,35,'The Rook'),(891,17,36,'Flame Tower'),(892,17,37,'Frost Tower'),(893,17,38,'Blood Ward'),(900,18,0,NULL),(901,18,41,NULL),(902,18,8,NULL),(903,18,10,NULL),(904,18,19,NULL),(905,18,21,NULL),(906,18,30,'Trollhair Belt'),(907,18,32,'Spellpouch Belt'),(916,19,39,'Windwalker Boots'),(915,19,25,NULL),(914,19,29,NULL),(913,19,14,NULL),(912,19,18,NULL),(911,19,41,NULL),(910,19,0,NULL),(917,19,35,'Phase Step Boots'),(940,20,0,NULL),(941,20,42,NULL),(942,20,9,NULL),(943,20,11,NULL),(944,20,20,NULL),(945,20,22,NULL),(946,20,31,'Signet of Forgotten Kings'),(947,20,33,'Band of Enlightenment');
/*!40000 ALTER TABLE `items` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2011-03-22  5:33:58
