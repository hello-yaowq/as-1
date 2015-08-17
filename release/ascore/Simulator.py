import ctypes

autosar = ctypes.windll.LoadLibrary('./out/mingw.dll')
autosar.main()
