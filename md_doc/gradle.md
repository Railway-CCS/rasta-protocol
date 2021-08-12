# HowTo - Gradle
Gradle is the build tool we use to compile the project, execute unit tests and a lot of other tasks such as creating the Doxygen documentation. In this section you will learn how to use Gradle to run tasks and how to edit the Gradle config file.

## Using gradle
You can execute a named task by typing
```
gradle <taskname>
```

See the next section for available tasks.

### Tasks
|Taskname           |Description                                |
|-------------------|-------------------------------------------|
|build              |Compiles the RaSTA library and all executables that are specified in the config file. Runs all registered unit tests. Runs the static code analysis tool `cppcheck` and prints it's results|
|clean              |removes all generated binary files which were created during compilation|
|genDoc             |generated the Doxygen documentation in `doc/`|
|genCCHtml          |generated a report about the code coverage of the CUnit tests in `ccHtml`. Open `ccHtml/index.html`to view the reports.|
|valgrindAnalysis   |run the dynamic code analysis tool `valgrind` to detect memory leaks|

## Adding a new executable to Gradle
Assume that you created a program that uses the RaSTA library. You should have created a directory inside `src/` with subdirctories `c/`and (optionally) `headers/`. Inside you `src/<yourprogramname>/c/`should be a file with a `main` function.  
Gradle will not automatically compile your project when you execute `gradle build`. You have to specify that inside the `build.gradle` in the project root directory.  
Although the config file might look pretty complicated at first, it's pretty easy.  
There is a `components` container inside the `model` container.
```
...
model{
    ...
    components{
        ...
        // your program definition needs to go here
    }
}
```

To add a executable to the compile task just add a new container inside `components`
```
<yourprogramname>(NativeExecutableSpec){
       sources.c{
           lib library: "rasta", linkage: "static"
       }
}
```
Where `<yourprogramname>` is the name of the directory you created inside `src/`.  
`NativeExecutableSpec` tells Gradle to build a executable (in contrast `NativeLibrarySpec` will build a library).  
`lib library: "rasta", linkage: "static"` will link the RaSTA library statically to your executable. You can replace `static` with `dynamic` to link the library dynamically.  

Now save the file and run `gradle build`, your executable should be located at `build/exe/<yourprogramname>/<yourprogramname>`.


For further information about creating additional tasks and more, have a look at the [Gradle Native Documentation](https://docs.gradle.org/current/userguide/native_software.html).