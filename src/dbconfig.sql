# database configuration settings

create table user(username varchar(15) unique not null, name varchar(50) not null, email varchar(50) unique not null, password varchar(24) not null, salt varchar(24) not null, ctime integer not null);

create table friend_request(username varchar(15), user_from varchar(15), ctime integer not null);

create table friend_list(username varchar(15), friend varchar(15), ctime integer not null);

create table message(username varchar(15), frm varchar(15), ctime integer not null, msg text);
