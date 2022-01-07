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

        public DevicePage()
        {
            InitializeComponent();

            if (Device == null)
                return;

            Title = Device.Name;

            if (Device.HasFeature("OnOff"))
            {
                string btnText = "Off";
                if (bool.Parse(Device.GetValue("on"))
                {
                    Button btnOnOff = new Button
                    {

                    };
                }
            }

            if (Device.HasFeature("SingleColor"))
            {
                // add brightness slider
                Slider brightness = new Slider{
                    Minimum = 0,
                    Maximim = 255,
                    Value = int.Parse(Device.GetValue("brightness"));
                };
                brightness.ValueChanged += brightnessChanged;
                featuresList.Children.Add(brightness);
            }
            elseif (Device.HasFeature("SolidColor"))
            {
                // add brightness slider 
                Slider brightness = new Slider{
                    Minimum = 0,
                    Maximim = 255,
                    Value = int.Parse(Device.GetValue("brightness"));
                };
                brightness.ValueChanged += brightnessChanged;
                featuresList.Children.Add(brightness);
            }
        }

        private void brightnessChanged(object sender, ValueChangedEventArgs e)
        {
            TcpClient client = new TcpClient();
            client.Connect(IPAddress, 3001);

            NetworkStream nStream = client.GetStream();

            byte[] data = Syste.Text.Encoding.ASCII.GetBytes("{ 'brightness' : '" + e.Value.ToString() + "' }")

            nStream.Write(data, 0, data.Length);

            nStream.Close();
            client.Close();
        }
    }
}