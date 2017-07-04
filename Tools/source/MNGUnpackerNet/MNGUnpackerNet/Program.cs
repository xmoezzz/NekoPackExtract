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
            if (args.Length != 1 && args.Length != 2)
                return;

            string outdir = Path.ChangeExtension(args[0], null);
            if (!Directory.Exists(outdir))
            {
                Directory.CreateDirectory(outdir);
            }

            var Loader = new MNG();
            Loader.Load(args[0]);
            int Count = Loader.NumEmbeddedPNG;


            string TextName = Path.ChangeExtension(args[0], ".txt");
            var TextStream = new FileStream(TextName, FileMode.Create, FileAccess.Write);
            var TextWriter = new StreamWriter(TextStream);

            for(int i = 0; i < Count; i++)
            {
                var bitmap = Loader.ToBitmap(i);

                var FileName = Loader.pngs[i].GetName();

                TextWriter.WriteLine(FileName);

                if (FileName != null && FileName.Length > 0 && args.Length == 2)
                    bitmap.Save(Path.Combine(outdir, FileName + ".png"), System.Drawing.Imaging.ImageFormat.Png);
                else
                    bitmap.Save(Path.Combine(outdir, string.Format("{0:0000}.png", i)), System.Drawing.Imaging.ImageFormat.Png);
            }

            TextWriter.Close();
        }
    }
}
