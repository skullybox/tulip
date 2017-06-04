CREATE TABLE IF NOT EXISTS users (
    id MEDIUMINT NOT NULL AUTO_INCREMENT,
    lastName varchar(255) NOT NULL,
    firstName varchar(255) NOT NULL,
    department varchar(55),
    email varchar(100) NOT NULL, 
    PRIMARY KEY (id)
);
