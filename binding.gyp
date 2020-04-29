{
  "targets": [
    {
      "target_name": "termios",
      "sources":
        [
          "src/termios_basic.cpp",
          "src/node_termios.cpp"
        ],
      "include_dirs" : ['<!(node -e "require(\'nan\')")'],
      "cflags": ["-std=c++11"]
    }
  ]
}
