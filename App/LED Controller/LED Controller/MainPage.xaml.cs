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

            //Task.Run(FindDevices);

            Device testDevice = new Device() { Name = "Test Device", On = false, State = 1 };
            testDevice.AddFeature("OnOff");
            testDevice.AddFeature("SolidColor");

            Button newButton = new Button
            {
                Text = testDevice.Name,
            };
            newButton.Clicked += (sender, e) => {
                DevicePage devPage = new DevicePage
                {
                    Device = testDevice,
                    address = new IPAddress(new byte[] { 127, 0, 0, 1 })
                };
                Navigation.PushAsync(devPage);
            };

            deviceList.Children.Add(newButton);
        }

        void FindDevices()
        {
            for (byte b = 0; b < 255; b++)
            {
                IPAddress ip = new IPAddress(new byte[] { 192, 168, 1, b });
                TcpClient tcpClient = new TcpClient();
                tcpClient.Connect(ip, 3001);

                using (NetworkStream nStream = tcpClient.GetStream())
                {
                    byte[] data = new byte[256];
                    nStream.Read(data, 0, data.Length);
                    string responseText = System.Text.Encoding.ASCII.GetString(data);

                    Dispatcher.BeginInvokeOnMainThread(() =>
                    {
                        Device newDevice = new Device();
                        newDevice.Parse(responseText);

                        Button newButton = new Button
                        {
                            Text = newDevice.Name,
                            VerticalOptions = LayoutOptions.CenterAndExpand,
                        };
                        newButton.Clicked += (sender, e) => {
                            DevicePage devPage = new DevicePage
                            {
                                Device = newDevice,
                                address = ip
                            };
                            Navigation.PushModalAsync(devPage);
                        };

                        if (deviceList.Children.Contains(lblNoDevices))
                            deviceList.Children.Remove(lblNoDevices);

                        deviceList.Children.Add(newButton);
                    });
                }

                tcpClient.Close();
            }
        }
    }
}
