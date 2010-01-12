SET SQL_MODE="NO_AUTO_VALUE_ON_ZERO";

CREATE TABLE IF NOT EXISTS `cms_menu` (
  `ID` int(11) NOT NULL AUTO_INCREMENT,
  `ParentID` int(11) DEFAULT NULL,
  `Title` varchar(64) NOT NULL,
  `URL` varchar(512) NOT NULL,
  `Weight` int(11) NOT NULL DEFAULT '1',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB  DEFAULT CHARSET=latin1 AUTO_INCREMENT=13 ;

INSERT INTO `cms_menu` (`ID`, `ParentID`, `Title`, `URL`, `Weight`) VALUES
(1, NULL, 'Home', '', 1),
(2, NULL, 'Story', 'story', 2),
(3, NULL, 'The team', 'the_team', 6),
(5, NULL, 'Media', 'media', 3),
(6, 5, 'Images', 'images', 4),
(7, 5, 'Videos', 'videos', 5),
(8, NULL, 'FAQ', 'faq', 7),
(9, NULL, '---', '', 8),
(10, NULL, 'Register', 'user/register', 9),
(11, NULL, 'Login', 'user/login', 10),
(12, NULL, 'Logout', 'user/logout', 12);

CREATE TABLE IF NOT EXISTS `cms_node` (
  `ID` int(11) NOT NULL AUTO_INCREMENT,
  `Path` varchar(128) NOT NULL,
  `Data` text NOT NULL,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB  DEFAULT CHARSET=latin1 AUTO_INCREMENT=2 ;

INSERT INTO `cms_node` (`ID`, `Path`, `Data`) VALUES
(1, 'test', 'This is a database node =D');
