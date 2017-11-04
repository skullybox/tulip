# database configuration settings

CREATE TABLE user (uid varchar(30) unique NOT NULL, ctime datetime NOT NULL DEFAULT CURRENT_TIMESTAMP, name varchar(50) unique NOT NULL, email varchar(50) unique NOT NULL, password varchar(25) NOT NULL, salt varchar(25) NOT NULL, PRIMARY KEY (uid));

create table friend_request(rowid bigint not null auto_increment, uid varchar(30), user_from varchar(30), ctime DATETIME not null DEFAULT CURRENT_TIMESTAMP, PRIMARY KEY(rowid)); 

create table friend_list(rowid bigint not null auto_increment, uid varchar(30), friend varchar(30), ctime DATETIME not null DEFAULT CURRENT_TIMESTAMP, foreign key (uid) references user(uid), foreign key (friend) references user(uid), PRIMARY KEY(rowid));

create table message(rowid bigint not null auto_increment, uid varchar(30), frm varchar(30), ctime DATETIME not null DEFAULT CURRENT_TIMESTAMP, msg text, foreign key (uid) references user(uid), foreign key(frm) references user(uid), primary key(rowid));

insert into user set name = "admin",email = "admin@tulipsecure.com",password = "tulipPa55",salt = "verysalty";

