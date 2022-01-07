using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Xamarin.Forms;
using Xamarin.Forms.Xaml;

namespace LED_Controller
{
    [XamlCompilation(XamlCompilationOptions.Compile)]
    public partial class DevicePage : ContentPage
    {
        public Device Device;
        public IPAddress address;

        private int previousState;

        public DevicePage()
        {
            InitializeComponent();

            if (Device == null)
                return;

            Title = Device.Name;

            // add on off button
            Button btnOnOff = new Button
            {
                VerticalOptions = LayoutOptions.CenterAndExpand,
                HorizontalOptions = LayoutOptions.Center
            };
            btnOnOff.Text = Device.On == 0 ? "Off" : "On";
            btnOnOff.Clicked += btnOnOff_Clicked;
            featuresList.Children.Add(btnOnOff);

            if (Device.HasFeature("SingleColor"))
            {
                // add brightness slider
                Slider brightness = new Slider{
                    Minimum = 0,
                    Maximim = 255,
                    Value = int.Parse(Device.GetValue("brightness"));
                };
                brightness.ValueChanged += brightness_Changed;
                featuresList.Children.Add(brightness);
            }
            elseif (Device.HasFeature("SolidColor"))
            {
                // Color Wheel


                // add brightness slider
                Slider brightness = new Slider{
                    Minimum = 0,
                    Maximim = 255,
                    Value = int.Parse(Device.GetValue("brightness"));
                };
                brightness.ValueChanged += brightness_Changed;
                featuresList.Children.Add(brightness);
            }
            else if (Device.HasFeature("MultiColor"))
            {
                // add brightness slider 
                Slider brightness = new Slider{
                    Minimum = 0,
                    Maximim = 255,
                    Value = int.Parse(Device.GetValue("brightness"));
                };
                brightness.ValueChanged += brightness_Changed;
                featuresList.Children.Add(brightness);
            }
        }

        private void brightness_Changed(object sender, ValueChangedEventArgs e)
        {
            SendValue("brightness");
        }

        private void btnOnOff_Clicked(object sender, EventArgs e)
        {
            Device.On = !Device.On;
           
            Button btnOnOff = sender as Button;
            if (Device.On)
                btnOnOff.Text = "On";
            else
                btnOnOff.Text = "Off";
            
            SendString("{ 'on' : " + Device.On.ToString().ToLower() + " }");
        }

        private void SendData(byte[] data)
        {
            TcpClient client = new TcpClient();
            client.Connect(IPAddress, 3001);

            using (NetworkStream nStream = client.GetStream())
            {
                nStream.Write(data, 0, data.Length);
            }

            client.Close();
        }

        privte void SendString(string str)
        {
            TcpClient client = new TcpClient();
            client.Connect(IPAddress, 3001);

            using (NetworkStream nStream = client.GetStream())
            {
                byte[] data = System.Text.Encoding.ASCII.GetBytes(str);
                nStream.Write(data, 0, data.Length);
            }

            client.Close();
        }

        private void SendValue(string name)
        {
            TcpClient client = new TcpClient();
            client.Connect(IPAddress, 3001);

            if (!Device.Values.ContainsKey(name))
                return;

            using (NetworkStream nStream = client.GetStream())
            {
                byte[] data = System.Text.Encoding.ASCII.GetBytes("{ '" + name.ToLower() + "' : '" + device.Values[name].ToLower() + "' }");
                nStream.Write(data, 0, data.Length);
            }

            client.Close();
        }
    }
}