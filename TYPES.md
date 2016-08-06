# Object Definitions
# ------------------

# Objects defined here are parsed
#  and generated as C objects for
#  serialization and storage

# Objects with id are hashes that are based 
#  on all object members defined
#  below it. id:[object] are ids referencing other
#  object's id value.

# Objects with member names suffixed with a '+'
#  have those members as mutable otherwise
#  all object members are immutable prior to destroying
#  the object.

# Object members with a:[object] signifies a list of
#  objects references ':' separated

# Objects that are owned by other objects
#  can only have one owner but multiple object references
#  in the case where multiple id:[object] are listed
#  the first one listed takes owner ship and can modify
#  this object's modifiable members

-user
 pass+
 id
 fname
 email

-group
 a:user+
 id
 name
 id:user

-msggroup
 id:user
 id
 id:group
 message

-msguser
 id
 id:user
 id:user
 message

-contacts
 id:user
 a:user+


