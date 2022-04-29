using HookInjector;
DllInjectionResult res = DllInjector.GetInstance.Inject("Bionicle", @$"D:\Documents\GitHub\HeroHookDLL\HeroHookDLL\Release\HeroHookDLL.dll");
Console.WriteLine(@$"{Directory.GetCurrentDirectory()}\HeroHookDLL.dll");
Console.WriteLine($"Injection Result {res}");
Console.ReadKey();