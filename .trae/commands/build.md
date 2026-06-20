---
name: build
description: build this project.
---
if you have changed the moc files, you need to delete the temp directory,then, build the project.
what to do :
 - tell the user am I building the project.
   - make a CmakeLists.txt file.
     - you should get all files the project depends on(include files,source files,headers,etc).
     - you should add all the files to the CmakeLists.txt file.
     - you should add a cmake command to add them to the build dictory(get the path with a cmake command,don't use relative path).
     - you should add a cmake command to add the project name.
     - you should add a cmake command to add the project version.
     - you should add a cmake command to add the project description.
   - build the project.
   - compile the project.
 - don't do anything else.
how to build :
 - use the command "cmake .." in the directory "/build".
 - compile.

 
how to compile :
 - use the command "ninja" in the directory "/build".
 - if the build failed,read the error message.
   - if the error message is about the moc files, you need to delete the temp directory,then, build the project.
   - if the error message is about the other files, you need to check the files.
   - if the error message is about the headers, you need to check the headers.
   - if the error message is about the source files, you need to check the source files.
   - if they're all right, you need to check the CmakeLists.txt.
   - if they're all right, you need to check the code exceedingly detailed(if there's a debug skill,use it).
   - compile.

