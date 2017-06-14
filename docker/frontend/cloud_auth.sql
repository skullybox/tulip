# ************************************************************
# Sequel Pro SQL dump
# Version 4541
#
# http://www.sequelpro.com/
# https://github.com/sequelpro/sequelpro
#
# Host: 127.0.0.1 (MySQL 5.7.17)
# Database: cloud_auth
# Generation Time: 2017-06-14 17:06:47 +0000
# ************************************************************


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;


# Dump of table auth
# ------------------------------------------------------------

DROP TABLE IF EXISTS `auth`;

CREATE TABLE `auth` (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT,
  `password` text NOT NULL,
  PRIMARY KEY (`id`),
  CONSTRAINT `auth_ibfk_1` FOREIGN KEY (`id`) REFERENCES `user` (`id`) ON DELETE NO ACTION ON UPDATE NO ACTION
) ENGINE=InnoDB DEFAULT CHARSET=utf8;



# Dump of table dept
# ------------------------------------------------------------

DROP TABLE IF EXISTS `dept`;

CREATE TABLE `dept` (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT,
  `name` varchar(30) NOT NULL DEFAULT '',
  `active` bit(1) NOT NULL DEFAULT b'1',
  PRIMARY KEY (`id`),
  UNIQUE KEY `name` (`name`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;



# Dump of table group
# ------------------------------------------------------------

DROP TABLE IF EXISTS `group`;

CREATE TABLE `group` (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT,
  `name` varchar(30) NOT NULL DEFAULT '',
  PRIMARY KEY (`id`),
  UNIQUE KEY `name` (`name`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;



# Dump of table user
# ------------------------------------------------------------

DROP TABLE IF EXISTS `user`;

CREATE TABLE `user` (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT,
  `uname` varchar(30) NOT NULL,
  `fname` varchar(30) NOT NULL DEFAULT '',
  `lname` varchar(30) NOT NULL DEFAULT '',
  `active` bit(1) NOT NULL DEFAULT b'1',
  `comment` varchar(120) DEFAULT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `uname` (`uname`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;



# Dump of table user_map
# ------------------------------------------------------------

DROP TABLE IF EXISTS `user_map`;

CREATE TABLE `user_map` (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT,
  `auth_id` int(11) unsigned NOT NULL,
  `dept_id` int(11) unsigned NOT NULL,
  `groups` text,
  PRIMARY KEY (`id`),
  KEY `auth_id` (`auth_id`),
  KEY `dept_id` (`dept_id`),
  CONSTRAINT `user_map_ibfk_1` FOREIGN KEY (`auth_id`) REFERENCES `auth` (`id`),
  CONSTRAINT `user_map_ibfk_2` FOREIGN KEY (`dept_id`) REFERENCES `dept` (`id`),
  CONSTRAINT `user_map_ibfk_3` FOREIGN KEY (`id`) REFERENCES `user` (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;




/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;
/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
