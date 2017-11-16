# database configuration settings

CREATE TABLE user (uname varchar(30) unique NOT NULL, ctime datetime NOT NULL DEFAULT CURRENT_TIMESTAMP, email varchar(50) unique NOT NULL, password varchar(25) NOT NULL, salt varchar(25) NOT NULL, PRIMARY KEY (uname));

create table friend_request(rowid bigint not null auto_increment, uname varchar(30), user_from varchar(30), ctime DATETIME not null DEFAULT CURRENT_TIMESTAMP, PRIMARY KEY(rowid)); 

create table friend_list(rowid bigint not null auto_increment, uname varchar(30), friend varchar(30), ctime DATETIME not null DEFAULT CURRENT_TIMESTAMP, foreign key (uname) references user(uname), foreign key (friend) references user(uname), PRIMARY KEY(rowid));

create table message(rowid bigint not null auto_increment, uname varchar(30), frm varchar(30), ctime DATETIME not null DEFAULT CURRENT_TIMESTAMP, msg text, new BOOL NOT NULL default TRUE, foreign key (uname) references user(uname), foreign key(frm) references user(uname), primary key(rowid));

insert into user set name = "admin",email = "admin@tulipsecure.com",password = "tulipPa55",salt = "verysalty";

