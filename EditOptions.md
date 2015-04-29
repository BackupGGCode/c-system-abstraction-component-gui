# Introduction #

Options; Edit them...


# Details #

EditOptions is a utility which can adjust internal library options (if compiled in).
EditOptions.plugin can be loaded and the public function "EditOptions" may be used to display option editor...

```
(void (*editOptions)(PODBC));
editOptions = (void (*)(PODBC))LoadFunction( "EditOptions.plugin", "EditOptions" )

editOptions( NULL )
editOptions( GetOptionODBC( "database_dsn", version ) );
```

and the ODBC parameter can be null for default; otherwise you specify the database it connects to.


the ODBC should be initialized with ... GetOptionODBC( "DSN", version );
or other methods to set the version if another version other than the default is desired.