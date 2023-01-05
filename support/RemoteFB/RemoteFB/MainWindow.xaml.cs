using DossySerialPort;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace RemoteFB
{

    /// <summary>
    /// FxCop requires all Marshalled functions to be in a class called NativeMethods.
    /// </summary>
    internal static class NativeMethods
    {
        [DllImport("gdi32.dll")]
        [return: MarshalAs(UnmanagedType.Bool)]
        internal static extern bool DeleteObject(IntPtr hObject);
    }

    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            const int W = 1024;

            try
            {
                using (var bm = new Bitmap(W, 200))
                {


                    using (var s = new DossySerial("COM8", 115200))
                    {
                        s.Write(new byte[] { 0xD, 65 }, 0, 2, 1000);

                        byte[] buf = new byte[W / 4];



                        for (int y = 0; y < 200; y++)
                        {
                            int n = s.Read(buf, W / 4, 1000);
                            if (n != W / 4)
                            {
                                MessageBox.Show($"BOP {n}");
                                return;
                            }

                            int x = 0;
                            for (int j = 0; j < W / 4; j++)
                            {
                                byte b = buf[j];
                                for (int i = 0; i < 4; i++)
                                {
                                    bm.SetPixel(x++, y, ((b & 0x01) != 0) ? System.Drawing.Color.White : System.Drawing.Color.Black);
                                    b = (byte)(b >> 2);
                                }

                            }

                        }

                        piccer.Source = ToBitmapSource(bm);

                    }
                }
            }
            catch (Exception ex) {

                MessageBox.Show(ex.ToString());
            }

        }


        /// <summary>
        /// Converts a <see cref="System.Drawing.Bitmap"/> into a WPF <see cref="BitmapSource"/>.
        /// </summary>
        /// <remarks>Uses GDI to do the conversion. Hence the call to the marshalled DeleteObject.
        /// </remarks>
        /// <param name="source">The source bitmap.</param>
        /// <returns>A BitmapSource</returns>
        public BitmapSource ToBitmapSource(System.Drawing.Bitmap source)
        {
            BitmapSource bitSrc = null;

            var hBitmap = source.GetHbitmap();

            try
            {
                bitSrc = System.Windows.Interop.Imaging.CreateBitmapSourceFromHBitmap(
                    hBitmap,
                    IntPtr.Zero,
                    Int32Rect.Empty,
                    BitmapSizeOptions.FromEmptyOptions());
            }
            catch (Win32Exception)
            {
                bitSrc = null;
            }
            finally
            {
                NativeMethods.DeleteObject(hBitmap);
            }

            return bitSrc;
        }
    }
}
