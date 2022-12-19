# HeroHookDLL
![HeroHookDLLLogo](https://user-images.githubusercontent.com/43097509/168440310-e47c2372-14fc-4651-b2d3-753b48ccc7af.png)



A DLL Hook for Bionicle Heroes written in C++.
The purpose of this tool is to manipulate and inject code at runtime.
ASM Injections are possible and used for further experimentation and reverse-engineering.
It is also possible to trigger "dead" code sections by forcing a function at a certain memory address to be called.


V2 in development
V2 doesnt rely on an injector anymore, it hijacks another DLL (binkw32.dll)
V2 also uses a dedicated GUI Library and hijacks more parts of the engine/game.
The keyboard and mouse get intercepted.


# TODOs 
- Make keys remappable, seeing as a lot of features are on the numpad only (Working on it)


# Known Bugs
- Coordinate Display causes crash when entering level (Toggle off before exiting or entering level)
- Injector gets flagged as a virus because it manipulated memory (aka. its injecting a DLL)

# Bugs & Feature Requests
Feel free to hit me up on Discord. kansokusha#3132




# Downloads
Head over to the [Releases](https://github.com/0za0/HeroHookDLL/releases/tag/V0.1H) page for the latest release

# Todo
- Fix that it gets flagged as a virus
- Finishing up this ReadMe


# Building
TODO

