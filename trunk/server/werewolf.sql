SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT;
SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS;
SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION;
SET NAMES utf8;

/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE=NO_AUTO_VALUE_ON_ZERO */;


CREATE DATABASE /*!32312 IF NOT EXISTS*/ `werewolf`;
USE `werewolf`;
CREATE TABLE `permissions` (
  `userid` int(10) unsigned NOT NULL default '0',
  `clientapplication` varchar(45) NOT NULL default 'werewolf',
  `shardid` int(11) NOT NULL default '-1',
  PRIMARY KEY  (`userid`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
CREATE TABLE `shards` (
  `shardid` int(10) unsigned NOT NULL auto_increment,
  `wsaddr` varchar(45) default NULL,
  `plrcount` int(10) unsigned default '0',
  `name` varchar(45) default 'Unknown Shard',
  `online` tinyint(3) unsigned default '0',
  `clientapplication` varchar(45) default 'werewolf',
  `version` varchar(45) default NULL,
  `internalid` int(10) unsigned default '0',
  PRIMARY KEY  (`shardid`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
INSERT INTO `shards` (`shardid`,`wsaddr`,`plrcount`,`name`,`online`,`clientapplication`,`version`,`internalid`) VALUES 
 (1,'127.0.0.1',0,'Test Shard',1,'werewolf','0.1',256);
CREATE TABLE `users` (
  `userid` int(10) unsigned NOT NULL auto_increment,
  `username` varchar(45) NOT NULL default '',
  `passwd` varchar(45) NOT NULL default '',
  `shardid` int(11) NOT NULL default '-1',
  `state` enum('Offline','Online','Authorized','Waiting') NOT NULL default 'Offline',
  `privilege` varchar(45) default NULL,
  `cookie` varchar(45) default NULL,
  `address` varchar(45) default NULL,
  PRIMARY KEY  (`userid`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 COMMENT='Main user table.';
INSERT INTO `users` (`userid`,`username`,`passwd`,`shardid`,`state`,`privilege`,`cookie`,`address`) VALUES 
 (1,'mattr','banana',1,'Waiting',NULL,'00B6FD70|00002CD6|00000001',NULL),
 (2,'testuser','falsepass',-1,'Offline',NULL,NULL,NULL);
/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT;
SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS;
SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
