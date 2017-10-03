# database configuration settings

create table user(uid integer not null, username varchar(15) unique not null, name varchar(50) not null, email varchar(50) unique not null, password varchar(24) not null, salt varchar(24) not null, ctime integer not null, primary key(uid));

create table friend_request(uid integer, user_from varchar(15), ctime integer not null);

create table friend_list(uid integer, friend integer, ctime integer not null, foreign key(uid) references user(uid), foreign key(friend) references user(uid));

create table message(msg_id integer not null, uid integer, frm integer, ctime integer not null, msg text, primary key(msg_id), foreign key (uid) references user(uid), foreign key(frm) references user(uid));




