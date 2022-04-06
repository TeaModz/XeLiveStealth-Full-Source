using System;

namespace XeLive.ClientHandler
{
    class Program {
        static void Main(string[] args) {
            Console.ForegroundColor = ConsoleColor.White;

            try {
                // handles unbanning banned ips from firewall after an hour
                new FirewallBanHandler().Start();

                // handles incoming packers
                new ClientHandler().Start();
            }
            catch(Exception ex) {
                Console.WriteLine($"Startup Exception: {ex.Message}");
            }
        }
    }
}
