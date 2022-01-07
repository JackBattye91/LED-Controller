﻿using System;
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

                using (NetworkStream nStream = tcpClient.GetStream())
                {
                    byte[] data = new byte[256];
                    nStream.Read(data, 0, data.Length);
                    string responseText = System.Data.Encoding.ASCII.GetString(data);

                    Dispatcher.BeginInvokeOnMainThread(() =>
                    {
                        Device newDevice = Device.Parse(responseText);
                        devices.Add(ip, newDevice);

                        Button newButton = new Button
                        {
                            Text = newDevice.Name,
                            VerticalOptions = LayoutOptions.CenterAndExpand,
                            HorizontalOptions = LayoutOptions.Center
                        };
                        newbutton.Clicked += (sender, e) => {
                            DevicePage devPage = new DevicePage();
                            devPage.Device = newDevice;
                            devPage.IPAddress = ip;
                            Navigation.PushModalAsync(devPage);
                        };

                        deviceList.Children.Add(newButton);
                    });
                }

                tcpClient.Close();
            }
        }
    }
}
