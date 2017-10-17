# database configuration settings

create table user(uid varchar(30) unique not null, name varchar(50) not null, email varchar(50) unique not null, password varchar(25) not null, salt varchar(25) not null, ctime DATETIME DEFAULT CURRENT_TIMESTAMP, primary key(uid));

create table friend_request(uid varchar(30), user_from varchar(30), ctime DATETIME DEFAULT CURRENT_TIMESTAMP);

create table friend_list(id unsigned big int null, uid varchar(30), friend varchar(30), ctime DATETIME DEFAULT CURRENT_TIMESTAMP, foreign key(uid) references user(uid), foreign key(friend) references user(uid), primary key(id));

create table message(msg_id unsigned big int not null, uid varchar(30), frm varchar(30), ctime DATETIME DEFAULT CURRENT_TIMESTAMP, msg text, primary key(msg_id), foreign key (uid) references user(uid), foreign key(frm) references user(uid));




