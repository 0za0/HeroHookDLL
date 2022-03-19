using HookInjector;
DllInjectionResult res = DllInjector.GetInstance.Inject("Bionicle", @"C:\Users\0za0\source\repos\HeroHookDLL\Debug\HeroHookDLL.dll");
Console.WriteLine($"Injection Result {res}");
Console.ReadKey();