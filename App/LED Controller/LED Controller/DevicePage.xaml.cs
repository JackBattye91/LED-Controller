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

            // add on off button
            Button btnOnOff = new Button
            {
                VerticalOptions = LayoutOptions.CenterAndExpand,
                HorizontalOptions = LayoutOptions.Center
            };
            btnOnOff.Text = Device.State == 0 ? "Off" : "On";
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
            Device.Values["brightness"] = e.Value.ToString();

            TcpClient client = new TcpClient();
            client.Connect(IPAddress, 3001);

            using (NetworkStream nStream = client.GetStream())
            {
                byte[] data = System.Text.Encoding.ASCII.GetBytes("{ 'brightness' : '" + Device.Values["brightness"] + "' }");
                nStream.Write(data, 0, data.Length);
            }

            client.Close();
        }

        private void btnOnOff_Clicked(object sender, EventArgs e)
        {
            Device.State = Device.State == 0 ? 1 : 0;

            TcpClient client = new TcpClient();
            client.Connect(IPAddress, 3001);

            using (NetworkStream nStream = client.GetStream())
            {
                byte[] data = System.Text.Encoding.ASCII.GetBytes("{ 'state' : '" + Device.State.ToString() + "' }");
                nStream.Write(data, 0, data.Length);
            }

            client.Close();
        }
    }
}