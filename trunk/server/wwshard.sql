-- MySQL dump 10.9
--
-- Host: 10.10.39.5    Database: wwshard
-- ------------------------------------------------------
-- Server version	3.23.58
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `characters`
--

DROP TABLE IF EXISTS `characters`;
CREATE TABLE `characters` (
  `characterid` int(10) unsigned NOT NULL auto_increment,
  `userid` int(10) unsigned NOT NULL default '0',
  `charname` varchar(45) NOT NULL default '',
  `active` tinyint(3) unsigned default '0',
  `emdtype` varchar(254) default NULL,
  PRIMARY KEY  (`characterid`)
) TYPE=MyISAM;

--
-- Dumping data for table `characters`
--


/*!40000 ALTER TABLE `characters` DISABLE KEYS */;
LOCK TABLES `characters` WRITE;
INSERT INTO `characters` VALUES (1,1,'sfb',0,'sphere guy'),(2,3,'henri',0,'sphere guy');
UNLOCK TABLES;
/*!40000 ALTER TABLE `characters` ENABLE KEYS */;

--
-- Table structure for table `users`
--

DROP TABLE IF EXISTS `users`;
CREATE TABLE `users` (
  `userid` int(10) unsigned NOT NULL default '0',
  `plrallowed` int(10) unsigned default '2',
  `state` enum('Offline','CharSelect','CharCreate','Playing') default 'Offline',
  PRIMARY KEY  (`userid`)
) TYPE=MyISAM;

--
-- Dumping data for table `users`
--


/*!40000 ALTER TABLE `users` DISABLE KEYS */;
LOCK TABLES `users` WRITE;
INSERT INTO `users` VALUES (1,2,'Offline'),(3,2,'Offline');
UNLOCK TABLES;
/*!40000 ALTER TABLE `users` ENABLE KEYS */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

