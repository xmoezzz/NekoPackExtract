using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using SprinterPublishing;

namespace MNGUnpackerNet
{
    class Program
    {
        static void Main(string[] args)
        {
            if (args.Length != 1)
                return;

            string outdir = Path.ChangeExtension(args[0], null);
            if (!Directory.Exists(outdir))
            {
                Directory.CreateDirectory(outdir);
            }

            var Loader = new MNG();
            Loader.Load(args[0]);
            int Count = Loader.NumEmbeddedPNG;

            for(int i = 0; i < Count; i++)
            {
                var bitmap = Loader.ToBitmap(i);
                bitmap.Save(Path.Combine(outdir, string.Format("{0:0000}.png", i)), System.Drawing.Imaging.ImageFormat.Png);
            }
        }
    }
}
