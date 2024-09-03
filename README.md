### Intro
Lightweight C++/CLI utility designed for Windows, aimed at enabling efficient remote administration of windows client machine(s). This tool is optimized for performance, small footprint and size. It interact with a REST/JSON server (*i.e. C&C server*) for communication.

### Application
Administer your windows machine(s) remotely via any REST/json server e.g. [Remote Administration Console](https://github.com/tajiknomi/Remote_Administrative_Console). It uses standard HTTP/json/base64 for communication, appearing as a standard web traffic for router/firewall(s). Whether you need to manage configurations, monitor system, execute commands, upload/download files or folders to/from machine, this app seamlessly integrates into your system without requiring additional software's.

### Features

***System Information***: Gather basic system information i.e. username, computer name, IP address, OS version etc.

***File Manager***: Effortlessly manage your files and directories such as view, copy, paste, and delete.

***Shell Handling***: Invoke ***cmd.exe*** on request which is available in almost every windows machine.

***Upload/Download*** File or Directory: Seamlessly transfer files and directories to any http server (*which accepts files*).

***Archive/Compress File or Directory***: This feature compresses file/directory(s) before uploading it to the HTTP server.

***Execute***: Run program or scripts on the client to automate tasks.

***Notifications***: Stay informed with clear and concise messages or notifications from client about an operation or task.

***Logging***: Track client/server activity and events for troubleshooting.

***Persist (Optional)***: Implement your own persistence mechanism in "*src/operations.cpp::persist section*"

### Usage
Download the latest package from the release section and install it. Go to the installation directory and use the below command to start the service.
```
clienthttp.exe <URL/IP> <port>
```
place the url/ip and port of the Command and Control server.

On server side; you can use [this server app as a command & control unit](https://github.com/tajiknomi/Remote_Administrative_Console/releases/tag/v1.0.1) OR you can use your own REST/json http server.


By default, the app will send hearbeat/alive signal every 500 milliseconds in order to inform the server at *<URL/IP>* that it is alive and will collect the command/instruction from server (*if the server have any instruction/command/data for the client*). You can modify this interval time in operations.cpp (variable ---> *receiveResponse_timeout*).

The details of REST/json request/response are specified in the [REST requests (for advance users)](https://github.com/tajiknomi/Remote_Administrative_Console/blob/main/README.md#rest-requests-for-advance-users).


### Project layout

```
├── clientHTTP                    // The main executable (.exe)
├── filemanager                   // Provide file-manager functionalities (.dll)
├── executeCommands               // Provide shell/execute functionalities (.dll)
├── filetransfer                  // Provide file upload/download functionalities (.dll)
```
To enhance flexibility and maintainability, the project is designed with a modular structure. This allows individual components, such as the filetransfer, executionCommands and filemanager modules to be easily updated or replaced [**without the need for recompilation or exiting the app/service**] on the fly. For instance, if you want to use a different file transfer library, you can simply create a new filetransfer.dll and place it alongside the executable. The application will automatically use the new module [**just make sure that replacement dll must have the same name as the one it's replacing**]

### How to build
The app is intentionally written on windows-7 to provide backward compatibility for older machines as well. CMake is used for generating and building the project.

```
mkdir build & cd build
cmake -A win32 ../ & cmake --build . --target clientHTTP --config Release         // for x86
cmake -A x64 ../ & cmake --build . --target clientHTTP --config Release           // for x64
```

### Dependencies
The **filetransfer** module depends on [libcurl](https://curl.se/libcurl/) and its minimal version is statically linked to filetransfer.dll. Both the x86 and x64 version of libcurl are provided in **curlFileTransfer\lib** directory. In future, dependency of **filetransfer.dll** on **libcurl.lib** may be removed, without effecting the project.

### Notes
Your AntiVirus might complain about the app as it is not signed by the CA provided certificate. You can put the app in AV exception list.

### Disclaimer
This application is designed for personal and administrative use. It is not intended for unauthorized access, data manipulation, or any other malicious activity. Any use of this software for illegal purposes is strictly prohibited. You can use this service in offensive security scenarios on you own machine/network ONLY.
The author disclaims all liability for any misuse or damage caused by the application. Users are solely responsible for their actions and the consequences thereof.

### Contributing

Pull requests are welcome. For major changes, please open an issue first
to discuss what you would like to add/change/modify.
For other issues, please open thread in issue section.

### License

[GNU GENERAL PUBLIC LICENSE](https://www.gnu.org/licenses/gpl-3.0.en.html)
