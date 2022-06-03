using HookInjector;
string thing = @"██╗  ██╗███████╗██████╗  ██████╗     ██╗  ██╗ ██████╗  ██████╗ ██╗  ██╗    ██╗███╗   ██╗     ██╗███████╗ ██████╗████████╗ ██████╗ ██████╗ 
██║  ██║██╔════╝██╔══██╗██╔═══██╗    ██║  ██║██╔═══██╗██╔═══██╗██║ ██╔╝    ██║████╗  ██║     ██║██╔════╝██╔════╝╚══██╔══╝██╔═══██╗██╔══██╗
███████║█████╗  ██████╔╝██║   ██║    ███████║██║   ██║██║   ██║█████╔╝     ██║██╔██╗ ██║     ██║█████╗  ██║        ██║   ██║   ██║██████╔╝
██╔══██║██╔══╝  ██╔══██╗██║   ██║    ██╔══██║██║   ██║██║   ██║██╔═██╗     ██║██║╚██╗██║██   ██║██╔══╝  ██║        ██║   ██║   ██║██╔══██╗
██║  ██║███████╗██║  ██║╚██████╔╝    ██║  ██║╚██████╔╝╚██████╔╝██║  ██╗    ██║██║ ╚████║╚█████╔╝███████╗╚██████╗   ██║   ╚██████╔╝██║  ██║
╚═╝  ╚═╝╚══════╝╚═╝  ╚═╝ ╚═════╝     ╚═╝  ╚═╝ ╚═════╝  ╚═════╝ ╚═╝  ╚═╝    ╚═╝╚═╝  ╚═══╝ ╚════╝ ╚══════╝ ╚═════╝   ╚═╝    ╚═════╝ ╚═╝  ╚═╝
                                                        By Nullpo
                                            

";
Console.WindowWidth = 160;
Console.WriteLine(thing);
//DllInjectionResult a = DllInjector.Inject("Bionicle", @$"{Directory.GetCurrentDirectory()}\HeroHookDLL.dll"); 
DllInjectionResult a = DllInjector.Inject("Bionicle", @$"D:\Documents\GitHub\HeroHookDLL\HeroHookDLL\Debug\HeroHookDLL.dll");
Console.WriteLine(@$"{Directory.GetCurrentDirectory()}\HeroHookDLL.dll");
Console.WriteLine($"Injection Result {a.ToString()}");
Console.ReadKey();