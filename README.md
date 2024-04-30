### MLD PROJECT
Memory leak detection for dynamic memory requested as structure type

### The project is divided into three phases
#### Phase 1
- Create a structure database. The database stores information about each structure declared by the user. Each structure information is a record in the database, so each row of records saves the name, size, and number of members of the structure. , and all member information (all member information uses a member structure pointer to point to the first element in a member structure array)
- The member structure contains the name, size, offset, type and nested structure name (if this attribute is a certain structure type, this value is the name of the structure type)
- Members currently only support the following types:
UINT8,UINT32,INT32,CHAR,OBJ_PTR,FLOAT,DOUBLE,OBJ_STRUCT
