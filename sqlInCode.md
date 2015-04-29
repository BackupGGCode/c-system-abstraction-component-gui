# Introduction #

http://stackoverflow.com/questions/15142/what-are-the-pros-and-cons-to-keeping-sql-in-stored-procs-versus-code

in C#; there is very little actual SQL; it is phrases used to bind together other parts that are implemented in



# Details #


in C#; there is very little actual SQL; it is phrases used to bind together other parts that are implemented in datatables.

All database handling code in source control.
Database schema able to update itself.

## Pitfalls of self update ##
if the developer changes a column from one to another, and then back, this is just bad form.  During the course of development this may happen, but should have never happened to the field.

Checking the types of columns may cause problems between tinyint(1) or int(13) translations.

