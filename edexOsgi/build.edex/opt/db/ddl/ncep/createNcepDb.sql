\set ON_ERROR_STOP 1
DROP DATABASE IF EXISTS ncep;
DROP TABLESPACE IF EXISTS ncep;
CREATE TABLESPACE ncep OWNER awipsadmin LOCATION '%{tablespace_dir}%/ncep';
COMMENT ON TABLESPACE ncep IS 'NCEP Database tablespace';
CREATE DATABASE ncep OWNER awipsadmin TABLESPACE ncep;

\c ncep

BEGIN TRANSACTION;
GRANT CONNECT, TEMPORARY ON DATABASE ncep TO awips;

GRANT USAGE ON SCHEMA public to awips; -- Don't grant create
ALTER DEFAULT PRIVILEGES IN SCHEMA public GRANT SELECT, INSERT, UPDATE, DELETE, TRIGGER, TRUNCATE ON TABLES TO awips; -- Don't grant references
ALTER DEFAULT PRIVILEGES IN SCHEMA public GRANT ALL ON SEQUENCES TO awips;
ALTER DEFAULT PRIVILEGES IN SCHEMA public GRANT ALL ON FUNCTIONS TO awips;
ALTER DEFAULT PRIVILEGES IN SCHEMA public GRANT ALL ON TYPES TO awips;
COMMIT TRANSACTION;

