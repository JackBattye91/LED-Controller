using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Xamarin.Forms;
using System.IO;
using System.Net;
using System.Net.Sockets;

namespace LED_Controller
{
    public partial class MainPage : ContentPage
    {
        public MainPage()
        {
            InitializeComponent();

            Task.Run(FindDevices);
        }

        void FindDevices()
        {
            for (byte b = 0; b < 255; b++)
            {
                IPAddress ip = new IPAddress(new byte[] { 192, 168, 1, b });
                TcpClient tcpClient = new TcpClient();
                tcpClient.Connect(ip, 3001);

                using (StreamReader reader = new StreamReader(tcpClient.GetStream()))
                {
                    Dispatcher.BeginInvokeOnMainThread(() =>
                    {

                    });
                }
            }
        }
    }
}
