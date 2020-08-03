# 视觉SLAM十四讲代码注释整理
书中讲解不细致章节代码注释整理

# 基于vscode调试
## 注：
- 1.cmake的版本，过低不支持cmake调试，本机cmake的版本为3.9.1
- 2.CMakeLists.txt文件中的set(CMAKE_BUILD_TYPE "Debug")设置为Debug模式，Release模式不支持调试
- 3.在vscode中，ctrl+shift+p打开命令行提示窗口
  - 3.1 输入Debug:Open launch.json（创建launch.json文件）
  ```
  {
    // Use IntelliSense to learn about possible attributes.
    // Hover to view descriptions of existing attributes.
    // For more information, visit: https://go.microsoft.com/fwlink/?linkid=830387
    "version": "0.2.0",
    "configurations": [

        {
            "name": "g++ - 生成和调试活动文件",
            "type": "cppdbg",
            "request": "launch",
            "program":"${command:cmake.launchTargetPath}", //cmake固定写法
            "args": ["-input","./data/problem-16-22106-pre.txt"], //程序运行时需要的参数
            "stopAtEntry": false,
            "cwd": "${workspaceFolder}",
            "environment": [],
            "externalConsole": false,
            "MIMode": "gdb",
            "setupCommands": [
                {
                    "description": "为 gdb 启用整齐打印",
                    "text": "-enable-pretty-printing",
                    "ignoreFailures": true
                }
            ],
           "preLaunchTask": "make",  //与tasks.json中对应
            "miDebuggerPath": "/usr/bin/gdb"
        }
    ]
  }
  ```
  - 3.2 输入Tasks:Configure Task（创建tasks.json文件）
  ```
  {
    "version": "2.0.0",
    "options": {
      "cwd": "${workspaceRoot}/build"
    }, 
    "tasks": [
      {
        "label": "cmake",
        "type": "shell",
        "command": "cmake",
        "args": ["-G", "Unix Makefiles", "-DCMAKE_BUILD_TYPE=Debug", ".."]
      },
      {
        "label": "make", 
        "group": {
          "kind": "build",
          "isDefault": true
        },
        "type": "shell",
        "command": "make",
        "args": []
      }
    ]
  }
  ```
4.在main函数中设置断点启动调试即可
