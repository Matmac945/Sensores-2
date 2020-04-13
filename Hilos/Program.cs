using System;
using System.Threading;

namespace Hilos
{
    class Program
    {
        static bool done;
        static readonly object locker = new object();
        static int metcounter = 0;

        public static void Main()
        {
            new Thread (Go).Start();
            Program2 varP2 = new Program2();
            if(metcounter < 1){
                metcounter++;
                varP2.met();  
                Console.WriteLine(metcounter);    
                // metcounter++;
            } 
            Go(); 
        }

        static void Go()
        {
            lock(locker)
            {
                // if (!done) { done = true; Console.WriteLine("Done");}
                if (!done) { Console.WriteLine("Done"); done = true;}
            }
        }
    }

    class Program2
    {
        public void met(){
            Program varX = new Program();
            // Console.WriteLine("Metodo 2");
            Program.Main();
        }
    }
}
