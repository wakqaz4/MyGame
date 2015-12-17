










using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Security.Cryptography;

namespace RSACsharp
{
    class Program
    {

        static void Main(string[] args)
        {
            string[] keys = GenerateKeys();
            string sSource = "aasdfasdfqwerea";
            Console.WriteLine("Original: \n");
            Console.WriteLine(sSource);
            string sEncrypted = EncryptString(sSource, keys[0]);
            Console.WriteLine("After Enctyption: \n");
            Console.WriteLine(sEncrypted);
            string sDecrypted = DecryptString(sEncrypted, keys[1]);
            Console.WriteLine("After Dectyption: \n");
            Console.WriteLine(sDecrypted);
        }
        public static string[] GenerateKeys()
        {
            string[] sKeys = new string[2];
            RSACryptoServiceProvider rsa = new RSACryptoServiceProvider();
            sKeys[0] = rsa.ToXmlString(false);
            sKeys[1] = rsa.ToXmlString(true);
            return sKeys;
        }
        
        public static string EncryptString(string sSource, string sPublicKey)
        {

            RSACryptoServiceProvider rsa = new RSACryptoServiceProvider();
            //Initialize rsa object
            rsa.FromXmlString(sPublicKey);
            byte[] cipherbytes;
            cipherbytes = rsa.Encrypt(Encoding.UTF8.GetBytes(sSource), false);
            return Convert.ToBase64String(cipherbytes);

        }


        public static string DecryptString(String sSource, string sPrivateKey)
        {
            RSACryptoServiceProvider rsa = new RSACryptoServiceProvider();
            rsa.FromXmlString(sPrivateKey);
            //byte[] byteEn = rsa.Encrypt(Encoding.UTF8.GetBytes("a"), false);
            byte[] byteSource = Convert.FromBase64String(sSource);

            byte[] plaintbytes = rsa.Decrypt(byteSource, false);
            return Encoding.UTF8.GetString(plaintbytes);
        }
    }
}
