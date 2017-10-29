# database configuration settings

create table user(uid varchar(30) unique not null, name varchar(50) not null, email varchar(50) unique not null, password varchar(25) not null, salt varchar(25) not null, ctime DATETIME DEFAULT now(), primary key(uid));

create table friend_request(uid varchar(30), user_from varchar(30), ctime DATETIME DEFAULT now());

create table friend_list(uid varchar(30), friend varchar(30), ctime DATETIME DEFAULT now(), foreign key(uid) references user(uid), foreign key(friend) references user(uid));

create table message(uid varchar(30), frm varchar(30), ctime DATETIME DEFAULT now(), msg text, foreign key (uid) references user(uid), foreign key(frm) references user(uid));




