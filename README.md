#Audit Server

##Building
1. clone git repository
2. cd to the root of cloned repository
3. run "ant -f auditserver.xml"

##Running
1. create mysql database "audit" and import structure from "SQL-mysql\auditdb.sql"
2. cd to "out\artifacts\AuditServer_jar"
3. add "config.properties" file in that directory with following content:
`url = "jdbc:mysql://localhost:3306/audit?characterSetResults=UTF-8&characterEncoding=UTF-8&useUnicode=yes"`
`user = "username"`
`password = "password"`
4. run "java -jar AuditServer.jar"