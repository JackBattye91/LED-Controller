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

            Device testDevice = new Device() { Name = "Test Device", On = false, State = 1 };
            testDevice.Features |= Device.FEATURE_FLAGS.FEATURE_SOLIDCOLOR;

            Button newButton = new Button
            {
                Text = testDevice.Name
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


            Task.Run(FindDevices);
        }

        void FindDevices()
        {
            WebClient webClient = new WebClient();

            for (byte b = 0; b < 255; b++)
            {
                IPAddress ip = new IPAddress(new byte[] { 192, 168, 1, b });


                webClient.DownloadStringCompleted += (object sender, DownloadStringCompletedEventArgs e) =>
                {
                    try
                    {
                        string responseText = e.Result;

                        Dispatcher.BeginInvokeOnMainThread(() =>
                        {
                            Device newDevice = new Device();
                            newDevice.Parse(responseText);

                            Button newButton = new Button
                            {
                                Text = newDevice.Name
                            };

                            newButton.Clicked += (btnSender, btnE) =>
                            {
                                DevicePage devPage = new DevicePage
                                {
                                    Device = newDevice,
                                    address = ip
                                };
                                Navigation.PushModalAsync(devPage);
                            };

                            // remove label for no devices found
                            if (deviceList.Children.Contains(lblNoDevices))
                                deviceList.Children.Remove(lblNoDevices);

                            // add device to list
                            deviceList.Children.Add(newButton);
                        });
                    }
                    catch (Exception ex)
                    {

                    }
                };

                webClient.DownloadStringAsync(new Uri("http://" + ip.ToString()));
            }
        }
    }
}
