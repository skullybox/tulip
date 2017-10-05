# database configuration settings

create table user(uid varchar(15) unique not null, name varchar(50) not null, email varchar(50) unique not null, password varchar(25) not null, salt varchar(25) not null, ctime DATETIME DEFAULT CURRENT_TIMESTAMP, primary key(uid));

create table friend_request(uid integer, user_from varchar(15), ctime DATETIME DEFAULT CURRENT_TIMESTAMP);

create table friend_list(uid integer, friend integer, ctime DATETIME DEFAULT CURRENT_TIMESTAMP, foreign key(uid) references user(uid), foreign key(friend) references user(uid));

create table message(msg_id integer not null, uid integer, frm integer, ctime DATETIME DEFAULT CURRENT_TIMESTAMP, msg text, primary key(msg_id), foreign key (uid) references user(uid), foreign key(frm) references user(uid));




