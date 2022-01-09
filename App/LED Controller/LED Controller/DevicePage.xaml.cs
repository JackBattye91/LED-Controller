using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Net;
using System.Net.Sockets;

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
        }

        protected override void OnAppearing()
        {
            if (Device == null)
                return;

            Title = Device.Name;

            // add on off button
            Button btnOnOff = new Button
            {
                Text = Device.On ? "On" : "Off",
                BackgroundColor = Device.On ?  Color.LightBlue : Color.Gray,
                Margin = new Thickness(5)
            };
            btnOnOff.Clicked += btnOnOff_Clicked;
            featureList.Children.Add(btnOnOff);

            if (Device.HasFeature("SingleColor"))
            {
                if (!Device.Values.ContainsKey("brightness"))
                    Device.Values.Add("brightness", 255);

                // add brightness slider
                Slider brightnessSlider = new Slider()
                {
                    Minimum = 0,
                    Maximum = 255,
                    Value = (int)Device.Values["brightness"]
                };
                Label lblBrightness = new Label()
                {
                    Text = Device.Values["brightness"].ToString(),
                    HorizontalOptions = LayoutOptions.CenterAndExpand,
                    BindingContext = brightnessSlider
                };
                lblBrightness.SetBinding(Label.TextProperty, "Value", BindingMode.Default, null, "{0:F0}");
                brightnessSlider.ValueChanged += brightness_Changed;

                featureList.Children.Add(new Label() { Text = "Brightness", FontAttributes = FontAttributes.Bold, FontSize = 24, HorizontalOptions = LayoutOptions.CenterAndExpand });
                featureList.Children.Add(lblBrightness);
                featureList.Children.Add(brightnessSlider);
            }
            else if (Device.HasFeature("SolidColor"))
            {
                if (!Device.Values.ContainsKey("brightness"))
                    Device.Values.Add("brightness", 255);

                if (!Device.Values.ContainsKey("color"))
                    Device.Values.Add("color", Color.Blue);

                // Color Wheel
                ColorPicker.ColorWheel colorWheel = new ColorPicker.ColorWheel() { SelectedColor = (Color)Device.Values["color"] };

                // add brightness slider
                Slider brightnessSlider = new Slider()
                {
                    Minimum = 0,
                    Maximum = 255,
                    Value = (int)Device.Values["brightness"]
                };
                Label lblBrightness = new Label()
                {
                    Text = Device.Values["brightness"].ToString(),
                    HorizontalOptions = LayoutOptions.CenterAndExpand,
                    BindingContext = brightnessSlider
                };
                lblBrightness.SetBinding(Label.TextProperty, "Value", BindingMode.Default, null, "{0:F0}");
                brightnessSlider.ValueChanged += brightness_Changed;

                featureList.Children.Add(new Label() { Text = "Color", FontAttributes = FontAttributes.Bold, FontSize = 24, HorizontalOptions = LayoutOptions.CenterAndExpand });
                featureList.Children.Add(colorWheel);
                featureList.Children.Add(new Label() { Text = "Brightness", FontAttributes = FontAttributes.Bold, FontSize = 24, HorizontalOptions = LayoutOptions.CenterAndExpand });
                featureList.Children.Add(lblBrightness);
                featureList.Children.Add(brightnessSlider);
                featureList.Children.Add(new Label() { Text = "Text" });
            }
            else if (Device.HasFeature("MultiColor"))
            {
                // add brightness slider
                Slider brightnessSlider = new Slider()
                {
                    Minimum = 0,
                    Maximum = 255,
                    Value = (int)Device.Values["brightness"]
                };
                Label lblBrightness = new Label()
                {
                    Text = Device.Values["brightness"].ToString(),
                    HorizontalOptions = LayoutOptions.CenterAndExpand,
                    BindingContext = brightnessSlider
                };
                lblBrightness.SetBinding(Label.TextProperty, "Value", BindingMode.Default, null, "{0:F0}");
                brightnessSlider.ValueChanged += brightness_Changed;

                featureList.Children.Add(new Label() { Text = "Brightness", FontAttributes = FontAttributes.Bold, FontSize = 24, HorizontalOptions = LayoutOptions.CenterAndExpand });
                featureList.Children.Add(lblBrightness);
                featureList.Children.Add(brightnessSlider);
            }

            base.OnAppearing();
        }

        private void brightness_Changed(object sender, ValueChangedEventArgs e)
        {
            SendValue("brightness");
        }

        private void btnOnOff_Clicked(object sender, EventArgs e)
        {
            Device.On = !Device.On;
           
            (sender as Button).Text = Device.On ? "On" : "Off";
            (sender as Button).BackgroundColor = Device.On ? Color.LightBlue : Color.Gray;
            
            SendString("{ 'on' : " + Device.On.ToString().ToLower() + " }");
        }

        private void SendData(byte[] data)
        {
            try
            {
                TcpClient client = new TcpClient();
                client.Connect(address, 3001);

                using (NetworkStream nStream = client.GetStream())
                {
                    nStream.Write(data, 0, data.Length);
                }

                client.Close();
            }
            catch(Exception e)
            {
                Console.WriteLine(e.Message);
            }
        }

        private void SendString(string str)
        {
            try
            {
                TcpClient client = new TcpClient();
                client.Connect(address, 3001);

                using (NetworkStream nStream = client.GetStream())
                {
                    byte[] data = System.Text.Encoding.ASCII.GetBytes(str);
                    nStream.Write(data, 0, data.Length);
                }

                client.Close();
            }
            catch(Exception e)
            {
                Console.WriteLine(e.Message);
            }
        }

        private void SendValue(string name)
        {
            try
            {
                TcpClient client = new TcpClient();
                client.Connect(address, 3001);

                if (!Device.Values.ContainsKey(name))
                    return;

                using (NetworkStream nStream = client.GetStream())
                {
                    byte[] data = System.Text.Encoding.ASCII.GetBytes("{ '" + name.ToLower() + "' : '" + Device.Values[name].ToString().ToLower() + "' }");
                    nStream.Write(data, 0, data.Length);
                }

                client.Close();
            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
            }
        }
    }
}