# protei
project task in c++

REQUIRED: cmake version(at least 3.15), c++ standart(at least c++17)

how to assemble project:
1) enter in protei directory
2) run assemble.sh (it could take some time to compile project ~30s, because of some header only libraries)

After that, there will be server directory with two executable: main and test.
Also, there are server_config.json file in which you can set up parameters of the server

how to use it:
after start server you can take requests to it. (ip: 127.0.0.1 and port: 8080)
in procces of working server there will appear file: client_information.txt,
which contains information about proccess clients
