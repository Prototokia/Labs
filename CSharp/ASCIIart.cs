using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Drawing;
using System.IO;

namespace ASCIIart {
    internal class Program {
        private const double WIDTH_OFFSET = 1.7;
        private const int MAX_WIDTH = 474;

        [STAThread]
        static void Main(string[] args) {

            Console.ForegroundColor = ConsoleColor.Cyan;

            var openFileDialog = new OpenFileDialog {
                Filter = "Images | *.bmp; *.png; *.jpg; *.JPEG"
            };

            Console.WriteLine("Press enter to start");
            
            while (true) {
                Console.ReadLine();

                if (openFileDialog.ShowDialog() != DialogResult.OK) 
                    continue;

                Console.Clear();

                var bitmap = new Bitmap(openFileDialog.FileName);
                bitmap = ResizeBitmap(bitmap);
                bitmap.ToGrayScale();

                var converter = new ToASCIIConverter(bitmap);
                var rows = converter.Convert();

                foreach (var row in rows) 
                    Console.WriteLine(row);

                Console.SetCursorPosition(0, 0);
            }
        }

        private static Bitmap ResizeBitmap(Bitmap bitmap) {
            double maxHeight = bitmap.Height / WIDTH_OFFSET * MAX_WIDTH / bitmap.Width;
            if (bitmap.Width > MAX_WIDTH || bitmap.Height > maxHeight)
                bitmap = new Bitmap(bitmap, new Size(MAX_WIDTH, (int)maxHeight));
            return bitmap;
        }
    }
}
