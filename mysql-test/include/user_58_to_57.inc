# Create 5.7 mysql.user table layout

ALTER TABLE mysql.user DROP COLUMN Drop_role_priv;
ALTER TABLE mysql.user DROP COLUMN Create_role_priv;

