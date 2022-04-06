using IPTables.Net.Iptables;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace XeLive.ClientHandler {
    class Utils {
        public static void Command(string command) {
            var psi = new ProcessStartInfo();
            psi.FileName = "/bin/bash";
            psi.Arguments = command;
            psi.RedirectStandardOutput = true;
            psi.UseShellExecute = false;
            psi.CreateNoWindow = true;

            using var process = Process.Start(psi);

            process.Close();
        }

        public static void AddToFirewall(string IP) {
            Command($"/sbin/iptables -A INPUT -s {IP} -p tcp -m tcp --dport 7554 -j DROP");
        }

        public static void RemoveFromFirewall(string IP) {
            Command($"/sbin/iptables -D INPUT -s {IP} -p tcp -m tcp --dport 7554 -j DROP");
        }

        public static long GetTimeStamp() {
            return DateTimeOffset.UtcNow.ToUnixTimeSeconds();
        }

        public static string BytesToString(byte[] Buffer) {
            string str = "";
            for (int i = 0; i < Buffer.Length; i++) str = str + Buffer[i].ToString("X2");
            return str;
        }

        public static byte[] GenerateRandomData(int count) {
            byte[] RandData = new byte[count];
            new Random().NextBytes(RandData);
            return RandData;
        }

        public static void AddStringToArray(ref char[] array, string err) {
            Array.Copy(err.ToCharArray(), 0, array, 0, err.Length);
        }

        public static byte[] StringToByteArray(string str) {
            return Enumerable.Range(0, str.Length).Where(x => x % 2 == 0).Select(x => Convert.ToByte(str.Substring(x, 2), 16)).ToArray();
        }
    }
}
