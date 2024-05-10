# LKM rootkit
 This Linux Kernel Module is developed as a project for the Kernel Implementation subject at PUC Campinas. The module acts as a rootkit, intercepting the compilation or interpretation of C and Python files in the operating system. It searches for a specific and predefined sentence in the code and replaces it with another predefined sentence.

# Compatibility
> [!NOTE]
> This module is designed to work with OS kernel versions lower than v5.7. Compatibility with newer versions is not guaranteed.

# Disclaimer

> [!CAUTION]
> This project is intended for educational purposes only. Improper use or misuse of this software can lead to system instability and damage to the kernel. The developers are not liable for any damages or legal consequences resulting from the use or misuse of this software.

# Usage
### Building the module
To build the module, run:

``` shell
sudo make
```

### Displaying Module Information
To display information about the module, run:

``` shell
modinfo rootkit.ko
```

### Inserting the Module
To insert the module into the system, run:

``` shell
sudo insmod rootkit.ko
```

Now, the module will now take action for every C file or Python file being compiled in the OS.
> [!NOTE]
> You won't be able to see the module running in the OS Kernel because we've hidden it. If you really want to see it, comment down `hideme()` function call in `rootkit.c`.

To list the modules running in the OS Kernel, run:
```shell
lsmod
```

To examine the kernel ring buffer, run:
```shell
dmesg
```
### Testing the module in action
To test the module in action, there are sample files provided: `hello.c` and `main.py`. 
1. For the C file (`hello.c`), compile it by running:
``` shell
sudo gcc hello.c -o hello
./hello
```

2. For the Python file (`main.py`), run:
```shell
python main.py
```

- If the module is working properly, you should see the following output for both files:
```
WARNING: system has been compromised
```


