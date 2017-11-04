# database configuration settings

CREATE TABLE user (uid varchar(30) unique NOT NULL, ctime datetime NOT NULL DEFAULT CURRENT_TIMESTAMP, name varchar(50) unique NOT NULL, email varchar(50) unique NOT NULL, password varchar(25) NOT NULL, salt varchar(25) NOT NULL, PRIMARY KEY (uid));

create table friend_request(uid varchar(30), user_from varchar(30), ctime DATETIME not null DEFAULT CURRENT_TIMESTAMP); 

create table friend_list(uid varchar(30), friend varchar(30), ctime DATETIME not null DEFAULT CURRENT_TIMESTAMP, foreign key (uid) references user(uid), foreign key (friend) references user(uid));

create table message(uid varchar(30), frm varchar(30), ctime DATETIME not null DEFAULT CURRENT_TIMESTAMP, msg text, foreign key (uid) references user(uid), foreign key(frm) references user(uid));

insert into user set name = "admin",email = "admin@tulipsecure.com",password = "tulipPa55",salt = "verysalty";

