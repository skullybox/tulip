# database configuration settings

CREATE TABLE user (uid int(11) unique NOT NULL AUTO_INCREMENT, ctime datetime NOT NULL DEFAULT CURRENT_TIMESTAMP, name varchar(50) unique NOT NULL, email varchar(50) NOT NULL, password varchar(25) NOT NULL, salt varchar(25) NOT NULL, PRIMARY KEY (uid));

create table friend_request(uid int(11), user_from varchar(30), ctime DATETIME not null DEFAULT now());

create table friend_list(uid int(11), friend varchar(30), ctime DATETIME not null DEFAULT now(), foreign key (uid) references user(uid));

create table message(uid int(11), frm varchar(30), ctime DATETIME not null DEFAULT now(), msg text, foreign key (uid) references user(uid));

insert into user set name = "admin",email = "admin@tulipsecure.com",password = "tulipPa55",salt = "verysalty";

